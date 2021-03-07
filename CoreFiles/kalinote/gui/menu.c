/* 绘制菜单栏 */
#include "../bootpack.h"

struct MENU *make_menu(struct MEMMAN *memman, int menux, int menuy) {
	/* 创建菜单结构体 */
	struct MENU *menu;
	int i;
	menu = (struct MENU *) memman_alloc_4k(memman, sizeof (struct MENU));		/* 给菜单(结构体)分配一个空间 */
	menu->menux = menux;			/* 菜单X坐标 */
	menu->menuy = menuy;			/* 菜单Y坐标 */
	menu->flags = 0;				/* 未显示 */
	menu->now = 0;					/* 默认选中index=0(第一项) */
	menu->old = 0;					/* 上一次选择项 */
	menu->option_num = 0;			/* 选项数量 */
	for(i = 0; i <= MAX_OPTIONS; i++) {
		/* 初始化options，flags全部置0 */
		menu->options[i].flags = 0;
	}
	return menu;
}

void release_menu(struct MEMMAN *man, struct MENU *menu) {
	/* 释放菜单占用的内存空间 */
	if(menu->flags != 0) {
		/* 如果没有关闭先关闭 */
		hide_menu(man, menu);
	}
	memman_free_4k(man, (unsigned int)&menu, sizeof(struct MENU));
}

void add_options(struct MENU *menu, char *option_title, void(*onOptionClick)()) {
	/* 创建选项结构体 */
	struct OPTIONS options;
	if(menu->option_num < MAX_OPTIONS) {
		/* 选项未达到最大值，向后插入选项 */
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
	/* 移除选项 */
	/* 从index+1开始，每个选项往前移动一位，一直到menu->option_num-1 */
	/* index+1 -> index | index+2 -> index+1	... */
	int i;
	for(i = index; i < menu->option_num - 1; i++) {
		menu->options[i] = menu->options[i+1];
	}
	/* 把index为option_num-1的选项的flags置0 */
	menu->options[menu->option_num - 1].flags = 0;
	menu->option_num -= 1;
}

void show_menu(struct SHTCTL *shtctl, struct MEMMAN *memman, struct MENU *menu) {
	/* 显示菜单 */
	int i;
	
	menu->flags = 1;							/* 正在显示 */
	
	/* 计算菜单栏的高度和宽度 */
	menu->mheight = menu->option_num * 25;			/* 每个选项高25 */
	menu->mwidth = 120;								/* 宽度固定120 */
	
	/* 分配缓冲区和sheet */
	menu->sht = sheet_alloc(shtctl);
	menu->sht->flags = SHEET_MENU;
	menu->buf = (unsigned int *) memman_alloc_4k(memman,menu->mheight * (menu->mwidth + 1) * 4);
	sheet_setbuf(menu->sht, menu->buf, menu->mwidth, menu->mheight + 1, -1);
	menu->sht->win = (unsigned int *)menu;
	
	/* 绘制 */
	boxfill8(menu->buf, menu->mwidth, COL_WHITE, 0, 0, menu->mwidth, menu->mheight);										/* 背景板 */
	for(i = 0;i <= MAX_OPTIONS; i++){
		if(menu->options[i].flags == 1) {
			/* 正在使用的选项(flags为1) */
			boxfill8(menu->buf, menu->mwidth, COL_BLACK,          0, i * 25,     menu->mwidth,       i * 25);				/* 上横线 */
			boxfill8(menu->buf, menu->mwidth, COL_BLACK,          0, i * 25,          0, (i + 1) * 25);						/* 左竖线 */
			boxfill8(menu->buf, menu->mwidth, COL_BLACK, menu->mwidth - 1, i * 25, menu->mwidth - 1, (i + 1) * 25);			/* 右竖线 */
			putfonts8_asc(menu->buf, menu->mwidth, 5, 5 + (i * 25), COL_BLACK, menu->options[i].title);
		}
	}
	boxfill8(menu->buf, menu->mwidth, COL_BLACK, 0, menu->mheight, menu->mwidth, menu->mheight);						/* 下横线 */
	sheet_slide(menu->sht, menu->menux, menu->menuy);
	sheet_updown(menu->sht, shtctl->top);
	return;
}

void hide_menu(struct MEMMAN *memman, struct MENU *menu) {
	/* 隐藏菜单 */
	sheet_free(menu->sht);																		/* 释放图层 */
	memman_free_4k(memman, (unsigned int)(menu->buf), menu->mheight * menu->mwidth * 4);		/* 释放图层占用的内存 */
	menu->flags = 0;																			/* 未显示 */
	return;
}

void option_change(struct MENU *menu, int mouse_y) {
	/* 处理鼠标移动到选项上 */
	menu->now = mouse_y / 25;	/* 现在选中的是哪一项 */
	/* 先判断是否和上一次选择一样 */
	if(menu->now == menu->old) {
		/* 如果一样不用改变 */
		return;
	}
	/* 如果不一样，更换旧的和新的选项的底色 */
	boxfill8(menu->buf, menu->mwidth, COL_WHITE, 1, menu->old * 25 + 1, menu->mwidth - 2 , (menu->old + 1) * 25 - 1);			/* 白色覆盖旧选项 */
	putfonts8_asc(menu->buf, menu->mwidth, 5, 5 + (menu->old * 25), COL_BLACK, menu->options[menu->old].title);					/* 重新写标题 */
	sheet_refresh(menu->sht, 0, menu->old * 25 + 1, menu->mwidth, (menu->old + 1) * 25);										/* 刷新 */
	boxfill8(menu->buf, menu->mwidth, 0xff86c1, 1, menu->now * 25 + 1, menu->mwidth - 2 , (menu->now + 1) * 25 - 1);			/* 粉色覆盖新选项 */
	putfonts8_asc(menu->buf, menu->mwidth, 5, 5 + (menu->now * 25), COL_BLACK, menu->options[menu->now].title);					/* 重新写标题 */
	sheet_refresh(menu->sht, 0, menu->now * 25 + 1, menu->mwidth, (menu->now + 1) * 25);										/* 刷新 */
	menu->old = menu->now;
	return;
}

void menu_click(struct MENU *menu, int mouse_y) {
	/* 菜单栏被单击 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	menu->now = mouse_y / 25;	/* 现在选中的是哪一项 */
	
	/* 运行程序 */
	(*(menu->options[menu->now].onOptionClick))();
	
	/* 隐藏菜单栏 */
	hide_menu(memman, menu);
	return;
}
