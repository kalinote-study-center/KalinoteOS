/* 窗口相关程序 */

#include "bootpack.h"

void make_window8(unsigned int *buf, int xsize, int ysize, char *title, char act){
	/* 窗口窗体 */
	/* 暂时还没有引入窗口结构体 */
	// struct WINDOW window;
	// window.wtitle  = title;
	// window.xsize = xsize;
	// window.ysize = ysize;
	// window.wcolor.act_color = 0x00cd9b9b;			/* 窗口聚焦色(粉色) */
	// window.wcolor.dis_act_color 0x00ffc1c1;			/* 窗口未聚焦色(暗粉色) */
	// window.wcolor.back_color = COL_WHITE;			/* 背景色 */
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
	* 绘制窗口和任务栏(暂时没有)的logo
	* type参数为窗口类型，如果是0则不显示icon
	* icon_pic为0则使用默认
	* 具体的窗口类型代码后面再来定义
	*/
	int x, y, c, i, j;
	char icon_pic[16][16];
	static char icon_cmd[16][16] = {
		/* 命令行logo */
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
	
	/* 效率太低，不过后面再改进吧 */
	if (type == 1){
		for(i = 0;i < 16;i++)
			for(j = 0;j < 16;j++)
				icon_pic[i][j] = icon_cmd[i][j];
	}

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			c = icon_pic[y][x];
			if (c == '0') {
				/* 黑色 */
				c = 0x000000;
			} else if (c == '1') {
				/* 蓝色 */
				c = 0x0000ff;
			} else if (c == '2') {
				/* 绿色 */
				c = 0x00ff00;
			} else if (c == '3') {
				/* 浅绿色 */
				c = 0x53ff53;
			} else if (c == '4') {
				/* 红色 */
				c = 0xff0000;
			} else if (c == '5') {
				/* 紫色 */
				c = 0x800080;
			} else if (c == '6') {
				/* 黄色 */
				c = 0xffff00;
			} else if (c == '7') {
				/* 白色 */
				c = 0xffffff;
			} else if (c == '8') {
				/* 灰色 */
				c = 0xcccccc;
			} else if (c == '9') {
				/* 淡蓝色 */
				c = 0x00ffff;
			} else if (c == 'A') {
				/* 淡绿色 */
				c = 0x90ee90;
			} else if (c == 'B') {
				/* 淡浅绿色 */
				c = 0xbbffbb;
			} else if (c == 'C') {
				/* 淡红色 */
				c = 0xff5151;
			} else if (c == 'D') {
				/* 淡紫色 */
				c = 0xda70d6;
			} else if (c == 'E') {
				/* 淡黄色 */
				c = 0xe3cf57;
			} else if (c == 'F') {
				/* 淡白色 */
				c = 0xffffff;
			}
			buf[(5 + y) * xsize + (x + 5)] = c;
		}
	}
	return;
}

void make_wtitle8(unsigned int *buf, int xsize, char *title, char act){
	/* 窗口标题栏 */
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
	/* 最小化窗口按钮 */
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
	/* 关闭窗口按钮 */
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
	/* 改变窗口标题栏颜色 */
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
*                           菜单功能                           *
***************************************************************/

struct MENU *make_menu(struct MEMMAN *memman, int menux, int menuy) {
	/* 创建菜单结构体 */
	struct MENU *menu;
	menu = (struct MENU *) memman_alloc_4k(memman, sizeof (struct MENU));		/* 给菜单分配一个空间 */
	menu->menux = menux;			/* 菜单X坐标 */
	menu->menuy = menuy;			/* 菜单Y坐标 */
	menu->flags = 0;				/* 未显示 */
	menu->now = 0;					/* 默认选中index=0(第一项) */
	menu->old = 0;
	menu->option_num = 0;			/* 选项数量 */
	return menu;
}

void add_options(struct MENU *menu, char *option_title, unsigned char index) {
	/* 创建选项结构体 */
	struct OPTIONS options;
	if(index <= MAX_OPTIONS) {
		/* index需要在0到256之间 */
		options.title = option_title;
		options.index = index;
		options.flags = 1;
		menu->options[index] = options;
		menu->option_num += 1;
	}
	return;
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
	boxfill8(menu->buf, menu->mwidth, COL_BLACK, 0, menu->mheight - 1, menu->mwidth, menu->mheight);						/* 下横线 */
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
