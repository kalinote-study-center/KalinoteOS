/* ���Ʋ˵��� */
#include "../bootpack.h"

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
	if(menu->flags != 0) {
		/* ���û�йر��ȹر� */
		hide_menu(man, menu);
	}
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
