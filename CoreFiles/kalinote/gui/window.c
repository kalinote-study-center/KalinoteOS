/* ������س��� */

#include "../bootpack.h"
#include <string.h>

struct WINDOW *make_window8(struct SHEET *sht, int xsize, int ysize, int act_color, int deact_color, char *title, char act){
	/* ���ڴ��� */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;		/* �ڴ������� */
	struct WINDOW *window;
	window = (struct WINDOW *) memman_alloc_4k(memman, sizeof (struct WINDOW));			/* ��������Ϣ�ṹ�����һ���ڴ� */
	window->wtitle  = title;															/* ���ڱ��� */
	window->xsize = xsize;																/* ����x��С	*/
	window->ysize = ysize;																/* ����y��С */
	window->buf = sht->buf;																/* ����ͼ�λ����� */
	window->whandle = (int)sht;															/* ���ھ�� */		/* ��������ٸĳɵײ�ͼ�� */
	window->wcolor.act_color = act_color;												/* ���ھ۽�ɫ(��ɫ) */
	window->wcolor.dis_act_color = deact_color;											/* ����δ�۽�ɫ(����ɫ) */
	window->wcolor.back_color = COL_WHITE;												/* ����ɫ */
	sht->win = (unsigned int *)window;													/* �õײ�ͼ��ָ�򴰿� */
	boxfill8(sht->buf, xsize, 					COL_BGREY, 0,         0,         xsize - 1, 0        );
	boxfill8(sht->buf, xsize, 					COL_WHITE, 1,         1,         xsize - 2, 1        );
	boxfill8(sht->buf, xsize, 					COL_BGREY, 0,         0,         0,         ysize - 1);
	boxfill8(sht->buf, xsize, 					COL_WHITE, 1,         1,         1,         ysize - 2);
	boxfill8(sht->buf, xsize,					COL_DGREY, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(sht->buf, xsize,					COL_WHITE, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(sht->buf, xsize, 	window->wcolor.back_color, 2,         2,         xsize - 3, ysize - 3);				/* ������ */
	boxfill8(sht->buf, xsize,					COL_DGREY, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(sht->buf, xsize,					COL_WHITE, 0,         ysize - 1, xsize - 1, ysize - 1);
	make_wtitle8(window, act);
	
	/* ����������Ӱ�ť */
	if(strcmp(title, "DEBUG console") != 0){
		/* ����DEBUG console */
		window->tskwinbtn = taskbar_addwin(window);
	}
	
	debug_print("window>create new window,index:%d\n",window->tskwinbtn);
	
	return window;
}

void make_icon(struct WINDOW *window, char type){
	/*
	* ���ƴ��ں�������(��ʱû��)��logo
	* type����Ϊ�������ͣ������0����ʾicon
	* icon_picΪ0��ʹ��Ĭ��
	* ����Ĵ������ʹ��������������
	*/
	int x, y, c, i, j;
	char icon_pic[16][16];
	static char icon_cmd[16][16] = {
		/* ������logo */
		"0000000000000000",
		"0707070000000000",
		"0000000000000000",
		"0777777777777770",
		"0777777777777770",
		"0777777777777770",
		"0777077777777770",
		"0777707777777770",
		"0777770777777770",
		"0777777077777770",
		"0777770770000770",
		"0777707777777770",
		"0777077777777770",
		"0777777777777770",
		"0777777777777770",
		"0000000000000000",
	};
	
	/* Ч��̫�ͣ����������ٸĽ��� */
	if (type == 1){
		for(i = 0;i < 16;i++)
			for(j = 0;j < 16;j++)
				icon_pic[i][j] = icon_cmd[i][j];
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
			window->buf[(5 + y) * window->xsize + (x + 5)] = c;
		}
	}
	return;
}

// void make_wtitle8(unsigned int *buf, int xsize, char *title, char act){
void make_wtitle8(struct WINDOW *window, char act){
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
		tbc = window->wcolor.act_color;
	} else {
		tc = COL_BGREY;
		tbc = window->wcolor.dis_act_color;
	}
	boxfill8(window->buf, window->xsize, tbc, 3, 3, window->xsize - 4, 20);
	putfonts8_asc(window->buf, window->xsize, 24, 4, tc, window->wtitle);
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
			window->buf[(5 + y) * window->xsize + (window->xsize - 40 + x)] = c;
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
			window->buf[(5 + y) * window->xsize + (window->xsize - 21 + x)] = c;
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

void change_wtitle8(struct WINDOW *window, char act){
	/* �ı䴰�ڱ�������ɫ */
	int x, y, xsize = window->xsize;
	int c, tc_new, tbc_new, tc_old, tbc_old, *buf = window->buf;
	if (act != 0) {
		tc_new  = COL_WHITE;
		tbc_new = window->wcolor.act_color;
		tc_old  = COL_BGREY;
		tbc_old = window->wcolor.dis_act_color;
	} else {
		tc_new  = COL_BGREY;
		tbc_new = window->wcolor.dis_act_color;
		tc_old  = COL_WHITE;
		tbc_old = window->wcolor.act_color;
	}
	for (y = 3; y <= 20; y++) {
		for (x = 3; x <= xsize - 4; x++) {
			c = buf[y * xsize + x];
			if (c == tc_old && x <= xsize - 41) {
				c = tc_new;
			} else if (c == tbc_old) {
				c = tbc_new;
			}
			buf[y * xsize + x] = c;
		}
	}
	sheet_refresh((struct SHEET *)(window->whandle), 3, 3, xsize, 21);
	return;
}
