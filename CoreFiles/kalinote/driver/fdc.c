/* 软盘驱动 */
#include "../bootpack.h"

struct FIFO32 *ffifo;		/* FDC task的FIFO */
struct FIFO32 fififo;		/* 用于与 FDC 进行调解的 FIFO */
int ffbuf[32], fibuf[32];
struct FDC fdc;

void fdcstruct_init(void) {
	/* FDC结构体初始化 */
	fdc.mode = 0;
	fdc.cyl = 0;
	fdc.head = 0;
	fdc.sect = 0;
	fdc.sects = 0;
	fdc.st0 = 0;
	return;
}

void fdc_init(void) {
	fdcstruct_init();
	
	/* DMA初始化 */
	io_out8(0x00d6, 0xc0);	/* 将主ch0置于级联模式 */
	io_out8(0x00c0, 0x00);	/* 允许从设备 DMA */
	io_out8(0x000a, 0x06);	/* 屏蔽master的ch2的DMA */
	return;
}

void fdc_initwait(int wait) {
	for(;;) {
		if((io_in8(0x03f4) & wait) == 0) {
			break;
		}
	}
	return;
}

void fdc_sendcmd(int data) {
	for(;;) {
		if((io_in8(0x03f4) & 0xc0) == 0x80) {
			io_out8(0x03f5, data);
			return;
		}
	}
}

void fdc_setdma(void) {
	int addr = ADR_DISKIMG + (fdc.cyl * 36 + fdc.head * 18 + fdc.sect - 1) * 512;

	if(fdc.mode == 1) {
		io_out8(0x000b, 0x06);	/* 请求，地址增加方向，写入内存，ch2 */
	} else if(fdc.mode == 2) {
		io_out8(0x000b, 0x0a);	/* 请求，地址增加方向，读取内存，ch2 */
	}

	io_out8(0x0005, 0xff);	/* 字节设置 */
	io_out8(0x0005, fdc.sects * 2 - 1);

	io_out8(0x0004, addr & 0xff);	/* 内存地址设置 */
	io_out8(0x0004, (addr >> 8) & 0xff);
	io_out8(0x0081, (addr >> 16) & 0xff);

	io_out8(0x000a, 0x02);	/* 取消master的ch2掩码 */
	return;
}

void fdc_sethead(void) {
	fdc_initwait(0x11);
	fdc_sendcmd(0x0f);			/* 寻找 */
	fdc_sendcmd(fdc.head << 2);	/* head */
	fdc_sendcmd(fdc.cyl);		/* 柱 */
	return;
}

void fdc_setcmd(void) {
	fdc_initwait(0x11);

	if(fdc.mode == 1) {	/* 模式 */
		fdc_sendcmd(0xe6);	/* 读取 */
	} else if(fdc.mode == 2) {
		fdc_sendcmd(0xc5);	/* 写入 */
	}

	fdc_sendcmd(fdc.head << 2);	/* 指定FD地址 */
	fdc_sendcmd(fdc.cyl);
	fdc_sendcmd(fdc.head);
	fdc_sendcmd(fdc.sect);

	fdc_sendcmd(0x02);	/* 扇区长度：512B */
	if(fdc.mode == 1) {
		fdc_sendcmd(0x12);	/* 轨道数：18 */
		fdc_sendcmd(0x01);	/* GAP3 */
	} else if(fdc.mode == 2) {
		fdc_sendcmd(0x7f);	/* ? */
		fdc_sendcmd(0x12);	/* ? */
	}
	fdc_sendcmd(0xff);	/* 针对整个轨道扇区大小 */
	return;
}

char fdc_getrstatsub(void) {
	char i;

	for(;;) {
		if((io_in8(0x03f4) & 0xc0) == 0xc0) {
			break;
		}
	}
	i = (char) io_in8(0x03f5);
	return i;
}

void fdc_getint(void) {
	fdc_initwait(0x10);
	fdc_sendcmd(0x08);	/* 获取中断状态 */
	fdc.st0 = fdc_getrstatsub();
	fdc_getrstatsub();	/* 柱末尾：丢弃 */
	return;
}

void fdc_getrstat(void) {
	fdc.st0 = fdc_getrstatsub();
	fdc_getrstatsub();	/* st1 */
	fdc_getrstatsub();	/* st2 */
	fdc_getrstatsub();	/* 柱末尾：丢弃 */
	fdc_getrstatsub();	/* head末尾：丢弃 */
	fdc_getrstatsub();	/* 扇区末尾：丢弃 */
	fdc_getrstatsub();	/* 扇区大小：丢弃 */
	return;
}

int fdc_rdwri(void) {
	struct TASK *task = task_now();
	struct TIMER *timer = 0;
	int err = 0, ret = 0, ph = 0, i;

	fdc_setdma();
	if(fdc.mot == 1) {
		fdc_sethead();
	} else {	/* 等待电机启动及转速稳定 */
		io_out8(0x03f2, 0x1c);	/* 电机启动  */

		timer = timer_alloc();
		timer_init(timer, &fififo, 1);
		timer_settime(timer, 300);
	}

	for(;;) {
		io_cli();
		if(fifo32_status(&fififo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&fififo);
			io_sti();
			if(i == 1) {
				fdc_sethead();
			} else if(i == 6) {
				if(ph == 0) {	/* 搜索完成 */
					fdc_getint();
					if((fdc.st0 & 0xc0) == 0x00) {	/* 正常结束 */
						fdc_setcmd();
						ph = 1;
					} else {	/* 异常状态 */
						ret = 2;
						goto end;
					}
				} else {	/* 命令结束 */
					fdc_getrstat();
					if((fdc.st0 & 0xc0) == 0x00) {		/* 正常结束 */
						ret = 0;
						goto end;
					} else if((fdc.st0 & 0xc0) == 0x40) {	/* 异常结束 */
						if(err > 5) {
							ret = 1;
							goto end;
						} else {
							err++;
							fdc_setcmd();
						}
					} else {	/* 异常状态 */
						ret = 2;
						goto end;
					}
				}
			}
		}
	}

	/* 设置在最后 */
end:
	io_out8(0x000a, 0x06);	/* master 的掩码 ch2 */

	if(fdc.mot != 1) {
		timer_free(timer);
		fdc.mot = 1;
	}
	return ret;
}

/* 将操作调解到 FDC。
  * 使用cli-sti 发送以下数据到ffifo 会发送特定数据到指定的(fifo *)
  * 要操作的位置是来自DISKIMG_ADDR 的磁盘映像的虚拟位置。 
  * 读取：[1 << 12 | 扇区数] [扇区数] [fifo *]
  * 写：[2 << 12 | 扇区数] [扇区数] [fifo *]
  * 返回值：0：正常终止
  * 1：异常终止
  * 2: 以非法指令终止
  * 扇区数可以通过 柱面*36+磁头*18+(sector-1) 来计算。 
*/ 

void fdc_task(void) {
	struct TASK *task = task_now();
	struct FIFO32 *f;
	struct TIMER *timer;	/* 电机控制定时器 */
	int ret, i, j;

	fifo32_init(&fififo, 32, fibuf, task);

	fdc_init();
	io_out8(0x03f2, 0x0c);	/* 停止电机 */
	fdc.mot = 0;

	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);

	for(;;) {
		io_cli();
		if(fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if(i == 1) {	/* 可以停止电机 */
				fdc.mot = 0;
				io_out8(0x03f2, 0x0c);
			} else {	/* 获取到指令 */
				timer_cancel(timer);

				/* 存储在 fdc 结构中 */
				fdc.mode = (char) ((i >> 12) & 0x03);

				j = (short) (i & 0x0fff);
				fdc.cyl = (char) (j / 36);
				fdc.head = (char) (j % 36 / 18);
				fdc.sect = (char) (j % 36 % 18 + 1);
				fdc.sects = fifo32_get(&task->fifo);

				f = (struct FIFO32 *) fifo32_get(&task->fifo);

				if(fdc.mode == 1 || fdc.mode == 2) {
					ret = fdc_rdwri();
					fifo32_put(f, ret);
				} else {	/* 以非法指令结束 */
					fifo32_put(f, 2);
				}

				/* 下次初始化 */
				fdcstruct_init();
				timer_settime(timer, 300);
			}
		}
	}
}

void inthandler26(int *esp) {
	/* 软盘中断程序 */
	io_in8(0x03f4);
	io_out8(PIC0_OCW2, 0x66);	/* 完成 IRQ-06 的通知 */
	fifo32_put(&fififo, 6);
	return;
}
