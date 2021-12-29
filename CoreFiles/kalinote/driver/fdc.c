/* 软盘驱动 */
#include "../bootpack.h"

struct FDC fdc;
struct FIFO32 *fifo_fdc;		/* FDC任务的FIFO */

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
	struct TIMER *timer;
	char mot = 0, ph = 0, err = 1;
	int i;

	/* DMA初始化 */
	fdc_dmainit();

	/* 电机初始化 */
	io_out8(FDC_MOTOR, 0x0c);	/* 电机停止 */
	timer = timer_alloc();
	timer_init(timer, fifo_fdc, 1);

start:
	for (;;) {
		io_cli();
		if (fifo32_status(fifo_fdc) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(fifo_fdc);
			io_sti();

			if(i == 1) {	/* 电机控制 */
				if(mot != 0) {	/* 电机停止 */
					io_out8(FDC_MOTOR, 0x0c);
					mot = 0;
				} else {	/* 磁头调整 */
					fdc_setdma();
					fdc_sethead();
					mot = 1;
					ph = 0;
				}
			} else if(i == 2) {	/* 开始读写 */
				if(mot == 0) {
					io_out8(FDC_MOTOR, 0x1c);	/* 启动电机 */
					timer_settime(timer, 300);
				} else {
					timer_cancel(timer);
					fdc_setdma();
					fdc_sethead();
					ph = 0;
				}
			} else if(i == 6) {	/* FDC中断 */
				if(ph == 0) {	/* 搜索完成 */
					fdc_getint();
					if((fdc.st0 & 0xc0) == 0x00) {	/* 正常结束 */
						fdc_setcmd();
						ph = 1;
						err = 1;
					} else {	/* 异常状态 */
						fdc.st0 = 2;
						goto end;
					}
				} else {	/* 命令完成 */
					fdc_getrstat();
					if((fdc.st0 & 0xc0) == 0x00) {		/* 正常结束 */
						fdc.st0 = 0;
						goto end;
					} else if ((fdc.st0 & 0xc0) == 0x40) {	/* 异常结束 */
						if(err > 5) {
							fdc.st0 = 1;
							goto end;
						} else {
							err++;
							fdc_setcmd();
						}
					} else {	/* 异常状态 */
						fdc.st0 = 2;
						goto end;
					}
				}
			}
		}
	}

end:	/* 恢复 */
	io_out8(DMAC0_SMR, 0x06);
	timer_settime(timer, 300);
	fdc.mode = IO_EXIT;
	goto start;
}


void fdc_tinit(struct TASK *task) {
	struct MEMMAN *mem = (struct MEMMAN *) MEMMAN_ADDR;
	int *fbuf = (int *) memman_alloc(mem, 32 * 4);

	/* 任务设置 */
	task->cons_stack = memman_alloc(mem, 65536);
	task->tss.eip = (int) &fdc_task;
	task->tss.esp = task->cons_stack + 65536;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;

	/* 请求结构和FIFO的初始化 */
	fdc.mode = 0;
	fifo_fdc = &task->fifo;
	fifo32_init(&task->fifo, 32, fbuf, task);

	/* 运行任务 */
	task_run(task, 1, 1);
	return;
}

void inthandler26(int *esp) {
	io_in8(FDC_STAT);			/* 指示FDC识别IRQ CPU */
	io_out8(PIC0_OCW2, 0x66);	/* IRQ-06受理完成 */
	fifo32_put(fifo_fdc, 6);
	return;
}

void fdc_dmainit(void) {
	/* DMA初始化 */
	io_out8(DMAC1_MODE, 0xc0);		/* 将master的ch0设置为联级模式 */
	io_out8(DMAC1_CBAR, 0x00);		/* 运行从DMA */
	io_out8(DMAC0_SMR, 0x06);		/* 主ch2的DMA掩码 */
	return;
}

void fdc_setdma(void) {
	int addr = ADR_DISKIMG + (fdc.cyl * 36 + fdc.head * 18 + fdc.sect - 1) * 512;

	if(fdc.mode == IO_READ) {
		io_out8(DMAC0_MODE, 0x06);	/* 需求地址增加方向·存储器的写入·ch2 */
	} else if (fdc.mode == IO_WRITE) {
		io_out8(DMAC0_MODE, 0x0a);	/* 需求地址增加方向·从存储器读取·ch2 */
	}

	io_out8(DMAC0_CBAR, addr & 0xff);	/* 设置存储地址 */
	io_out8(DMAC0_CBAR, (addr >> 8) & 0xff);
	io_out8(DMAP_ADDRUP, (addr >> 16) & 0xff);

	io_out8(DMAC0_CBCR, 0xff);	/* 字节数设置 */
	io_out8(DMAC0_CBCR, fdc.sects * 2 - 1);

	io_out8(DMAC0_SMR, 0x02);	/* 解除master ch2的mask */
	return;
}

void fdc_sethead(void) {
	fdc_initwait(0x11);
	fdc_sendcmd(0x0f);			/* 搜索 */
	fdc_sendcmd(fdc.head << 2);	/* 磁头 */
	fdc_sendcmd(fdc.cyl);		/* 柱面 */
	return;
}

void fdc_setcmd(void) {
	fdc_initwait(0x11);

	if(fdc.mode == IO_READ) {
		fdc_sendcmd(0xe6);	/* 读取 */
	} else if (fdc.mode == IO_WRITE) {
		fdc_sendcmd(0xc5);	/* 写入 */
	}

	fdc_sendcmd(fdc.head << 2);	/* 指定FD编号 */
	fdc_sendcmd(fdc.cyl);
	fdc_sendcmd(fdc.head);
	fdc_sendcmd(fdc.sect);

	fdc_sendcmd(0x02);	/* 扇区长度：512B */
	if(fdc.mode == IO_READ) {
		fdc_sendcmd(0x12);	/* 18 */
		fdc_sendcmd(0x01);	/* GAP3 */
	} else if(fdc.mode == IO_WRITE) {
		fdc_sendcmd(0x7f);	/* ? */
		fdc_sendcmd(0x12);	/* ? */
	}
	fdc_sendcmd(0xff);	/* 目标为整个指定扇区大小 */
	return;
}

void fdc_getint(void) {
	fdc_initwait(0x10);
	fdc_sendcmd(0x08);	/* 中断状态获取 */
	fdc.st0 = fdc_getrstatsub();
	fdc_getrstatsub();	/* 结束时的地址：柱面 */
	return;
}

void fdc_getrstat(void) {
	char i;

	fdc.st0 = fdc_getrstatsub();
	i = fdc_getrstatsub();	/* st1 */
	i = fdc_getrstatsub();	/* st2 */
	i = fdc_getrstatsub();	/* 结束时的地址：柱面 */
	i = fdc_getrstatsub();	/* 结束时的地址：磁头 */
	i = fdc_getrstatsub();	/* 结束时的地址：扇区 */
	i = fdc_getrstatsub();	/* 扇区大小 */
	return;
}

char fdc_getrstatsub(void) {
	char i;

	for(;;) {
		if((io_in8(FDC_STAT) & 0xc0) == 0xc0) {
			i = (char) io_in8(FDC_DATA);
			return i;
		}
	}
}

void fdc_initwait(int wait) {
	for(;;) {
		if((io_in8(FDC_STAT) & wait) == 0) {
			return;
		}
	}
}

void fdc_sendcmd(int data) {
	for(;;) {
		if((io_in8(FDC_STAT) & 0xc0) == 0x80) {
			io_out8(FDC_DATA, data);
			return;
		}
	}
}

void fdc_req(char mode, char cyl, char head, char sect, char sects) {
	for(;;) {
		if(fdc.mode == 0) {
			fdc.mode = mode;
			fdc.cyl = cyl;
			fdc.head = head;
			fdc.sect = sect;
			fdc.sects = sects;
			return;
		}
	}
}

int fdc_reqstat(void) {
	fifo32_put(fifo_fdc, 2);
	for(;;) {
		if(fdc.mode == IO_EXIT) {
			fdc.mode = 0;
			return (int) fdc.st0;
		} else {
			timer_sleep(1);
		}
	}
}
