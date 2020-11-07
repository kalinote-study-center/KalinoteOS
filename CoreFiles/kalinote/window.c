/* ������س��� */

#include "bootpack.h"

void make_window8(unsigned int *buf, int xsize, int ysize, char *title, char act){
	/* ���ڴ��� */
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL_DGREY, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL_BGREY, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL_DGREY, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, 0,         ysize - 1, xsize - 1, ysize - 1);
	make_wtitle8(buf, xsize, title, act);
	return;
}

void make_icon(unsigned int *buf, int xsize, char icon_pic[16][16], char type){
	/*
	* ���ƴ��ں�������(��ʱû��)��logo
	* type����Ϊ�������ͣ������0����ʾicon
	* icon_picΪ0��ʹ��Ĭ��
	* ����Ĵ������ʹ��������������
	*/
	int x, y, c, i;
	char is_emp = 0;
	static char icon_cmd[16][16] = {
		"0000000000000000",
		"0000000000000000",
		"0000000000000000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"000AAAAAAAAAA000",
		"0000000000000000",
		"0000000000000000",
		"0000000000000000",
	};
	
	if ( sizeof(icon_pic) != 16 ){
		is_emp = 1;
	} else {
		for (i = 0; i <= 16; i++) {
			if (sizeof(icon_pic[i]) != 16) {
				is_emp = 1;
			}
		}
	}

	if (is_emp == 1) {
		if (type == 1){
			icon_pic = icon_cmd;
		}
	}

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			c = icon_pic[y][x];
			if (c == '0') {
				/* ��ɫ */
				c = 0x000000;
			} else if (c == '1') {
				/* ��ɫ */
				c = 0x0000ff;
			} else if (c == '2') {
				/* ��ɫ */
				c = 0x00ff00;
			} else if (c == '3') {
				/* ǳ��ɫ */
				c = 0x53ff53;
			} else if (c == '4') {
				/* ��ɫ */
				c = 0xff0000;
			} else if (c == '5') {
				/* ��ɫ */
				c = 0x800080;
			} else if (c == '6') {
				/* ��ɫ */
				c = 0xffff00;
			} else if (c == '7') {
				/* ��ɫ */
				c = 0xffffff;
			} else if (c == '8') {
				/* ��ɫ */
				c = 0xcccccc;
			} else if (c == '9') {
				/* ����ɫ */
				c = 0x00ffff;
			} else if (c == 'A') {
				/* ����ɫ */
				c = 0x90ee90;
			} else if (c == 'B') {
				/* ��ǳ��ɫ */
				c = 0xbbffbb;
			} else if (c == 'C') {
				/* ����ɫ */
				c = 0xff5151;
			} else if (c == 'D') {
				/* ����ɫ */
				c = 0xda70d6;
			} else if (c == 'E') {
				/* ����ɫ */
				c = 0xe3cf57;
			} else if (c == 'F') {
				/* ����ɫ */
				c = 0xffffff;
			}
			buf[(5 + y) * xsize + (x + 5)] = c;
		}
	}
	return;
}

void make_wtitle8(unsigned int *buf, int xsize, char *title, char act){
	/* ���ڱ����� */
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
	static char minimizebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQ@@@@@@@@@QQ$@",
		"OQQ@@@@@@@@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	int c, tc, tbc;
	if (act != 0) {
		tc = COL_WHITE;
		tbc = 0x00ffc1c1;
	} else {
		tc = COL_BGREY;
		tbc = 0x00cd9b9b;
	}
	boxfill8(buf, xsize, tbc, 3, 3, xsize - 4, 20);
	putfonts8_asc(buf, xsize, 24, 4, tc, title);
	/* ��С�����ڰ�ť */
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = minimizebtn[y][x];
			if (c == '@') {
				c = COL_BLACK;
			} else if (c == '$') {
				c = COL_DGREY;
			} else if (c == 'Q') {
				c = COL_WHITE;
			} else {
				c = COL_WHITE;
			}
			buf[(5 + y) * xsize + (xsize - 40 + x)] = c;
		}
	}
	/* �رմ��ڰ�ť */
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL_BLACK;
			} else if (c == '$') {
				c = COL_DGREY;
			} else if (c == 'Q') {
				c = COL_WHITE;
			} else {
				c = COL_WHITE;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l){
	struct TASK *task = task_now();
	boxfill8(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
	if (task->langmode != 0 && task->langbyte1 != 0) {
		putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
		sheet_refresh(sht, x - 8, y, x + l * 8, y + 16);
	} else {
		putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
		sheet_refresh(sht, x, y, x + l * 8, y + 16);
	}
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

void change_wtitle8(struct SHEET *sht, char act){
	/* �ı䴰�ڱ�������ɫ */
	int x, y, xsize = sht->bxsize;
	int c, tc_new, tbc_new, tc_old, tbc_old, *buf = sht->buf;
	if (act != 0) {
		tc_new  = COL_WHITE;
		tbc_new = 0x00ffc1c1;
		tc_old  = COL_BGREY;
		tbc_old = 0x00cd9b9b;
	} else {
		tc_new  = COL_BGREY;
		tbc_new = 0x00cd9b9b;
		tc_old  = COL_WHITE;
		tbc_old = 0x00ffc1c1;
	}
	for (y = 3; y <= 20; y++) {
		for (x = 3; x <= xsize - 4; x++) {
			c = buf[y * xsize + x];
			if (c == tc_old && x <= xsize - 22) {
				c = tc_new;
			} else if (c == tbc_old) {
				c = tbc_new;
			}
			buf[y * xsize + x] = c;
		}
	}
	sheet_refresh(sht, 3, 3, xsize, 21);
	return;
}
