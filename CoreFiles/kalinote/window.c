/* ������س��� */

#include "bootpack.h"

void make_window8(unsigned int *buf, int xsize, int ysize, char *title, char act){
	/* ���ڴ��� */
	/* ��ʱ��û�����봰�ڽṹ�� */
	// struct WINDOW window;
	// window.wtitle  = title;
	// window.xsize = xsize;
	// window.ysize = ysize;
	// window.wcolor.act_color = 0x00cd9b9b;			/* ���ھ۽�ɫ(��ɫ) */
	// window.wcolor.dis_act_color 0x00ffc1c1;			/* ����δ�۽�ɫ(����ɫ) */
	// window.wcolor.back_color = COL_WHITE;			/* ����ɫ */
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

void make_icon(unsigned int *buf, int xsize, char type){
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

/***************************************************************
*                           �˵�����                           *
***************************************************************/

struct MENU *make_menu(struct MEMMAN *memman, int menux, int menuy) {
	/* �����˵��ṹ�� */
	struct MENU *menu;
	menu = (struct MENU *) memman_alloc_4k(memman, sizeof (struct MENU));		/* ���˵�����һ���ռ� */
	menu->menux = menux;			/* �˵�X���� */
	menu->menuy = menuy;			/* �˵�Y���� */
	menu->flags = 0;				/* δ��ʾ */
	menu->now = 0;					/* Ĭ��ѡ��index=0(��һ��) */
	menu->old = 0;
	menu->option_num = 0;			/* ѡ������ */
	return menu;
}

void add_options(struct MENU *menu, char *option_title, unsigned char index) {
	/* ����ѡ��ṹ�� */
	struct OPTIONS options;
	if(index <= MAX_OPTIONS) {
		/* index��Ҫ��0��256֮�� */
		options.title = option_title;
		options.index = index;
		options.flags = 1;
		menu->options[index] = options;
		menu->option_num += 1;
	}
	return;
}

void show_menu(struct SHTCTL *shtctl, struct MEMMAN *memman, struct MENU *menu) {
	/* ��ʾ�˵� */
	int i;
	
	menu->flags = 1;							/* ������ʾ */
	
	/* ����˵����ĸ߶ȺͿ�� */
	menu->mheight = menu->option_num * 25;			/* ÿ��ѡ���25 */
	menu->mwidth = 120;								/* ��ȹ̶�120 */
	
	/* ���仺������sheet */
	menu->sht = sheet_alloc(shtctl);
	menu->sht->flags = SHEET_MENU;
	menu->buf = (unsigned int *) memman_alloc_4k(memman,menu->mheight * (menu->mwidth + 1) * 4);
	sheet_setbuf(menu->sht, menu->buf, menu->mwidth, menu->mheight + 1, -1);
	
	/* ���� */
	boxfill8(menu->buf, menu->mwidth, COL_WHITE, 0, 0, menu->mwidth, menu->mheight);										/* ������ */
	for(i = 0;i <= MAX_OPTIONS; i++){
		if(menu->options[i].flags == 1) {
			/* ����ʹ�õ�ѡ��(flagsΪ1) */
			boxfill8(menu->buf, menu->mwidth, COL_BLACK,          0, i * 25,     menu->mwidth,       i * 25);				/* �Ϻ��� */
			boxfill8(menu->buf, menu->mwidth, COL_BLACK,          0, i * 25,          0, (i + 1) * 25);						/* ������ */
			boxfill8(menu->buf, menu->mwidth, COL_BLACK, menu->mwidth - 1, i * 25, menu->mwidth - 1, (i + 1) * 25);			/* ������ */
			putfonts8_asc(menu->buf, menu->mwidth, 5, 5 + (i * 25), COL_BLACK, menu->options[i].title);
		}
	}
	boxfill8(menu->buf, menu->mwidth, COL_BLACK, 0, menu->mheight - 1, menu->mwidth, menu->mheight);						/* �º��� */
	sheet_slide(menu->sht, menu->menux, menu->menuy);
	sheet_updown(menu->sht, shtctl->top);
	return;
}

void hide_menu(struct MEMMAN *memman, struct MENU *menu) {
	/* ���ز˵� */
	sheet_free(menu->sht);																		/* �ͷ�ͼ�� */
	memman_free_4k(memman, (unsigned int)(menu->buf), menu->mheight * menu->mwidth * 4);		/* �ͷ�ͼ��ռ�õ��ڴ� */
	menu->flags = 0;																			/* δ��ʾ */
	return;
}

void option_change(struct MENU *menu, int mouse_y) {
	/* ��������ƶ���ѡ���� */
	menu->now = mouse_y / 25;	/* ����ѡ�е�����һ�� */
	/* ���ж��Ƿ����һ��ѡ��һ�� */
	if(menu->now == menu->old) {
		/* ���һ�����øı� */
		return;
	}
	/* �����һ���������ɵĺ��µ�ѡ��ĵ�ɫ */
	boxfill8(menu->buf, menu->mwidth, COL_WHITE, 1, menu->old * 25 + 1, menu->mwidth - 2 , (menu->old + 1) * 25 - 1);			/* ��ɫ���Ǿ�ѡ�� */
	putfonts8_asc(menu->buf, menu->mwidth, 5, 5 + (menu->old * 25), COL_BLACK, menu->options[menu->old].title);					/* ����д���� */
	sheet_refresh(menu->sht, 0, menu->old * 25 + 1, menu->mwidth, (menu->old + 1) * 25);										/* ˢ�� */
	boxfill8(menu->buf, menu->mwidth, 0xff86c1, 1, menu->now * 25 + 1, menu->mwidth - 2 , (menu->now + 1) * 25 - 1);			/* ��ɫ������ѡ�� */
	putfonts8_asc(menu->buf, menu->mwidth, 5, 5 + (menu->now * 25), COL_BLACK, menu->options[menu->now].title);					/* ����д���� */
	sheet_refresh(menu->sht, 0, menu->now * 25 + 1, menu->mwidth, (menu->now + 1) * 25);										/* ˢ�� */
	menu->old = menu->now;
	return;
}
