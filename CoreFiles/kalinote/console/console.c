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

	/*
	* �������ϵͳģʽ�л�����Ҫ�Ľ�
	*/
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
						cmdline[cons.cur_x / 8 - 2] = 0;		//������ʾ��
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
						cons_putchar(&cons, '>', 1);			//������ʾ��
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
			//if (cons->cur_x == 8 + 240) {
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
	} else if (strcmp(cmdline, "hdnum") == 0) {
		cmd_hdnum(cons);
	} else if (strncmp(cmdline, "hdinfo ", 7) == 0) {
		cmd_hdinfo(cons, cmdline);
	} else if (strcmp(cmdline,"test") == 0) {
		/* ����ר�� */
		cmd_systest(cons);
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
		p = (char *) memman_alloc_4k(memman, finfo->size);											/* ��Ӧ�ó������һ�ο��ڴ�ռ�(��ȡӦ��) */
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));		/* ����Ӧ�ó����ļ� */
		if (finfo->size >= 36 && strncmp(p + 4, "Kali", 4) == 0 && *p == 0x00) {
			/*  ����ļ����жϣ���һ����kalͷ�ļ�Ϊ36�ֽڣ�����kalӦ�ó���һ�������36�ֽڣ��ڶ����Ǽ���Ƿ���kalӦ��ǩ�� */
			/* ����kal�ļ�ͷ(�����Ժ���ܻ���Ҫ�޸�һ��) */
			segsiz = *((int *) (p + 0x0000));					/* stack+.data+heap(���ݶΣ������ⶨ������ݣ��Լ��ַ�����)�Ĵ�С(4K�ı���) */
			esp    = *((int *) (p + 0x000c));					/* ��ջ��ʼֵ��.data����Ŀ�ĵ� */
			datsiz = *((int *) (p + 0x0010));					/* .data�Ĵ�С */
			datkal = *((int *) (p + 0x0014));					/* .data�ĳ�ʼֵ�����ļ��е�λ�� */
			q = (char *) memman_alloc_4k(memman, segsiz);		/* ����Ӧ�ó�����ڴ�ռ�(���ݶ�) */
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);				/* �ζ������0x60(01100000)���Խ��ö�Ȩ������ΪӦ�ó���ʹ�� */	/* �ɶ���ִ�в���д */
			set_segmdesc(task->ldt + 1, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);				/* �ɶ�д����ִ�� */
			for (i = 0; i < datsiz; i++) {
				/* ��KalӦ���е����ݲ��ָ��Ƶ����ݶ� */
				q[esp + i] = p[datkal + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));							/* ���û��д����������Ӧ�ó���ĺ���������0x1b��KaliMain�������(������һ��JMPָ���ת�������ĳ������) */
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

int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
	/* ���Ÿ��ⲿ�����ϵͳAPI */
	/* ��������Ĵ�������ļĴ����ǰ���PUSHAD��д�� */
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
	int *reg = &eax + 1;	/* eax����ĵ�ַ */
		/* ǿ�и�дͨ��PUSHAD�����ֵ */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */

	if (edx == 1) {
		//��ӡ�ַ�
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		//��ӡ�ַ���(���ַ�����0��β)
		cons_putstr0(cons, (char *) ebx + ds_base);
	} else if (edx == 3) {
		//��ӡ�ַ���(ָ������)
		cons_putstr1(cons, (char *) ebx + ds_base, ecx);
	} else if (edx == 4) {
		//��������
		return &(task->tss.esp0);
	} else if (edx == 5) {
		//���ɴ���
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags = SHEET_APIWIN;			/* �ⲿAPI���� */
		sheet_setbuf(sht, (int *) ebx + ds_base, esi, edi, eax);
		window = make_window8(sht, esi, edi, TIT_ACT_DEFAULT, TIT_DEACT_DEFAULT, (char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* ������ͼ��߶�ָ��Ϊ��ǰ�������ͼ��ĸ߶ȣ�����Ƶ��ϲ� */
		reg[7] = window->whandle;			/* (ͨ��EAX�Ĵ���)���ش��ھ��((int)sht) */
	} else if (edx == 6) {
		//�ڴ����ϻ����ַ�
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		//�ڴ����ϻ��Ʒ���
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		/* ��ʼ���ڴ���� */
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0;	/* ��16�ֽ�Ϊ��λ */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		/* �����ڴ� */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* ��16�ֽ�Ϊ��λ��λȡ�� */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	} else if (edx == 10) {
		/* �ͷ��ڴ� */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* ��16�ֽ�Ϊ��λ��λȡ�� */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		/* �������ص� */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		/* ����ˢ�´��� */
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		/* ����ֱ�� */
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
		/* �رմ��� */
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) {
		/* �������� */
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					/* FIFOΪ�գ����ߣ�ֱ�����̽��յ����� */
					task_sleep(task);
				} else {
					/* FIFOΪ�գ�����-1 */
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) { /* ��궨ʱ�� */
				/* Ӧ�ó�������ʱ����ʾ��꣬������ǽ��´���ʾ�õ�ֵ��1 */
				timer_init(cons->timer, &task->fifo, 1); /* ��1 */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* ���ON */
				cons->cur_c = COL_WHITE;
			}
			if (i == 3) {	/* ���OFF */
				cons->cur_c = -1;
			}
			if (i == 4) {	/* ֻ�ر������ */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);	/* 2024��2279 */
				cons->sht = 0;
				io_sti();
			}
			if (256 <= i) { /* �������ݣ�ͨ������A�� */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		/* ��ȡ��ʱ�� */
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1;	/* �����Զ�ȡ�� */
	} else if (edx == 17) {
		/* ���ö�ʱ���������� */
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		/* ��ʱ��ʱ���趨 */
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		/* �ͷŶ�ʱ�� */
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		/* ������ */
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
		/* ���ļ� */
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
		/* �ر��ļ� */
		fh = (struct FILEHANDLE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		/* �ļ���λ */
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
		/* ��ȡ�ļ���С */
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		/* �ļ���ȡ */
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
		/* ������API */
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
		/* ��ѯ����ģʽ */
		reg[7] = task->langmode;
	} else if (edx == 28) {
		/* ����icon */
		
	} else if (edx == 29) {
		/* �������̨ */
		cmd_cls(cons);
	}
	return 0;
}

int *inthandler0d(int *esp){
	/* CPU�쳣�ж� */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg,"********************һ�㱣���쳣********************\n   ϵͳ�ڳ�������Ӧ��ʱ�����˴���\n   һ�㱣���쳣ͨ���������û�������ͼ���ʲ��ɷ��ʵĵ�ַʱ������ô����һ�γ��֣��볢������������Ӧ�ó�������ô��󷴸����֣�����ϵ����Ŀ����ߡ�\n   �����Ǵ˴δ������Ϣ��\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x0D :\n һ�㱣���쳣\n");
	} else {
		/* ������ģʽ */
		sprintf(msg,"*****************General protection exception*****************\n   The system encountered an error while trying to run the application.\n   General protection exception usually occurs when the user program attempts to access an inaccessible address. If the error occurs for the first time, please try to restart the application. If the error occurs repeatedly, please contact the software developer. \n   the following is the error message:\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0D :\n General Protected Exception\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);/* esp11����EIP����ϸesp�б������ϵ�451ҳ */
	cons_putstr0(cons, s);
	
	return &(task->tss.esp0);	/* ǿ�ƽ������� */
}

int *inthandler0c(int *esp){
	/* ջ�쳣�ж� */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg,"********************ջ�쳣********************\n   ϵͳ�ڳ�������Ӧ��ʱ�����˴���\n   ջ�쳣ͨ���������û���������������ջ(�������Ա�±�Խ���)������ô����һ�γ��֣��볢������������Ӧ�ó�������ô��󷴸����֣�����ϵ����Ŀ����ߡ�\n   �����Ǵ˴δ������Ϣ��\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x0C :\n ջ�쳣\n");
	} else {
		sprintf(msg,"********************Stack Exception********************\n   The system encountered an error while trying to run the application. \n   stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds). If this error occurs for the first time, please try to restart the application. If this error occurs repeatedly, please contact the software developer. \n the following is the error message:\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0C :\n Stack Exception.\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* ǿ�ƽ������� */
}

void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col){
	/* ����ֱ�� */
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
	//sheet_setbuf(sht, buf, 256, 165, -1); /* ��͸��ɫ */
	sheet_setbuf(sht, buf, 525, 479, -1); /* ��͸��ɫ */
	if(debug == 1) {
		window = make_window8(sht, 525, 479, COL_BRED, COL_DRED, "DEBUG consle", 0);
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
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = SHEET_NO_USE; /* �������task_free(task); */
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
