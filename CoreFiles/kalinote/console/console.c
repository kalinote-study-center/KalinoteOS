/* 命令窗口相关程序 */

#include "../bootpack.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void console_task(struct SHEET *sheet, unsigned int memtotal){
	/* 命令行代码 */
	struct TASK *task = task_now(); 								 /* 获取到自身task地址 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	struct FILEHANDLE fhandle[8];
	char cmdline[30];
	unsigned char *hzk = (char *) *((int *) 0x0fe8);
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);

	cons.sht = sheet;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	task->cons = &cons;
	task->cmdline = cmdline;

	if (cons.sht != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200)); /* 解码FAT，(ADR_DISKIMG + 0x000200)是FAT表在内存中的位置 */
	for (i = 0; i < 8; i++) {
		fhandle[i].buf = 0;	/* 未使用标记符 */
	}
	task->fhandle = fhandle;
	task->fat = fat;
	if (hzk[4096] != 0xff) {	/* 是否载入了字库？ */
		/* 默认使用中文 */
		task->langmode = 1;
	} else {
		task->langmode = 0;
	}
	task->langbyte1 = 0;

	/*
	* 这里关于系统模式切换还需要改进
	*/
	if(cons.sht->flags != SHEET_DEBUG_CONS){
		/* DEBUG窗口不操作 */
		if (sysinfo->sysmode == 0) {
			/* 普通模式 */
				cons_putchar(&cons, '>', 1);			//命令提示符
		} else if (sysinfo->sysmode == 1) {
			/* 调试模式 */
			cons_putchar(&cons, 10001, 1);
		} else {
			/* 未知系统模式 */
			cons_putchar(&cons, 10002, 1);
		}
	}	

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			/* FIFO缓冲区没有数据，任务休眠 */
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) {  /* 光标定时器 */
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0); /* 置0 */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL_WHITE;
					}
				} else {
					timer_init(cons.timer, &task->fifo, 1); /* 置1 */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL_BLACK;
					}
				}
				timer_settime(cons.timer, 50);
			}
			if (i == 2) {	/* 开启光标 */
				cons.cur_c = COL_WHITE;
			}
			if (i == 3) {	/* 关闭光标 */
				if (cons.sht != 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL_BLACK,
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				cons.cur_c = -1;
			}
			if (i == 4) {	/* 命令窗口的X被点击 */
				cmd_exit(&cons, fat);
			}
			if (256 <= i && i <= 511 && cons.sht->flags != SHEET_DEBUG_CONS) { /* 键盘数据(通过任务A)，debug窗口不接受键盘数据(暂时) */
				if (i == 8 + 256) {
					/* backspace */
					/*
					*****************************这里还需要修改*****************************
					*/
					if (sysinfo->sysmode == 0) {
						/* 普通模式 */
						if (cons.cur_x > 2 * 8) {
							/* 用空格键把光标消去后，前移一次光标 */
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}
					} else if (sysinfo->sysmode == 1) {
						/* 调试模式 */
						if (cons.cur_x > 8 * 8) {
							/* 用空格键把光标消去后，前移一次光标 */
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}
					} else {
						/* 未知系统模式 */
						if (cons.cur_x > 16 * 8) {
							/* 用空格键把光标消去后，前移一次光标 */
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}
					}
				} else if (i == 10 + 256) {
					/* Enter */
					/* 用空格将光标擦除 */
					cons_putchar(&cons, ' ', 0);
					if (sysinfo->sysmode == 0) {
						/* 普通模式 */
						cmdline[cons.cur_x / 8 - 2] = 0;		//命令提示符
					} else if (sysinfo->sysmode == 1) {
						/* 调试模式 */
						cmdline[cons.cur_x / 8 - 8] = 0;
					} else {
						/* 未知系统模式 */
						cmdline[cons.cur_x / 8 - 16] = 0;
					}
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal, sysinfo->sysmode);	/* 运行命令 */
					if (sheet == 0) {
						cmd_exit(&cons, fat);
					}
					/* 显示提示符 */
					if (sysinfo->sysmode == 0) {
						/* 普通模式 */
						cons_putchar(&cons, '>', 1);			//命令提示符
					} else if (sysinfo->sysmode == 1) {
						/* 调试模式 */
						cons_putchar(&cons, 10001, 1);
					} else {
						/* 未知系统模式 */
						cons_putchar(&cons, 10002, 1);
					}
				} else {
					/* 一般字符 */
					//if (cons.cur_x < 240) {
					if (cons.cur_x < 512) {
						/* 显示一个字符后将光标后移一位 */
						if (sysinfo->sysmode == 0) {
							/* 普通模式 */
							cmdline[cons.cur_x / 8 - 2] = i - 256;		//命令提示符
						} else if (sysinfo->sysmode == 1) {
							/* 调试模式 */
							cmdline[cons.cur_x / 8 - 8] = i - 256;
						} else {
							/* 未知系统模式 */
							cmdline[cons.cur_x / 8 - 16] = i - 256;
						}
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			/* 重新显示光标，debug窗口不显示光标 */
			if (sheet != 0 && cons.sht->flags != SHEET_DEBUG_CONS) {
				if (cons.cur_c >= 0) {
					boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, 
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move){
	/*
	* 这里关于系统模式切换还需要改进
	*/
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	/* 制表符 */
		for (;;) {
			if (cons->sht != 0) {
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, " ", 1);
			}
			cons->cur_x += 8;
			//if (cons->cur_x == 8 + 240) {
			if (cons->cur_x == 8 + 512) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				break;	/* 被32整除则break */
			}
		}
	} else if (s[0] == 0x0a) {	/* 换行 */
		cons_newline(cons);
	} else if (s[0] == 0x0d) {	/* 回车 */
		/* 这里暂时不进行任何操作 */
	} else if (chr == 10001) {	/* 系统调试模式 */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, "DEBUG >", 7);
		}
		if (move != 0) {
			/* move为0时光标不后移 */
			cons->cur_x += 8 * 7;
			if (cons->cur_x == 56 + 464) {
				cons_newline(cons);
			}
		}
	} else if (chr == 10002) {	/* 系统未知模式 */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, "UNKNOWSYSMODE >", 15);
		}
		if (move != 0) {
			/* move为0时光标不后移 */
			cons->cur_x += 8 * 15;
			if (cons->cur_x == 120 + 400) {
				cons_newline(cons);
			}
		}
	} else {	/* 一般字符，系统为正常模式 */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, s, 1);
		}
		if (move != 0) {
			/* move为0时光标不后移 */
			cons->cur_x += 8;
			//if (cons->cur_x == 8 + 240) {
			if (cons->cur_x == 8 + 512) {
				cons_newline(cons);
			}
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	struct TASK *task = task_now();
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	if (cons->cur_y < 28 + 432) {
		cons->cur_y += 16; /* 换行 */
	} else {
		/* 滚动 */
		if (sheet != 0) {
			for (y = 28; y < 28 + 432; y++) {
				for (x = 8; x < 8 + 512; x++) {
					sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
				}
			}
			for (y = 28 + 432; y < 28 + 448; y++) {
				for (x = 8; x < 8 + 512; x++) {
					sheet->buf[x + y * sheet->bxsize] = COL_BLACK;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 512, 28 + 448);
		}
	}
	
	if (sysinfo->sysmode == 0) {
		/* 普通模式 */
		cons->cur_x = 8;
	} else if (sysinfo->sysmode == 1) {
		/* 调试模式 */
		//cons->cur_x = 8 * 7;
		cons->cur_x = 8;
	} else {
		/* 未知系统模式 */
		//cons->cur_x = 8 * 15;
		cons->cur_x = 8;
	}
	if (task->langmode == 1 && task->langbyte1 != 0) {
		cons->cur_x = 16;
	}
	
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s){
	/* 显示字符串：结尾字符编码0时停止 */
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l){
	/* 显示字符串：指定长度并显示 */
	int i;
	for (i = 0; i < l; i++) {
		cons_putchar(cons, s[i], 1);
	}
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal, char sysmode){
	if (strcmp(cmdline, "mem") == 0) {
		cmd_mem(cons, memtotal);
	} else if (((strcmp(cmdline, "cls") == 0) || (strcmp(cmdline, "clear") == 0)) && cons->sht != 0) {
		cmd_cls(cons);
	} else if (((strcmp(cmdline, "dir") == 0) || (strcmp(cmdline, "ls") == 0)) && cons->sht != 0) {
		cmd_dir(cons);
	} else if (strncmp(cmdline, "type ", 5) == 0 && cons->sht != 0) {
		cmd_type(cons, fat, cmdline);
	} else if (strcmp(cmdline, "exit") == 0) {
		if(cons->sht->flags == SHEET_DEBUG_CONS) {
		/* debug窗口不处理 */
			cons_putstr0(cons, "cannot run this command in debug console\n");
			return;
		}
		cmd_exit(cons, fat);
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		if(cons->sht->flags == SHEET_DEBUG_CONS) {
		/* debug窗口不处理 */
			cons_putstr0(cons, "cannot run this command in debug console\n");
			return;
		}
		cmd_start(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "run ", 4) == 0) {
		if(cons->sht->flags == SHEET_DEBUG_CONS) {
		/* debug窗口不处理 */
			cons_putstr0(cons, "cannot run this command in debug console\n");
			return;
		}
		cmd_run(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "langmode ", 9) == 0) {
		cmd_langmode(cons, cmdline);
	} else if (strcmp(cmdline, "shutdown") == 0) {
		cmd_shutdown();
	} else if (strncmp(cmdline, "sysmode ", 8) == 0) {
		cmd_sysmode(cons, cmdline);
	} else if (strncmp(cmdline, "echo ", 5) == 0) {
		cmd_echo(cons, cmdline);
	} else if (strcmp(cmdline, "hdnum") == 0) {
		cmd_hdnum(cons);
	} else if (strncmp(cmdline, "hdinfo ", 7) == 0) {
		cmd_hdinfo(cons, cmdline);
	} else if (strcmp(cmdline,"test") == 0) {
		/* 测试专用 */
		cmd_systest(cons);
	} else if (cmdline[0] != 0) {
		/* 执行cmd_app(),如果不是一个应用，会返回0 */
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* 不是内部或外部命令 */
			cons_putstr0(cons, "\"");
			cons_putstr0(cons, cmdline);
			cons_putstr0(cons, "\" is not a command.\n\n");
		}
	}
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline){
	/* 搜索并执行应用程序 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	char name[18], *p, *q;
	struct TASK *task = task_now();
	int i, segsiz, datsiz, esp, datkal;
	struct SHTCTL *shtctl;
	struct SHEET *sht;

	/* 根据命令生成文件名 */
	for (i = 0; i < 13; i++) {
		/* 8字节文件名和三个字节的后缀，中间加个点，一共12字节 */
		if (cmdline[i] <= ' ') {/* 非字符或符号 */
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0; /* 暂且将文件名后面置0 */

	/* 寻找文件 */
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		/* 找不到的话，在文件末尾加上.KAL继续查找 */
		name[i    ] = '.';
		name[i + 1] = 'K';
		name[i + 2] = 'A';
		name[i + 3] = 'L';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo != 0) {
		/* 找到文件的情况 */
		p = (char *) memman_alloc_4k(memman, finfo->size);											/* 给应用程序分配一段空内存空间(读取应用) */
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));		/* 加载应用程序文件 */
		if (finfo->size >= 36 && strncmp(p + 4, "Kali", 4) == 0 && *p == 0x00) {
			/*  这里的几个判断，第一个是kal头文件为36字节，所以kal应用程序一定会大于36字节，第二个是检查是否有kal应用签名 */
			/* 处理kal文件头(这里以后可能还需要修改一下) */
			segsiz = *((int *) (p + 0x0000));					/* stack+.data+heap(数据段，函数外定义的数据，以及字符串等)的大小(4K的倍数) */
			esp    = *((int *) (p + 0x000c));					/* 堆栈初始值和.data传输目的地 */
			datsiz = *((int *) (p + 0x0010));					/* .data的大小 */
			datkal = *((int *) (p + 0x0014));					/* .data的初始值列在文件中的位置 */
			q = (char *) memman_alloc_4k(memman, segsiz);		/* 分配应用程序段内存空间(数据段) */
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);				/* 段定义加上0x60(01100000)可以将该段权限设置为应用程序使用 */	/* 可读可执行不可写 */
			set_segmdesc(task->ldt + 1, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);				/* 可读写不可执行 */
			for (i = 0; i < datsiz; i++) {
				/* 将Kal应用中的数据部分复制到数据段 */
				q[esp + i] = p[datkal + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));							/* 调用汇编写的用于启动应用程序的函数，其中0x1b是KaliMain程序入口(这里有一个JMP指令，跳转到真正的程序入口) */
			shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if (sht->flags == SHEET_APIWIN && sht->task == task) {
					/* 找到应用程序残留的窗口 */
					sheet_free(sht);	/* 关闭 */
				}
			}
			for (i = 0; i < 8; i++) {	/* 将未关闭的文件关闭 */
				if (task->fhandle[i].buf != 0) {
					memman_free_4k(memman, (int) task->fhandle[i].buf, task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}
			timer_cancelall(&task->fifo);
			memman_free_4k(memman, (int) q, segsiz);
			task->langbyte1 = 0;
		} else {
			if (task->langmode == 1) {
				/* 中文模式 */
				cons_putstr0(cons,"不支持的文件格式。");
			} else {
				/* 英文或日文模式 */
				cons_putstr0(cons, "\nUnrecognized file format.\n");
		}
		}
		memman_free_4k(memman, (int) p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	/* 没有找到文件的情况 */
	return 0;
}

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
	int *reg = &eax + 1;	/* eax后面的地址 */
		/* 强行改写通过PUSHAD保存的值 */
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
		reg[7] = window->whandle;			/* (通过EAX寄存器)返回窗口句柄((int)sht) */
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
	}
	return 0;
}

int *inthandler0d(int *esp){
	/* CPU异常中断 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg,"********************一般保护异常********************\n   系统在尝试运行应用时遇到了错误。\n   一般保护异常通常出现在用户程序企图访问不可访问的地址时。如果该错误第一次出现，请尝试重新启动该应用程序，如果该错误反复出现，请联系软件的开发者。\n   下面是此次错误的信息：\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x0D :\n 一般保护异常\n");
	} else {
		/* 非中文模式 */
		sprintf(msg,"*****************General protection exception*****************\n   The system encountered an error while trying to run the application.\n   General protection exception usually occurs when the user program attempts to access an inaccessible address. If the error occurs for the first time, please try to restart the application. If the error occurs repeatedly, please contact the software developer. \n   the following is the error message:\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0D :\n General Protected Exception\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);/* esp11号是EIP，详细esp列表在书上第451页 */
	cons_putstr0(cons, s);
	
	return &(task->tss.esp0);	/* 强制结束程序 */
}

int *inthandler0c(int *esp){
	/* 栈异常中断 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg,"********************栈异常********************\n   系统在尝试运行应用时遇到了错误。\n   栈异常通常出现在用户程序错误访问数据栈(如数组成员下标越界等)。如果该错误第一次出现，请尝试重新启动该应用程序，如果该错误反复出现，请联系软件的开发者。\n   下面是此次错误的信息：\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x0C :\n 栈异常\n");
	} else {
		sprintf(msg,"********************Stack Exception********************\n   The system encountered an error while trying to run the application. \n   stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds). If this error occurs for the first time, please try to restart the application. If this error occurs repeatedly, please contact the software developer. \n the following is the error message:\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0C :\n Stack Exception.\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* 强制结束程序 */
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

void keywin_off(struct SHEET *key_win){
	if (key_win->flags != SHEET_NO_TITLE && key_win->flags < 100) {
		/* 不是无标题栏窗口，且flags小于100(100以后是特殊图层) */
		change_wtitle8((struct WINDOW *)(key_win->win), 0);
		if (key_win->flags == SHEET_CONS) {
			/* 命令行窗口 */
			fifo32_put(&key_win->task->fifo, 3); /* 命令行窗口光标OFF */
		}
	}
	return;
}

void keywin_on(struct SHEET *key_win){
	if (key_win->flags != SHEET_NO_TITLE && key_win->flags < 100){
		/* 不是无标题栏窗口，且flags小于100(100以后是特殊图层) */
		change_wtitle8((struct WINDOW *)(key_win->win), 1);
		if (key_win->flags == SHEET_CONS) {
			/* 命令行窗口 */
			fifo32_put(&key_win->task->fifo, 2); /* 命令行窗口光标ON */
		}
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2); /* level=2, priority=2 */
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal, int debug){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	struct WINDOW *window;
	//unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 256 * 165 * 4);
	unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 525 * 479 * 4);
	//sheet_setbuf(sht, buf, 256, 165, -1); /* 无透明色 */
	sheet_setbuf(sht, buf, 525, 479, -1); /* 无透明色 */
	if(debug == 1) {
		window = make_window8(sht, 525, 479, COL_BRED, COL_DRED, "DEBUG consle", 0);
	} else {
		window = make_window8(sht, 525, 479, TIT_ACT_DEFAULT, TIT_DEACT_DEFAULT, "console", 0);
	}
	make_textbox8(sht, 3, 24, 519, 452, COL_BLACK);
	make_icon(window, 1);
	sht->task = open_constask(sht, memtotal);
	sht->flags = SHEET_CONS;	/* 命令行窗口 */
	return sht;
}

void close_constask(struct TASK *task){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = SHEET_NO_USE; /* 用来替代task_free(task); */
	return;
}

void close_console(struct SHEET *sht){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165 * 4);
	sheet_free(sht);
	close_constask(task);
	return;
}

void debug_print(char *format, ...) {
	/* 格式化输出到debug窗口 */
	va_list ap;
	char s[1024];
	int i;
	struct SHEET *sht_debug_cons = (struct SHEET *) *((int *) DEBUG_ADDR);
	
	va_start(ap, format);
	i = vsprintf(s, format, ap);
	cons_putstr0(sht_debug_cons->task->cons,s);
	va_end(ap);
	return;
}
