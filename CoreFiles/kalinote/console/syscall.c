/* KalinoteOS 系统调用 */
#include "../bootpack.h"

int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
	/* 开放给外部程序的系统API */
	/* 这个函数的传入参数的寄存器是按照PUSHAD来写的 */
	int i;
	struct TASK *task = task_now();
	int ds_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) FIFO_ADDR);
	struct FILEINFO *finfo;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct WINDOW *window;
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	int *reg = &eax + 1;	/* eax后面的地址 */
		/* 强行改写通过PUSHAD保存的值，用于返回数值 */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */

	if (edx == 1) {
		//打印字符
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		//打印字符串(以字符编码0结尾)
		cons_putstr0(cons, (char *) ebx + ds_base);
	} else if (edx == 3) {
		//打印字符串(指定长度)
		cons_putstr1(cons, (char *) ebx + ds_base, ecx);
	} else if (edx == 4) {
		//结束程序
		return &(task->tss.esp0);
	} else if (edx == 5) {
		//生成窗口
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags = SHEET_APIWIN;			/* 外部API窗口 */
		sheet_setbuf(sht, (int *) ebx + ds_base, esi, edi, eax);
		window = make_window8(sht, esi, edi, TIT_ACT_DEFAULT, TIT_DEACT_DEFAULT, (char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* 将窗口图层高度指定为当前鼠标所在图层的高度，鼠标移到上层 */
		reg[7] = window->whandle;			/* (通过EAX寄存器)返回窗口句柄((int)sht) */		/* 有空把这里改成直接返回window结构体((int) window) */
	} else if (edx == 6) {
		//在窗口上绘制字符
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		//在窗口上绘制方块
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		/* 初始化内存管理 */
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0;	/* 以16字节为单位 */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		/* 分配内存 */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 以16字节为单位进位取整 */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	} else if (edx == 10) {
		/* 释放内存 */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 以16字节为单位进位取整 */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		/* 绘制像素点 */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		/* 区域刷新窗口 */
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		/* 绘制直线 */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		kal_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0) {
			if (eax > esi) {
				i = eax;
				eax = esi;
				esi = i;
			}
			if (ecx > edi) {
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {
		/* 关闭窗口 */
		taskbar_removewin(((struct WINDOW *)(((struct SHEET *) ebx)->win))->tskwinbtn);
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) {
		/* 键盘数据 */
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					/* FIFO为空，休眠，直到键盘接收到数据 */
					task_sleep(task);
				} else {
					/* FIFO为空，返回-1 */
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) { /* 光标定时器 */
				/* 应用程序运行时不显示光标，因此总是将下次显示用的值置1 */
				timer_init(cons->timer, &task->fifo, 1); /* 置1 */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* 光标ON */
				cons->cur_c = COL_WHITE;
			}
			if (i == 3) {	/* 光标OFF */
				cons->cur_c = -1;
			}
			if (i == 4) {	/* 只关闭命令窗口 */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);	/* 2024～2279 */
				cons->sht = 0;
				io_sti();
			}
			if (256 <= i) { /* 键盘数据（通过任务A） */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		/* 获取定时器 */
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1;	/* 运行自动取消 */
	} else if (edx == 17) {
		/* 设置定时器发送数据 */
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		/* 定时器时间设定 */
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		/* 释放定时器 */
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		/* 蜂鸣器 */
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {
		/* 打开文件 */
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0) {
				break;
			}
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char *) ebx + ds_base,
					(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
			if (finfo != 0) {
				reg[7] = (int) fh;
				fh->buf = (char *) memman_alloc_4k(memman, finfo->size);
				fh->size = finfo->size;
				fh->pos = 0;
				file_loadfile(finfo->clustno, finfo->size, fh->buf, task->fat, (char *) (ADR_DISKIMG + 0x003e00));
			}
		}
	} else if (edx == 22) {
		/* 关闭文件 */
		fh = (struct FILEHANDLE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		/* 文件定位 */
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			fh->pos = ebx;
		} else if (ecx == 1) {
			fh->pos += ebx;
		} else if (ecx == 2) {
			fh->pos = fh->size + ebx;
		}
		if (fh->pos < 0) {
			fh->pos = 0;
		}
		if (fh->pos > fh->size) {
			fh->pos = fh->size;
		}
	} else if (edx == 24) {
		/* 获取文件大小 */
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		/* 文件读取 */
		fh = (struct FILEHANDLE *) eax;
		for (i = 0; i < ecx; i++) {
			if (fh->pos == fh->size) {
				break;
			}
			*((char *) ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) {
		/* 命令行API */
		i = 0;
		for (;;) {
			*((char *) ebx + ds_base + i) =  task->cmdline[i];
			if (task->cmdline[i] == 0) {
				break;
			}
			if (i >= ecx) {
				break;
			}
			i++;
		}
		reg[7] = i;
	} else if (edx == 27) {
		/* 查询语言模式 */
		reg[7] = task->langmode;
	} else if (edx == 28) {
		/* 绘制icon */
		
	} else if (edx == 29) {
		/* 清空命令台 */
		cmd_cls(cons);
	} else if (edx == 30) {
		/* 获取系统时间：年 */
		reg[7] = get_year();
	} else if (edx == 31) {
		/* 获取系统时间：月 */
		reg[7] = get_mon_hex();
	} else if (edx == 32) {
		/* 获取系统时间：星期 */
		reg[7] = get_day_of_week();
	} else if (edx == 33) {
		/* 获取系统时间：日 */
		reg[7] = get_day_of_month();
	} else if (edx == 34) {
		/* 获取系统时间：时 */
		reg[7] = get_hour_hex();
	} else if (edx == 35) {
		/* 获取系统时间：分 */
		reg[7] = get_min_hex();
	} else if (edx == 36) {
		/* 获取系统时间：秒 */
		reg[7] = get_sec_hex();
	} else if (edx == 37) {
		/* 获取系统运行时间(秒) */
		reg[7] = sysinfo->time_counter;
	} else if (edx == 38) {
		/* 以绝对位置显示图片 */
		/* EBX: 窗口句柄	EAX: 文件名	ESI: 图片x	EDI: 图片y */
		int img_info;
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		img_info = read_image_32(((char *) eax + ds_base), esi, edi, sht->bxsize, task->fat, sht->buf);
		if(img_info != -1){
			/* 读取失败不执行此句 */
			sheet_refresh(sht, 0, 0, sht->bxsize, sht->bysize);
		}
	} else if (edx == 39) {
		/* 执行系统指令(向cmd发送键值) */
		unsigned char *p;
        for (p = (unsigned char *) ebx + ds_base; *p != 0; p++) {
            if (*p != 0x0d) {
                io_cli();
                fifo32_put(&task->fifo, *p + 256);
                io_sti();
            }
        }
	}
	return 0;
}

void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col){
	/* 绘制直线 */
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}

	return;
}
