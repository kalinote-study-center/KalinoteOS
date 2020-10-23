/* �������س��� */

#include "bootpack.h"
#include <stdio.h>
#include <string.h>

void console_task(struct SHEET *sheet, unsigned int memtotal){
	/* �����д��� */
	struct TIMER *timer;
	struct TASK *task = task_now();
	int i, fifobuf[128], cursor_x = 16, cursor_y = 28, cursor_c = -1;
	char s[30], cmdline[30], *p;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int x, y;
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));

	putfonts8_asc_sht(sheet, 8, 28, COL_WHITE, COL_BLACK, ">", 1);			//������ʾ��

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) { /* ��궨ʱ�� */
				if (i != 0) {
					timer_init(timer, &task->fifo, 0); /* ��0 */
					if (cursor_c >= 0) {
						cursor_c = COL_WHITE;
					}
				} else {
					timer_init(timer, &task->fifo, 1); /* ��1 */
					if (cursor_c >= 0) {
						cursor_c = COL_BLACK;
					}
				}
				timer_settime(timer, 50);
			}
			if (i == 2) {	/* ������� */
				cursor_c = COL_WHITE;
			}
			if (i == 3) {	/* �رչ�� */
				boxfill8(sheet->buf, sheet->bxsize, COL_BLACK, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
				cursor_c = -1;
			}
			if (256 <= i && i <= 511) { /* ��������(ͨ������A) */
				if (i == 8 + 256) {
					/* backspace */
					if (cursor_x > 16) {
						/* �ÿո���ѹ����ȥ��ǰ��һ�ι�� */
						putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL_WHITE, COL_BLACK, " ", 1);
						cursor_x -= 8;
					}
				} else if (i == 10 + 256) {
					/* Enter */
					/* �ÿո񽫹����� */
					putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL_WHITE, COL_BLACK, " ", 1);
					cmdline[cursor_x / 8 - 2] = 0;
					cursor_y = cons_newline(cursor_y, sheet);
					/* ִ������ */
					if (strcmp(cmdline, "mem") == 0) {
						/* mem���� */
						sprintf(s, "total   %dMB", memtotal / (1024 * 1024));
						putfonts8_asc_sht(sheet, 8, cursor_y, COL_WHITE, COL_BLACK, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						sprintf(s, "free %dKB", memman_total(memman) / 1024);
						putfonts8_asc_sht(sheet, 8, cursor_y, COL_WHITE, COL_BLACK, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					} else if ((strcmp(cmdline, "cls") == 0) || (strcmp(cmdline, "clear") == 0)) {
						/* cls&clear���� */
						for (y = 28; y < 28 + 128; y++) {
							for (x = 8; x < 8 + 240; x++) {
								sheet->buf[x + y * sheet->bxsize] = COL_BLACK;
							}
						}
						sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
						cursor_y = 28;
					} else if ((strcmp(cmdline, "dir") == 0) || (strcmp(cmdline, "ls") == 0)) {
						/* dirָ�� */
						for (x = 0; x < 224; x++) {
							if (finfo[x].name[0] == 0x00) {
								break;
							}
							if (finfo[x].name[0] != 0xe5) {
								if ((finfo[x].type & 0x18) == 0) {
									sprintf(s, "filename.ext   %7d", finfo[x].size);
									for (y = 0; y < 8; y++) {
										s[y] = finfo[x].name[y];
									}
									s[ 9] = finfo[x].ext[0];
									s[10] = finfo[x].ext[1];
									s[11] = finfo[x].ext[2];
									putfonts8_asc_sht(sheet, 8, cursor_y, COL_WHITE, COL_BLACK, s, 30);
									cursor_y = cons_newline(cursor_y, sheet);
								}
							}
						}
						cursor_y = cons_newline(cursor_y, sheet);
					} else if (strncmp(cmdline, "type ", 5) == 0) {
						
						/* typeָ�� */
						/* ׼���ļ��� */
						for (y = 0; y < 11; y++) {
							s[y] = ' ';
						}
						y = 0;
						for (x = 5; y < 11 && cmdline[x] != 0; x++) {
							if (cmdline[x] == '.' && y <= 8) {
								y = 8;
							} else {
								s[y] = cmdline[x];
								if ('a' <= s[y] && s[y] <= 'z') {
									/* ��Сд��ĸת���ɴ�д��ĸ */
									s[y] -= 0x20;
								} 
								y++;
							}
						}
						/* Ѱ���ļ� */
						for (x = 0; x < 224; ) {
							if (finfo[x].name[0] == 0x00) {
								break;
							}
							if ((finfo[x].type & 0x18) == 0) {
								for (y = 0; y < 11; y++) {
									if (finfo[x].name[y] != s[y]) {
										goto type_next_file;
									}
								}
								break; /* �ҵ��ļ� */
							}
		type_next_file:
							x++;
						}
						if (x < 224 && finfo[x].name[0] != 0x00) {
							/* �ҵ��ļ������ */
							p = (char *) memman_alloc_4k(memman, finfo[x].size);
							file_loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
							cursor_x = 8;
							for (y = 0; y < finfo[x].size; y++) {
								/* ������� */
								s[0] = p[y];
								s[1] = 0;
								if (s[0] == 0x09) {	/* �Ʊ��(Tab) */
									for (;;) {
										putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL_WHITE, COL_BLACK, " ", 1);
										cursor_x += 8;
										if (cursor_x == 8 + 240) {
											cursor_x = 8;
											cursor_y = cons_newline(cursor_y, sheet);
										}
										if (((cursor_x - 8) & 0x1f) == 0) {
											break;	/* ��32������break */
										}
									}
								} else if (s[0] == 0x0a) {	/* ���� */
									cursor_x = 8;
									cursor_y = cons_newline(cursor_y, sheet);
								} else if (s[0] == 0x0d) {	/* �س� */
									/* ������ʱ�������κβ��� */
								} else {	/* һ���ַ� */
									putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL_WHITE, COL_BLACK, s, 1);
									cursor_x += 8;
									if (cursor_x == 8 + 240) {
										cursor_x = 8;
										cursor_y = cons_newline(cursor_y, sheet);
									}
								}
							}
							memman_free_4k(memman, (int) p, finfo[x].size);
						} else {
							/* û���ҵ��ļ������ */
							putfonts8_asc_sht(sheet, 8, cursor_y, COL_WHITE, COL_BLACK, "File not found.", 15);
							cursor_y = cons_newline(cursor_y, sheet);
						}
						cursor_y = cons_newline(cursor_y, sheet);
					} else if (strcmp(cmdline, "hlt") == 0) {
						/* ����hlt.kal */
						for (y = 0; y < 11; y++) {
							s[y] = ' ';
						}
						s[0] = 'H';
						s[1] = 'L';
						s[2] = 'T';
						s[8] = 'K';
						s[9] = 'A';
						s[10] = 'L';
						for (x = 0; x < 224; ) {
							if (finfo[x].name[0] == 0x00) {
								break;
							}
							if ((finfo[x].type & 0x18) == 0) {
								for (y = 0; y < 11; y++) {
									if (finfo[x].name[y] != s[y]) {
										goto hlt_next_file;
									}
								}
								break; /* �ҵ��ļ� */
							}
		hlt_next_file:
							x++;
						}
						if (x < 224 && finfo[x].name[0] != 0x00) {
							/* �ҵ��ļ������ */
							p = (char *) memman_alloc_4k(memman, finfo[x].size);
							file_loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
							set_segmdesc(gdt + 1003, finfo[x].size - 1, (int) p, AR_CODE32_ER);
							farjmp(0, 1003 * 8);
							memman_free_4k(memman, (int) p, finfo[x].size);
						} else {
							/* û���ҵ��ļ������ */
							putfonts8_asc_sht(sheet, 8, cursor_y, COL_WHITE, COL_BLACK, "File not found.", 15);
							cursor_y = cons_newline(cursor_y, sheet);
						}
						cursor_y = cons_newline(cursor_y, sheet);
					} else if (cmdline[0] != 0) {
						/* �����ڲ����ⲿ���� */
						putfonts8_asc_sht(sheet, 8, cursor_y, COL_WHITE, COL_BLACK, "Not a command.", 15);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					/* ��ʾ��ʾ�� */
					putfonts8_asc_sht(sheet, 8, cursor_y, COL_WHITE, COL_BLACK, ">", 1);
					cursor_x = 16;
				} else {
					/* һ���ַ� */
					if (cursor_x < 240) {
						/* ��ʾһ���ַ��󽫹�����һλ */
						s[0] = i - 256;
						s[1] = 0;
						cmdline[cursor_x / 8 - 2] = i - 256;
						putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL_WHITE, COL_BLACK, s, 1);
						cursor_x += 8;
					}
				}
			}
			/* ������ʾ��� */
			if (cursor_c >= 0) {
				boxfill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
			}
			sheet_refresh(sheet, cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}
	}
}

int cons_newline(int cursor_y, struct SHEET *sheet){
	int x, y;
	if (cursor_y < 28 + 112) {
		cursor_y += 16; /* ���� */
	} else {
		/* ���� */
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
	return cursor_y;
}
