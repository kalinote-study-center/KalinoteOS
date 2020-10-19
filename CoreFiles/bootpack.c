/*����ʱ��������*/
#include <stdio.h>
#include "bootpack.h"

void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);					// ����һ������
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);		// ��Ϳ����ɫ����д�ַ���
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);				// ���ɱ༭��
void task_b_main(struct SHEET *sht_back);													// ����B(����)

void KaliMain(void){
	/* ϵͳ��� */
	
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct FIFO32 fifo;
	char s[40];
	int fifobuf[128];
	int mx, my, i, cursor_x, cursor_c;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHTCTL *shtctl;
	static char keytable[0x54] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.'
	};
	unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_win_b;
	struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_win_b[3];
	struct TASK *task_a, *task_b[3];
	struct TIMER *timer;
	
	init_gdtidt();													// ��ʼ��GDT��IDT
	init_pic();														// ��ʼ���жϿ�����
	io_sti(); 														// IDT/PIC��ʼ�����������CPU���жϽ�ֹ
	fifo32_init(&fifo, 128, fifobuf, 0);							// ��ʼ��FIFO������
	init_pit();														// ��ʼ����ʱ��
	init_keyboard(&fifo, 256);										// ��ʼ������FIFO������
	enable_mouse(&fifo, 512, &mdec);								// ��ʼ�����FIFO������
	io_out8(PIC0_IMR, 0xf8); 										// ����PIT��PIC1�ͼ���(11111000)
	io_out8(PIC1_IMR, 0xef); 										// �������(11101111)
	
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);
	
	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);										//***********************����������***********************
	fifo.task = task_a;

	/* sht_back */
	sht_back  = sheet_alloc(shtctl);
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_win_b */
	for (i = 0; i < 3; i++) {
		sht_win_b[i] = sheet_alloc(shtctl);
		buf_win_b = (unsigned char *) memman_alloc_4k(memman, 144 * 52);
		sheet_setbuf(sht_win_b[i], buf_win_b, 144, 52, -1);
		sprintf(s, "task_b%d", i);
		make_window8(buf_win_b, 144, 52, s, 0);
		task_b[i] = task_alloc();
		task_b[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		task_b[i]->tss.eip = (int) &task_b_main;
		task_b[i]->tss.es = 1 * 8;
		task_b[i]->tss.cs = 2 * 8;
		task_b[i]->tss.ss = 1 * 8;
		task_b[i]->tss.ds = 1 * 8;
		task_b[i]->tss.fs = 1 * 8;
		task_b[i]->tss.gs = 1 * 8;
		*((int *) (task_b[i]->tss.esp + 4)) = (int) sht_win_b[i];
		task_run(task_b[i]);
	}

	/* sht_win */
	sht_win   = sheet_alloc(shtctl);
	buf_win   = (unsigned char *) memman_alloc_4k(memman, 160 * 52);
	sheet_setbuf(sht_win, buf_win, 144, 52, -1); /* ͸��ɫ�ʤ� */
	make_window8(buf_win, 144, 52, "task_a", 1);
	make_textbox8(sht_win, 8, 28, 128, 16, COL_WHITE);
	cursor_x = 8;
	cursor_c = COL_WHITE;
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 50);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;

	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_win_b[0], 168,  56);
	sheet_slide(sht_win_b[1],   8, 116);
	sheet_slide(sht_win_b[2], 168, 116);
	sheet_slide(sht_win,        8,  56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back,     0);
	sheet_updown(sht_win_b[0], 1);
	sheet_updown(sht_win_b[1], 2);
	sheet_updown(sht_win_b[2], 3);
	sheet_updown(sht_win,      4);
	sheet_updown(sht_mouse,    5);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc_sht(sht_back, 0, 0, COL_WHITE, COL_LDBLUE, s, 10);

	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc_sht(sht_back, 0, 32, COL_WHITE, COL_LDBLUE, s, 40);
	
	for(;;){
		//ֹͣCPU
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			task_sleep(task_a);
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (256 <= i && i <= 511) { /* �������� */
				sprintf(s, "%02X", i - 256);
				putfonts8_asc_sht(sht_back, 0, 16, COL_WHITE, COL_LDBLUE, s, 2);
				if (i < 0x54 + 256) {
					if (keytable[i - 256] != 0 && cursor_x < 144) { /* һ���ַ� */
						/* ÿ��ʾһ���ַ���������ǰ�ƶ�һ�� */
						s[0] = keytable[i - 256];
						s[1] = 0;
						putfonts8_asc_sht(sht_win, cursor_x, 28, COL_BLACK, COL_WHITE, s, 1);
						cursor_x += 8;
					}
				}
				if (i == 256 + 0x0e && cursor_x > 8) { /* backspace */
					/* �ÿո���ѹ����ȥ�󣬺���һ�ι�� */
					putfonts8_asc_sht(sht_win, cursor_x, 28, COL_BLACK, COL_WHITE, " ", 1);
					cursor_x -= 8;
				}
				/* ����ٴ���ʾ */
				boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			} else if (512 <= i && i <= 767) { /* ������� */
				if (mouse_decode(&mdec, i - 512) != 0) {
					/* 3�ַ����룬��ʾ���� */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {
						//������
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {
						//����Ҽ�
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {
						//����м�
						s[2] = 'C';
					}
					putfonts8_asc_sht(sht_back, 32, 16, COL_WHITE, COL_LDBLUE, s, 15);
					/* ���ָ���ƶ� */
					mx += mdec.x;
					my += mdec.y;
					//�������ָ��߽�
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 1) {
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1) {
						my = binfo->scrny - 1;
					}
					sprintf(s, "(%3d, %3d)", mx, my);
					putfonts8_asc_sht(sht_back, 0, 0, COL_WHITE, COL_LDBLUE, s, 10);
					sheet_slide(sht_mouse, mx, my); /* ����sheet_refresh */
					if ((mdec.btn & 0x01) != 0) {
						/* ����������ƶ�sht_win */
						sheet_slide(sht_win, mx - 80, my - 8);
					}
				}
			} else if (i <= 1) { /* ��궨ʱ�� */
				if (i != 0) {
					timer_init(timer, &fifo, 0); /* ��0 */
					cursor_c = COL_BLACK;
				} else {
					timer_init(timer, &fifo, 1); /* ��1 */
					cursor_c = COL_WHITE;
				}
				timer_settime(timer, 50);
				boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			}
		}
	}
}

void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act){
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c, tc, tbc;
	if (act != 0) {
		tc = COL_WHITE;
		tbc = COL_DBLUE;
	} else {
		tc = COL_BGREY;
		tbc = COL_DGREY;
	}
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL_DGREY, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL_BGREY, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, tbc,         3,         3,         xsize - 4, 20       );
	boxfill8(buf, xsize, COL_DGREY, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, 0,         ysize - 1, xsize - 1, ysize - 1);
	putfonts8_asc(buf, xsize, 24, 4, tc, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL_BLACK;
			} else if (c == '$') {
				c = COL_DGREY;
			} else if (c == 'Q') {
				c = COL_BGREY;
			} else {
				c = COL_WHITE;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l){
	boxfill8(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
	putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}

void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c){
	int x1 = x0 + sx, y1 = y0 + sy;
	boxfill8(sht->buf, sht->bxsize, COL_DGREY, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	boxfill8(sht->buf, sht->bxsize, COL_DGREY, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL_WHITE, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL_WHITE, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL_BLACK, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	boxfill8(sht->buf, sht->bxsize, COL_BLACK, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	boxfill8(sht->buf, sht->bxsize, COL_BGREY, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL_BGREY, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, c,           x0 - 1, y0 - 1, x1 + 0, y1 + 0);
	return;
}

void task_b_main(struct SHEET *sht_win_b)
{
	struct FIFO32 fifo;
	struct TIMER *timer_1s;
	int i, fifobuf[128], count = 0, count0 = 0;
	char s[12];

	fifo32_init(&fifo, 128, fifobuf, 0);
	timer_1s = timer_alloc();
	timer_init(timer_1s, &fifo, 100);
	timer_settime(timer_1s, 100);

	for (;;) {
		count++;
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (i == 100) {
				sprintf(s, "%11d", count - count0);
				putfonts8_asc_sht(sht_win_b, 24, 28, COL_WHITE, COL_BGREY, s, 11);
				count0 = count;
				timer_settime(timer_1s, 100);
			}
		}
	}
}
