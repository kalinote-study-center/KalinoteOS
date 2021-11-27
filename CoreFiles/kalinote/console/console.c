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

	/*****************************************************
	*         这里关于系统模式切换还需要改进             *
	*****************************************************/
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
						cmdline[cons.cur_x / 8 - 2] = 0;				//命令提示符
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
						cons_putchar(&cons, '>', 1);					//命令提示符
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
	debug_print("command>try run %s at %d\n", cmdline, cons);
	if (strcmp(cmdline, "mem") == 0) {
		cmd_mem(cons, memtotal);
	} else if (((strcmp(cmdline, "cls") == 0) || (strcmp(cmdline, "clear") == 0)) && cons->sht != 0) {
		cmd_cls(cons);
	} else if (((strncmp(cmdline, "dir", 3) == 0) || (strncmp(cmdline, "ls", 2) == 0)) && cons->sht != 0) {
		cmd_dir(cons, cmdline);
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
	} else if (strcmp(cmdline, "getruntime") == 0) {
		cmd_getruntime(cons);
	} else if (strcmp(cmdline, "sysinfo") == 0) {
		cmd_sysinfo(cons, memtotal);
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

	debug_print("application>try run %s at %d\n", name, cons);

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
		p = (char *) memman_alloc_4k(memman, finfo->size);											/* 给应用程序分配一段空内存空间(读取应用到内存) */
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));		/* 加载应用程序文件 */
		if (finfo->size >= 36 && strncmp(p + 4, "Kali", 4) == 0 && *p == 0x00) {
			/*  这里的几个判断，第一个是kal头文件为36字节，所以kal应用程序一定会大于36字节，第二个是检查是否有kal应用签名 */
			/* 处理kal文件头(这里以后可能还需要修改一下) */
			/* 注释里面有写到新标的相关位置，但是新标的编译器还没有做好，所以暂时用不上 */
			segsiz = *((int *) (p + 0x0000));					/* stack+.data+heap(数据段，函数外定义的数据，以及字符串等)的大小(4K的倍数) */
			esp    = *((int *) (p + 0x000c));					/* 堆栈初始值和.data传输目的地 */			/* 新标为14 */
			datsiz = *((int *) (p + 0x0010));					/* .data的大小 */							/* 新标为1A */
			datkal = *((int *) (p + 0x0014));					/* .data的初始值列在文件中的位置 */			/* 新标为20 */
			q = (char *) memman_alloc_4k(memman, segsiz);		/* 分配应用程序段内存空间(数据段) */
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);				/* 段定义加上0x60(01100000)可以将该段权限设置为应用程序使用 */	/* 可读可执行不可写 */
			set_segmdesc(task->ldt + 1, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);				/* 可读写不可执行 */
			for (i = 0; i < datsiz; i++) {
				/* 将Kal应用中的数据部分复制到数据段 */
				q[esp + i] = p[datkal + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));							/* 调用汇编写的用于启动应用程序的函数，其中0x1b是KaliMain程序入口(这里有一个JMP指令，跳转到真正的程序入口，新标的入口地址为27) */
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
	/* 仅启动任务，而不打开命令行窗口 */
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
	/* 打开新的命令行窗口 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	struct WINDOW *window;
	//unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 256 * 165 * 4);
	unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 525 * 479 * 4);
	//sheet_setbuf(sht, buf, 256, 165, -1); /* 无透明色 */
	sheet_setbuf(sht, buf, 525, 479, -1); /* 无透明色 */
	if(debug == 1) {
		window = make_window8(sht, 525, 479, COL_BRED, COL_DRED, "DEBUG console", 0);
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
	/* 结束任务，而不关闭命令行窗口(对于无命令行窗口的应用使用) */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	/* FPU */
    io_cli();
	task->flags = SHEET_NO_USE; /* 用来替代task_free(task); */
    if (taskctl->task_fpu == task) {
        taskctl->task_fpu = 0;
    }
    io_sti();
	return;
}

void close_console(struct SHEET *sht){
	/* 关闭命令行窗口 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165 * 4);
	sheet_free(sht);
	close_constask(task);
	return;
}

void cons_printf(struct CONSOLE *cons, char *format, ...) {
	/* 格式化输出到指定cons窗口 */
	char buf[1024];
	int i;
	va_list ap;
	
	va_start(ap, format);
	i = vsprintf(buf, format, ap);
	cons_putstr0(cons, buf);
	va_end(ap);
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
