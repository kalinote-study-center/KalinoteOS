/* �������س��� */

#include "../bootpack.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void console_task(struct SHEET *sheet, unsigned int memtotal){
	/* �����д��� */
	struct TASK *task = task_now(); 								 /* ��ȡ������task��ַ */
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
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200)); /* ����FAT��(ADR_DISKIMG + 0x000200)��FAT�����ڴ��е�λ�� */
	for (i = 0; i < 8; i++) {
		fhandle[i].buf = 0;	/* δʹ�ñ�Ƿ� */
	}
	task->fhandle = fhandle;
	task->fat = fat;
	if (hzk[4096] != 0xff) {	/* �Ƿ��������ֿ⣿ */
		/* Ĭ��ʹ������ */
		task->langmode = 1;
	} else {
		task->langmode = 0;
	}
	task->langbyte1 = 0;

	/*****************************************************
	*         �������ϵͳģʽ�л�����Ҫ�Ľ�             *
	*****************************************************/
	if(cons.sht->flags != SHEET_DEBUG_CONS){
		/* DEBUG���ڲ����� */
		if (sysinfo->sysmode == 0) {
			/* ��ͨģʽ */
				cons_putchar(&cons, '>', 1);			//������ʾ��
		} else if (sysinfo->sysmode == 1) {
			/* ����ģʽ */
			cons_putchar(&cons, 10001, 1);
		} else {
			/* δ֪ϵͳģʽ */
			cons_putchar(&cons, 10002, 1);
		}
	}

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			/* FIFO������û�����ݣ��������� */
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) {  /* ��궨ʱ�� */
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0); /* ��0 */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL_WHITE;
					}
				} else {
					timer_init(cons.timer, &task->fifo, 1); /* ��1 */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL_BLACK;
					}
				}
				timer_settime(cons.timer, 50);
			}
			if (i == 2) {	/* ������� */
				cons.cur_c = COL_WHITE;
			}
			if (i == 3) {	/* �رչ�� */
				if (cons.sht != 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL_BLACK,
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				cons.cur_c = -1;
			}
			if (i == 4) {	/* ����ڵ�X����� */
				cmd_exit(&cons, fat);
			}
			if (256 <= i && i <= 511 && cons.sht->flags != SHEET_DEBUG_CONS) { /* ��������(ͨ������A)��debug���ڲ����ܼ�������(��ʱ) */
				if (i == 8 + 256) {
					/* backspace */
					/*
					*****************************���ﻹ��Ҫ�޸�*****************************
					*/
					if (sysinfo->sysmode == 0) {
						/* ��ͨģʽ */
						if (cons.cur_x > 2 * 8) {
							/* �ÿո���ѹ����ȥ��ǰ��һ�ι�� */
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}
					} else if (sysinfo->sysmode == 1) {
						/* ����ģʽ */
						if (cons.cur_x > 8 * 8) {
							/* �ÿո���ѹ����ȥ��ǰ��һ�ι�� */
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}
					} else {
						/* δ֪ϵͳģʽ */
						if (cons.cur_x > 16 * 8) {
							/* �ÿո���ѹ����ȥ��ǰ��һ�ι�� */
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}
					}
				} else if (i == 10 + 256) {
					/* Enter */
					/* �ÿո񽫹����� */
					cons_putchar(&cons, ' ', 0);
					if (sysinfo->sysmode == 0) {
						/* ��ͨģʽ */
						cmdline[cons.cur_x / 8 - 2] = 0;				//������ʾ��
					} else if (sysinfo->sysmode == 1) {
						/* ����ģʽ */
						cmdline[cons.cur_x / 8 - 8] = 0;
					} else {
						/* δ֪ϵͳģʽ */
						cmdline[cons.cur_x / 8 - 16] = 0;
					}
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal, sysinfo->sysmode);	/* �������� */
					if (sheet == 0) {
						cmd_exit(&cons, fat);
					}
					/* ��ʾ��ʾ�� */
					if (sysinfo->sysmode == 0) {
						/* ��ͨģʽ */
						cons_putchar(&cons, '>', 1);					//������ʾ��
					} else if (sysinfo->sysmode == 1) {
						/* ����ģʽ */
						cons_putchar(&cons, 10001, 1);
					} else {
						/* δ֪ϵͳģʽ */
						cons_putchar(&cons, 10002, 1);
					}
				} else {
					/* һ���ַ� */
					//if (cons.cur_x < 240) {
					if (cons.cur_x < 512) {
						/* ��ʾһ���ַ��󽫹�����һλ */
						if (sysinfo->sysmode == 0) {
							/* ��ͨģʽ */
							cmdline[cons.cur_x / 8 - 2] = i - 256;		//������ʾ��
						} else if (sysinfo->sysmode == 1) {
							/* ����ģʽ */
							cmdline[cons.cur_x / 8 - 8] = i - 256;
						} else {
							/* δ֪ϵͳģʽ */
							cmdline[cons.cur_x / 8 - 16] = i - 256;
						}
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			/* ������ʾ��꣬debug���ڲ���ʾ��� */
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
	* �������ϵͳģʽ�л�����Ҫ�Ľ�
	*/
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	/* �Ʊ�� */
		for (;;) {
			if (cons->sht != 0) {
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, " ", 1);
			}
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 512) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				break;	/* ��32������break */
			}
		}
	} else if (s[0] == 0x0a) {	/* ���� */
		cons_newline(cons);
	} else if (s[0] == 0x0d) {	/* �س� */
		/* ������ʱ�������κβ��� */
	} else if (chr == 10001) {	/* ϵͳ����ģʽ */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, "DEBUG >", 7);
		}
		if (move != 0) {
			/* moveΪ0ʱ��겻���� */
			cons->cur_x += 8 * 7;
			if (cons->cur_x == 56 + 464) {
				cons_newline(cons);
			}
		}
	} else if (chr == 10002) {	/* ϵͳδ֪ģʽ */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, "UNKNOWSYSMODE >", 15);
		}
		if (move != 0) {
			/* moveΪ0ʱ��겻���� */
			cons->cur_x += 8 * 15;
			if (cons->cur_x == 120 + 400) {
				cons_newline(cons);
			}
		}
	} else {	/* һ���ַ���ϵͳΪ����ģʽ */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL_WHITE, COL_BLACK, s, 1);
		}
		if (move != 0) {
			/* moveΪ0ʱ��겻���� */
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
		cons->cur_y += 16; /* ���� */
	} else {
		/* ���� */
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
		/* ��ͨģʽ */
		cons->cur_x = 8;
	} else if (sysinfo->sysmode == 1) {
		/* ����ģʽ */
		//cons->cur_x = 8 * 7;
		cons->cur_x = 8;
	} else {
		/* δ֪ϵͳģʽ */
		//cons->cur_x = 8 * 15;
		cons->cur_x = 8;
	}
	if (task->langmode == 1 && task->langbyte1 != 0) {
		cons->cur_x = 16;
	}
	
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s){
	/* ��ʾ�ַ�������β�ַ�����0ʱֹͣ */
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l){
	/* ��ʾ�ַ�����ָ�����Ȳ���ʾ */
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
		/* debug���ڲ����� */
			cons_putstr0(cons, "cannot run this command in debug console\n");
			return;
		}
		cmd_exit(cons, fat);
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		if(cons->sht->flags == SHEET_DEBUG_CONS) {
		/* debug���ڲ����� */
			cons_putstr0(cons, "cannot run this command in debug console\n");
			return;
		}
		cmd_start(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "run ", 4) == 0) {
		if(cons->sht->flags == SHEET_DEBUG_CONS) {
		/* debug���ڲ����� */
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
		/* ִ��cmd_app(),�������һ��Ӧ�ã��᷵��0 */
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* �����ڲ����ⲿ���� */
			cons_putstr0(cons, "\"");
			cons_putstr0(cons, cmdline);
			cons_putstr0(cons, "\" is not a command.\n\n");
		}
	}
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline){
	/* ������ִ��Ӧ�ó��� */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	char name[18], *p, *q;
	struct TASK *task = task_now();
	int i, segsiz, datsiz, esp, datkal;
	struct SHTCTL *shtctl;
	struct SHEET *sht;

	/* �������������ļ��� */
	for (i = 0; i < 13; i++) {
		/* 8�ֽ��ļ����������ֽڵĺ�׺���м�Ӹ��㣬һ��12�ֽ� */
		if (cmdline[i] <= ' ') {/* ���ַ������ */
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0; /* ���ҽ��ļ���������0 */

	debug_print("application>try run %s at %d\n", name, cons);

	/* Ѱ���ļ� */
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		/* �Ҳ����Ļ������ļ�ĩβ����.KAL�������� */
		name[i    ] = '.';
		name[i + 1] = 'K';
		name[i + 2] = 'A';
		name[i + 3] = 'L';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo != 0) {
		/* �ҵ��ļ������ */
		p = (char *) memman_alloc_4k(memman, finfo->size);											/* ��Ӧ�ó������һ�ο��ڴ�ռ�(��ȡӦ�õ��ڴ�) */
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));		/* ����Ӧ�ó����ļ� */
		if (finfo->size >= 36 && strncmp(p + 4, "Kali", 4) == 0 && *p == 0x00) {
			/*  ����ļ����жϣ���һ����kalͷ�ļ�Ϊ36�ֽڣ�����kalӦ�ó���һ�������36�ֽڣ��ڶ����Ǽ���Ƿ���kalӦ��ǩ�� */
			/* ����kal�ļ�ͷ(�����Ժ���ܻ���Ҫ�޸�һ��) */
			/* ע��������д���±�����λ�ã������±�ı�������û�����ã�������ʱ�ò��� */
			segsiz = *((int *) (p + 0x0000));					/* stack+.data+heap(���ݶΣ������ⶨ������ݣ��Լ��ַ�����)�Ĵ�С(4K�ı���) */
			esp    = *((int *) (p + 0x000c));					/* ��ջ��ʼֵ��.data����Ŀ�ĵ� */			/* �±�Ϊ14 */
			datsiz = *((int *) (p + 0x0010));					/* .data�Ĵ�С */							/* �±�Ϊ1A */
			datkal = *((int *) (p + 0x0014));					/* .data�ĳ�ʼֵ�����ļ��е�λ�� */			/* �±�Ϊ20 */
			q = (char *) memman_alloc_4k(memman, segsiz);		/* ����Ӧ�ó�����ڴ�ռ�(���ݶ�) */
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);				/* �ζ������0x60(01100000)���Խ��ö�Ȩ������ΪӦ�ó���ʹ�� */	/* �ɶ���ִ�в���д */
			set_segmdesc(task->ldt + 1, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);				/* �ɶ�д����ִ�� */
			for (i = 0; i < datsiz; i++) {
				/* ��KalӦ���е����ݲ��ָ��Ƶ����ݶ� */
				q[esp + i] = p[datkal + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));							/* ���û��д����������Ӧ�ó���ĺ���������0x1b��KaliMain�������(������һ��JMPָ���ת�������ĳ�����ڣ��±����ڵ�ַΪ27) */
			shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if (sht->flags == SHEET_APIWIN && sht->task == task) {
					/* �ҵ�Ӧ�ó�������Ĵ��� */
					sheet_free(sht);	/* �ر� */
				}
			}
			for (i = 0; i < 8; i++) {	/* ��δ�رյ��ļ��ر� */
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
				/* ����ģʽ */
				cons_putstr0(cons,"��֧�ֵ��ļ���ʽ��");
			} else {
				/* Ӣ�Ļ�����ģʽ */
				cons_putstr0(cons, "\nUnrecognized file format.\n");
		}
		}
		memman_free_4k(memman, (int) p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	/* û���ҵ��ļ������ */
	return 0;
}

void keywin_off(struct SHEET *key_win){
	if (key_win->flags != SHEET_NO_TITLE && key_win->flags < 100) {
		/* �����ޱ��������ڣ���flagsС��100(100�Ժ�������ͼ��) */
		change_wtitle8((struct WINDOW *)(key_win->win), 0);
		if (key_win->flags == SHEET_CONS) {
			/* �����д��� */
			fifo32_put(&key_win->task->fifo, 3); /* �����д��ڹ��OFF */
		}
	}
	return;
}

void keywin_on(struct SHEET *key_win){
	if (key_win->flags != SHEET_NO_TITLE && key_win->flags < 100){
		/* �����ޱ��������ڣ���flagsС��100(100�Ժ�������ͼ��) */
		change_wtitle8((struct WINDOW *)(key_win->win), 1);
		if (key_win->flags == SHEET_CONS) {
			/* �����д��� */
			fifo32_put(&key_win->task->fifo, 2); /* �����д��ڹ��ON */
		}
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal){
	/* ���������񣬶����������д��� */
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
	/* ���µ������д��� */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	struct WINDOW *window;
	//unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 256 * 165 * 4);
	unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 525 * 479 * 4);
	//sheet_setbuf(sht, buf, 256, 165, -1); /* ��͸��ɫ */
	sheet_setbuf(sht, buf, 525, 479, -1); /* ��͸��ɫ */
	if(debug == 1) {
		window = make_window8(sht, 525, 479, COL_BRED, COL_DRED, "DEBUG console", 0);
	} else {
		window = make_window8(sht, 525, 479, TIT_ACT_DEFAULT, TIT_DEACT_DEFAULT, "console", 0);
	}
	make_textbox8(sht, 3, 24, 519, 452, COL_BLACK);
	make_icon(window, 1);
	sht->task = open_constask(sht, memtotal);
	sht->flags = SHEET_CONS;	/* �����д��� */
	return sht;
}

void close_constask(struct TASK *task){
	/* �������񣬶����ر������д���(�����������д��ڵ�Ӧ��ʹ��) */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	/* FPU */
    io_cli();
	task->flags = SHEET_NO_USE; /* �������task_free(task); */
    if (taskctl->task_fpu == task) {
        taskctl->task_fpu = 0;
    }
    io_sti();
	return;
}

void close_console(struct SHEET *sht){
	/* �ر������д��� */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165 * 4);
	sheet_free(sht);
	close_constask(task);
	return;
}

void cons_printf(struct CONSOLE *cons, char *format, ...) {
	/* ��ʽ�������ָ��cons���� */
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
	/* ��ʽ�������debug���� */
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
