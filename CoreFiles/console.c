/* 命令窗口相关程序 */

#include "bootpack.h"
#include <stdio.h>
#include <string.h>

void console_task(struct SHEET *sheet, unsigned int memtotal){
	/* 命令行代码 */
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, fifobuf[128], *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	char cmdline[30];
	cons.sht = sheet;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	*((int *) 0x0fec) = (int) &cons;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));

	cons_putchar(&cons, '>', 1);			//命令提示符

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) { /* 光标定时器 */
				if (i != 0) {
					timer_init(timer, &task->fifo, 0); /* 置0 */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL_WHITE;
					}
				} else {
					timer_init(timer, &task->fifo, 1); /* 置1 */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL_BLACK;
					}
				}
				timer_settime(timer, 50);
			}
			if (i == 2) {	/* 开启光标 */
				cons.cur_c = COL_WHITE;
			}
			if (i == 3) {	/* 关闭光标 */
				boxfill8(sheet->buf, sheet->bxsize, COL_BLACK, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				cons.cur_c = -1;
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
					/* 显示提示符 */
					cons_putchar(&cons, '>', 1);
				} else {
					/* 一般字符 */
					if (cons.cur_x < 240) {
						/* 显示一个字符后将光标后移一位 */
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			/* 重新显示光标 */
			if (cons.cur_c >= 0) {
				boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			}
			sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move){
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	/* 制表符 */
		for (;;) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, " ", 1);
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
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
		putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, s, 1);
		if (move != 0) {
			/* move为0时光标不后移 */
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16; /* 换行 */
	} else {
		/* 滚动 */
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = COL_BLACK;
			}
		}
		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	}
	cons->cur_x = 8;
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal){
	if (strcmp(cmdline, "mem") == 0) {
		cmd_mem(cons, memtotal);
	} else if ((strcmp(cmdline, "cls") == 0) || (strcmp(cmdline, "clear") == 0)) {
		cmd_cls(cons);
	} else if ((strcmp(cmdline, "dir") == 0) || (strcmp(cmdline, "ls") == 0)) {
		cmd_dir(cons);
	} else if (strncmp(cmdline, "type ", 5) == 0) {
		cmd_type(cons, fat, cmdline);
	} else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* 不是内部或外部命令 */
			putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL_WHITE, COL_BLACK, "Not a command.", 15);
			cons_newline(cons);
			cons_newline(cons);
		}
	}
	return;
}


void cmd_mem(struct CONSOLE *cons, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[30];
	sprintf(s, "total   %dMB", memtotal / (1024 * 1024));
	putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL_WHITE, COL_BLACK, s, 30);
	cons_newline(cons);
	sprintf(s, "free %dKB", memman_total(memman) / 1024);
	putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL_WHITE, COL_BLACK, s, 30);
	cons_newline(cons);
	cons_newline(cons);
	return;
}

void cmd_cls(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL_BLACK;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons){
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[30];
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				sprintf(s, "filename.ext   %7d", finfo[i].size);
				for (j = 0; j < 8; j++) {
					s[j] = finfo[i].name[j];
				}
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL_WHITE, COL_BLACK, s, 30);
				cons_newline(cons);
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
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		for (i = 0; i < finfo->size; i++) {
			cons_putchar(cons, p[i], 1);
		}
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		/* 没有找到文件的情况 */
		putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL_WHITE, COL_BLACK, "File not found.", 15);
		cons_newline(cons);
	}
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	char name[18], *p;
	int i;

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
		set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER);
		farcall(0, 1003 * 8);
		memman_free_4k(memman, (int) p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	/* 没有找到文件的情况 */
	return 0;
}
