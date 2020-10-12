/*��������ж������������*/
#include <stdio.h>
#include "bootpack.h"

void make_window(unsigned char *buf, int xsize, int ysize, char *title);					//����һ������
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);		//��Ϳ����ɫ����д�ַ���

void KaliMain(void){
	/* ϵͳ��� */
	
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//������Ϣ(BOOTINFO�ṹ��)
	struct FIFO8 timerfifo;
	char s[40], keybuf[32], mousebuf[128], timerbuf[8];
	struct TIMER *timer, *timer2, *timer3;
	int mx, my, i;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse, *sht_win;
	unsigned char *buf_back, buf_mouse[256], *buf_win;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC��ʼ�����������CPU���жϽ�ֹ */
	fifo8_init(&keyfifo, 32, keybuf);							//��ʼ������fifo������
	fifo8_init(&mousefifo, 128, mousebuf);						//��ʼ�����fifo������
	init_pit();													//��ʼ����ʱ��
	io_out8(PIC0_IMR, 0xf8); /* ����PIT��PIC1�ͼ���(11111000) */
	io_out8(PIC1_IMR, 0xef); /* �������(11101111) */

	fifo8_init(&timerfifo, 8, timerbuf);
	timer = timer_alloc();
	timer_init(timer, &timerfifo, 10);
	timer_settime(timer, 1000);
	timer2 = timer_alloc();
	timer_init(timer2, &timerfifo, 3);
	timer_settime(timer2, 300);
	timer3 = timer_alloc();
	timer_init(timer3, &timerfifo, 1);
	timer_settime(timer3, 50);

	init_keyboard();				//��ʼ������
	enable_mouse(&mdec);			//�������
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();												//��ʼ����ɫ��
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sht_back  = sheet_alloc(shtctl);
	sht_mouse = sheet_alloc(shtctl);							//���
	sht_win = sheet_alloc(shtctl);						//counter����
	
	/* �����ڴ� */
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	buf_win = (unsigned char *) memman_alloc_4k(memman, 160 * 52);
	
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	// û��͸��ɫ
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);						// ��� ͸��ɫ��99
	sheet_setbuf(sht_win, buf_win, 160, 52, -1);						// counter����
	init_screen(buf_back, binfo->scrnx, binfo->scrny);					// ��ʼ����Ļ
	/*
	* ע������� binfo->vram �ȼ���(*binfo).vram
	* ������ͬ��
	*/
	init_mouse_cursor8(buf_mouse, 99);							//����ɫ��99.

	/* counter���� */
	make_window(buf_win, 160, 52, "counter");
	sheet_slide(sht_back, 0, 0);
	mx = (binfo->scrnx - 16) / 2; 								//���ָ��λ�ã�Ĭ��Ϊ��Ļ����
	my = (binfo->scrny - 28 - 16) / 2;
	sheet_slide(sht_mouse, mx, my);
	sheet_slide(sht_win, 75, 70);		//����counter����λ��
	
	sheet_updown(sht_back,  0);
	sheet_updown(sht_win,   1);			/* �����������0(��ײ�)������Ǵ���1(�м�)����������(���) */
	sheet_updown(sht_mouse, 2);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc_sht(sht_back, 0, 0, COL_WHITE, COL_LDBLUE, s, 10);

	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc_sht(sht_back, 0, 32, COL_WHITE, COL_LDBLUE, s, 40);
	
	for(;;){
		//������
		sprintf(s, "%010d", timerctl.count);
		putfonts8_asc_sht(sht_win, 40, 28, COL_BLACK, COL_BGREY, s, 10);
		
		//ֹͣCPU
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) + fifo8_status(&timerfifo) == 0) {
			io_sti();
		} else {
			if (fifo8_status(&keyfifo) != 0) {
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				putfonts8_asc_sht(sht_back, 0, 16, COL_WHITE, COL_LDBLUE, s, 2);
			} else if (fifo8_status(&mousefifo) != 0) {
				i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0) {
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
				}
			} else if (fifo8_status(&timerfifo) != 0) {
				i = fifo8_get(&timerfifo); /* ��һ����ʱ�� */
				io_sti();
				if (i == 10) {
					putfonts8_asc_sht(sht_back, 0, 64, COL_WHITE, COL_LDBLUE, "10[sec]", 7);
				} else if (i == 3) {
					putfonts8_asc_sht(sht_back, 0, 80, COL_WHITE, COL_LDBLUE, "3[sec]", 6);
				} else {
					/* 0��1 */
					if (i != 0) {
						timer_init(timer3, &timerfifo, 0); /* ��0 */
						boxfill8(buf_back, binfo->scrnx, COL_WHITE, 8, 96, 15, 111);
					} else {
						timer_init(timer3, &timerfifo, 1); /* ��1 */
						boxfill8(buf_back, binfo->scrnx, COL_LDBLUE, 8, 96, 15, 111);
					}
					timer_settime(timer3, 50);
					sheet_refresh(sht_back, 8, 96, 16, 112);
				}
			}
		}
	}
}

void make_window(unsigned char *buf, int xsize, int ysize, char *title){
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
	char c;
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL_DGREY, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL_BGREY, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL_DBLUE, 3,         3,         xsize - 4, 20       );
	boxfill8(buf, xsize, COL_DGREY, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, 0,         ysize - 1, xsize - 1, ysize - 1);
	putfonts8_asc(buf, xsize, 24, 4, COL_WHITE, title);
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

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l)
{
	boxfill8(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
	putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}
