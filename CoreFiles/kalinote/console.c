/* 命令窗口相关程序 */

#include "bootpack.h"
#include <stdio.h>
#include <string.h>

void console_task(struct SHEET *sheet, unsigned int memtotal){
	/* 命令行代码 */
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	struct FILEHANDLE fhandle[8];
	char cmdline[30];
	unsigned char *hzk = (char *) *((int *) 0x0fe8);

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
		task->langmode = 1;
	} else {
		task->langmode = 0;
	}
	task->langbyte1 = 0;

	cons_putchar(&cons, '>', 1);			//命令提示符

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
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
			if (256 <= i && i <= 511) { /* 键盘数据(通过任务A) */
				if (i == 8 + 256) {
					/* backspace */
					if (cons.cur_x > 16) {
						/* 用空格键把光标消去后，前移一次光标 */
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) {
					/* Enter */
					/* 用空格将光标擦除 */
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);	/* 运行命令 */
					if (sheet == 0) {
						cmd_exit(&cons, fat);
					}
					/* 显示提示符 */
					cons_putchar(&cons, '>', 1);
				} else {
					/* 一般字符 */
					//if (cons.cur_x < 240) {
					if (cons.cur_x < 512) {
						/* 显示一个字符后将光标后移一位 */
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			/* 重新显示光标 */
			if (sheet != 0) {
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
	} else {	/* 一般字符 */
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
	cons->cur_x = 8;
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

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal){
	if (strcmp(cmdline, "mem") == 0) {
		cmd_mem(cons, memtotal);
	} else if (((strcmp(cmdline, "cls") == 0) || (strcmp(cmdline, "clear") == 0)) && cons->sht != 0) {
		cmd_cls(cons);
	} else if (((strcmp(cmdline, "dir") == 0) || (strcmp(cmdline, "ls") == 0)) && cons->sht != 0) {
		cmd_dir(cons);
	} else if (strncmp(cmdline, "type ", 5) == 0 && cons->sht != 0) {
		cmd_type(cons, fat, cmdline);
	} else if (strcmp(cmdline, "exit") == 0) {
		cmd_exit(cons, fat);
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		cmd_start(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "run ", 4) == 0) {
		cmd_run(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "langmode ", 9) == 0) {
		cmd_langmode(cons, cmdline);
	} else if (strcmp(cmdline, "shutdown") == 0) {
		cmd_shutdown();
	} else if (cmdline[0] != 0) {
		/* 执行cmd_app(),如果不是一个应用，会返回0 */
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* 不是内部或外部命令 */
			cons_putstr0(cons, "Not a command.\n\n");
		}
	}
	return;
}


void cmd_mem(struct CONSOLE *cons, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[60];
	sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	//for (y = 28; y < 28 + 128; y++) {
	for (y = 28; y < 28 + 448; y++) {
		//for (x = 8; x < 8 + 240; x++) {
		for (x = 8; x < 8 + 512; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL_BLACK;
		}
	}
	//sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	sheet_refresh(sheet, 8, 28, 8 + 512, 28 + 448);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons){
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[50];
	struct TASK *task = task_now();
	if (task->langmode == 1) {
		cons_putstr0(cons, "文件名			类型  			 大小		  属性\n\n");
	} else {
		cons_putstr0(cons, "FILENAME		TYPE  			 SIZE		PROPERTIES\n\n");
	}
	
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			/* 没有文件信息 */
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			/* 未被删除的文件(finfo[i].name[0]=0xe5时代表文件被删除) */
			if ((finfo[i].type & 0xdf) == 0) {
				if (task->langmode == 1) {
					sprintf(s, "filename      ext 文件     %7d 字节		普通文件\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		NormalFile\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				/* 扩展名 */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xfe) == 0) {
				/* 只读文件 */
				if (task->langmode == 1) {
					sprintf(s, "filename      ext 文件     %7d 字节		只读文件\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		 ReadOnly\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				/* 扩展名 */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xfb) == 0) {
				/* 系统文件 */
				if (task->langmode == 1) {
					sprintf(s, "filename      ext 文件     %7d 字节		系统文件\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		SystemFile\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				/* 扩展名 */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xef) == 0) {
				/* 目录 */
				if (task->langmode == 1) {
					sprintf(s, "filename        目录       %7d 字节		  目录\n", finfo[i].size);
				} else {
					sprintf(s, "filename     Directory     %7d Byte		Directory\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	char *p;
	int i;
	if (finfo != 0) {
		/* 找到文件的情况 */
		p = (char *) memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));	/* 磁盘映像中的地址 = clustno * 512 +0x003e00 */
		for (i = 0; i < finfo->size; i++) {
			cons_putchar(cons, p[i], 1);
		}
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		/* 没有找到文件的情况 */
		putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL_WHITE, COL_BLACK, "File not found.", 25);
		cons_newline(cons);
	}
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons, int *fat){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0x0fec);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0) {
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);	/* 768～1023 */
	} else {
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);	/* 1024～2023 */
	}
	io_sti();
	for (;;) {
		task_sleep(task);
	}
}

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal){
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* 将命令行输入的字符串逐字复制到新的命令行窗口中 */
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal){
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	
	/* 将命令行输入的字符串逐字复制到新的命令行窗口中 */
	for (i = 4; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_langmode(struct CONSOLE *cons, char *cmdline){
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 3) {
		task->langmode = mode;
		if (task->langmode == 0) {
			cons_putstr0(cons,"Switch to English ASCII mode\n");
		} else if (task->langmode == 1) {
			cons_putstr0(cons,"切换到中文模式\n");
		} else if (task->langmode == 2) {
			cons_putstr0(cons,"Switch to shift-JIS encoding Japanese\n");
		} else if (task->langmode == 3) {
			cons_putstr0(cons,"Switch to EUC encoding Japanese\n");
		}
	} else {
		cons_putstr0(cons, "mode number error.\n");
	}
	cons_newline(cons);
	return;
}

void cmd_shutdown(void){
	/* 关机 */
	asm_shutdown();
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	char name[18], *p, *q;
	struct TASK *task = task_now();
	int i, segsiz, datsiz, esp, datkal;
	struct SHTCTL *shtctl;
	struct SHEET *sht;

	/* 根据命令生成文件名 */
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
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
		p = (char *) memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		if (finfo->size >= 36 && strncmp(p + 4, "Kali", 4) == 0 && *p == 0x00) {
			segsiz = *((int *) (p + 0x0000));
			esp    = *((int *) (p + 0x000c));
			datsiz = *((int *) (p + 0x0010));
			datkal = *((int *) (p + 0x0014));
			q = (char *) memman_alloc_4k(memman, segsiz);
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[datkal + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & 0x11) == 0x11 && sht->task == task) {
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
			cons_putstr0(cons, "\nUnrecognized file format.\n");
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
	int i;
	struct TASK *task = task_now();
	int ds_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) 0x0fec);
	struct FILEINFO *finfo;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
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
		sht->flags |= 0x10;
		sheet_setbuf(sht, (int *) ebx + ds_base, esi, edi, eax);
		make_window8((int *) ebx + ds_base, esi, edi, (char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* 将窗口图层高度指定为当前鼠标所在图层的高度，鼠标移到上层 */
		reg[7] = (int) sht;
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
					task_sleep(task);	/* FIFO为空，休眠并等待 */
				} else {
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
		/* 显示icon */
		
	} else if (edx == 29) {
		/* 清空命令台 */
		cmd_cls(cons);
	}
	return 0;
}

int *inthandler0d(int *esp){
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0x0D :\n General Protected Exception.\n");
	sprintf(s, "EIP = 0x%08X\n", esp[11]);/* esp11号是EIP，详细esp列表在书上第451页 */
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* 强制结束程序 */
}

int *inthandler0c(int *esp){
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0x0C :\n Stack Exception.\n");
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

