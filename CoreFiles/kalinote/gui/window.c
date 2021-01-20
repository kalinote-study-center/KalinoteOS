/* ������س��� */

#include "../bootpack.h"

struct WINDOW *make_window8(struct SHEET *sht, int xsize, int ysize, int act_color, int deact_color, char *title, char act){
	/* ���ڴ��� */
	/* ��ʱ��û�����봰�ڽṹ�� */
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

/***************************************************************
*                           �˵�����                           *
***************************************************************/

struct MENU *make_menu(struct MEMMAN *memman, int menux, int menuy) {
	/* �����˵��ṹ�� */
	struct MENU *menu;
	int i;
	menu = (struct MENU *) memman_alloc_4k(memman, sizeof (struct MENU));		/* ���˵�(�ṹ��)����һ���ռ� */
	menu->menux = menux;			/* �˵�X���� */
	menu->menuy = menuy;			/* �˵�Y���� */
	menu->flags = 0;				/* δ��ʾ */
	menu->now = 0;					/* Ĭ��ѡ��index=0(��һ��) */
	menu->old = 0;					/* ��һ��ѡ���� */
	menu->option_num = 0;			/* ѡ������ */
	for(i = 0; i <= MAX_OPTIONS; i++) {
		/* ��ʼ��options��flagsȫ����0 */
		menu->options[i].flags = 0;
	}
	return menu;
}

void release_menu(struct MEMMAN *man, struct MENU *menu) {
	/* �ͷŲ˵�ռ�õ��ڴ�ռ� */
	memman_free_4k(man, (unsigned int)&menu, sizeof(struct MENU));
}

void add_options(struct MENU *menu, char *option_title, void(*onOptionClick)()) {
	/* ����ѡ��ṹ�� */
	struct OPTIONS options;
	if(menu->option_num < MAX_OPTIONS) {
		/* ѡ��δ�ﵽ���ֵ��������ѡ�� */
		options.title = option_title;
		options.index = menu->option_num;
		options.flags = 1;
		options.onOptionClick = onOptionClick;
		menu->options[menu->option_num] = options;
		menu->option_num += 1;
	}
	return;
}

void remove_options(struct MENU *menu, unsigned char index) {
	/* �Ƴ�ѡ�� */
	/* ��index+1��ʼ��ÿ��ѡ����ǰ�ƶ�һλ��һֱ��menu->option_num-1 */
	/* index+1 -> index | index+2 -> index+1	... */
	int i;
	for(i = index; i < menu->option_num - 1; i++) {
		menu->options[i] = menu->options[i+1];
	}
	/* ��indexΪoption_num-1��ѡ���flags��0 */
	menu->options[menu->option_num - 1].flags = 0;
	menu->option_num -= 1;
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
	menu->sht->win = (unsigned int *)menu;
	
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
	boxfill8(menu->buf, menu->mwidth, COL_BLACK, 0, menu->mheight, menu->mwidth, menu->mheight);						/* �º��� */
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

void menu_click(struct MENU *menu, int mouse_y) {
	/* �˵��������� */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	menu->now = mouse_y / 25;	/* ����ѡ�е�����һ�� */
	
	/* ���г��� */
	(*(menu->options[menu->now].onOptionClick))();
	
	/* ���ز˵��� */
	hide_menu(memman, menu);
	return;
}

/***************************************************************
*                           ��ť����                           *
***************************************************************/

struct BUTTON *make_button(struct MEMMAN *memman, int width, int height, int buttonx, int buttony, char *title, int back_color, void(*onButtonClick)()) {
	/* ����һ����ť */
	struct BUTTON *button;
	button = (struct BUTTON *) memman_alloc_4k(memman, sizeof (struct BUTTON));		/* ����ť(�ṹ��)����һ���ռ� */
	button->title = title;							/* ��ť���� */
	button->height = height;						/* ��ť��� */
	button->width = width;							/* ��ť�߶� */
	button->buttonx = buttonx;						/* ��ťX���� */
	button->buttony = buttony;						/* ��ťY���� */
	button->back_color = back_color;				/* ��ť������ɫ */
	button->flags = 1;								/* ����(0�ǽ���) */
	button->click_old = 0;							/* (��һ��)û�а��� */
	button->show = 0;								/* ������ */
	button->onButtonClick = onButtonClick;			/* ���ð�ť���� */
	return button;
}

struct SHEET *show_button(struct SHEET *sht, struct MEMMAN *memman, struct BUTTON *button) {
	/* ��ʾ��ť */
	struct SHEET *btn_sht;
	unsigned int *btn_buf;
	
	button->show = 1;			/* ���ÿ��� */
	
	/* ���仺������sheet */
	/* ��ť��sheet���ڴ��ڻ�����ͼ���ڵ���sheet */
	if(sht->subctl == 0) {
		/* �����û����ͼ�� */
		sht->subctl = shtctl_init(memman, sht->buf, sht->bxsize, sht->bysize);					/* ��ʼ����ͼ���ͼ������� */
		sht->subctl->fsheet = sht;																/* ����ͼ�������ָ��ͼ�� */
	}
	btn_sht = sheet_alloc(sht->subctl);															/* ��ťͼ�� */
	btn_buf = (unsigned int *) memman_alloc_4k(memman, button->height * button->width * 4);		/* ���䰴ťͼ���ڴ� */
	sheet_setbuf(btn_sht, btn_buf, button->width, button->height, -1);							/* ���ð�ťͼ�λ����� */
	btn_sht->flags = SHEET_BUTTON;								/* ����ͼ���ǩΪ��ť */
	btn_sht->win = (unsigned int *)button;						/* ͼ��ָ��ť */
	button->sht = btn_sht;
	
	/* ����ť */
	boxfill8(button->sht->buf,	button->width,	button->back_color,					0,					0,		 button->width - 1,		button->height - 1);			/* ������ */
	boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,						 0,		button->height - 1);			/* ��ɫ������ */
	boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,		 button->width - 1,						 0);			/* ��ɫ�Ϻ��� */
	boxfill8(button->sht->buf,	button->width,			 COL_DGREY, button->width - 2,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
	boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				0, button->height - 2,		 button->width - 1,		button->height - 2);			/* ��ɫ�º��� */
	boxfill8(button->sht->buf,	button->width,			 COL_BLACK,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
	boxfill8(button->sht->buf,	button->width,			 COL_BLACK, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* ��ɫ�º��� */
	/* д���� */
	putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	
	/* �ƶ�ͼ�� */
	sheet_slide(button->sht, button->buttonx, button->buttony);
	if(button->sht->ctl->top < 0) {
		/* �����û��ͼ�� */
		sheet_updown(button->sht, 0);
	} else {
		sheet_updown(button->sht, button->sht->ctl->top);
	}
	return btn_sht;
}

void change_button(struct BUTTON *button, struct SHEET *sht, char click) {
	/* ���İ�ť͹��Ͱ��»���õ�Ч�� */
	
	/****************************************
	*         ���ﻹ��������Ҫ���          *
	****************************************/
	// if(button->click_old == click) {
		// /* ���øı� */
		// return;
	// }
	// if(button->show == 0) {
		// /* ������ */
		// button->click_old = click;
		// return;
	// }
	// if(click == 0) {
		// /* û�а��� */
		// button->click_old = click;
		// /* ����ť */
		// boxfill8(sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* ������ */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE,					0, 					0,						 0,		button->height - 1);			/* ��ɫ������ */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE,					0, 					0,		 button->width - 1,						 0);			/* ��ɫ�Ϻ��� */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, button->width - 2,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, 				0, button->height - 2,		 button->width - 1,		button->height - 2);			/* ��ɫ�º��� */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* ��ɫ�º��� */
		// /* д���� */
		// putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	// } else {
		// /* ���� */
		// button->click_old = click;
		// /* ����ť */
		// boxfill8(sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* ������ */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK,					0, 					0,						 0,		button->height - 1);			/* ��ɫ������ */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK,					0, 					0,		 button->width - 1,						 0);			/* ��ɫ�Ϻ��� */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, 				1,					1,		 button->width - 2,						 1);			/* ��ɫ�Ϻ��� */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, 				1, 					1,						 1,		button->height - 2);			/* ��ɫ������ */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* ��ɫ�º��� */
		// /* д���� */
		// putfonts8_asc(sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	// }
	// sheet_refresh(sht,0,0,sht->bxsize,sht->bysize);
	return;
}

void hide_button() {
	/* ���ذ�ť */
	
}
