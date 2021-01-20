/* 窗口相关程序 */

#include "../bootpack.h"

struct WINDOW *make_window8(struct SHEET *sht, int xsize, int ysize, int act_color, int deact_color, char *title, char act){
	/* 窗口窗体 */
	/* 暂时还没有引入窗口结构体 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;		/* 内存管理程序 */
	struct WINDOW *window;
	window = (struct WINDOW *) memman_alloc_4k(memman, sizeof (struct WINDOW));			/* 给窗口信息结构体分配一段内存 */
	window->wtitle  = title;															/* 窗口标题 */
	window->xsize = xsize;																/* 窗口x大小	*/
	window->ysize = ysize;																/* 窗口y大小 */
	window->buf = sht->buf;																/* 窗口图形缓冲区 */
	window->whandle = (int)sht;															/* 窗口句柄 */		/* 这里后面再改成底层图层 */
	window->wcolor.act_color = act_color;												/* 窗口聚焦色(粉色) */
	window->wcolor.dis_act_color = deact_color;											/* 窗口未聚焦色(暗粉色) */
	window->wcolor.back_color = COL_WHITE;												/* 背景色 */
	sht->win = (unsigned int *)window;													/* 让底层图层指向窗口 */
	boxfill8(sht->buf, xsize, 					COL_BGREY, 0,         0,         xsize - 1, 0        );
	boxfill8(sht->buf, xsize, 					COL_WHITE, 1,         1,         xsize - 2, 1        );
	boxfill8(sht->buf, xsize, 					COL_BGREY, 0,         0,         0,         ysize - 1);
	boxfill8(sht->buf, xsize, 					COL_WHITE, 1,         1,         1,         ysize - 2);
	boxfill8(sht->buf, xsize,					COL_DGREY, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(sht->buf, xsize,					COL_WHITE, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(sht->buf, xsize, 	window->wcolor.back_color, 2,         2,         xsize - 3, ysize - 3);				/* 主背景 */
	boxfill8(sht->buf, xsize,					COL_DGREY, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(sht->buf, xsize,					COL_WHITE, 0,         ysize - 1, xsize - 1, ysize - 1);
	make_wtitle8(window, act);
	return window;
}

void make_icon(struct WINDOW *window, char type){
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
			window->buf[(5 + y) * window->xsize + (x + 5)] = c;
		}
	}
	return;
}

// void make_wtitle8(unsigned int *buf, int xsize, char *title, char act){
void make_wtitle8(struct WINDOW *window, char act){
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
		tbc = window->wcolor.act_color;
	} else {
		tc = COL_BGREY;
		tbc = window->wcolor.dis_act_color;
	}
	boxfill8(window->buf, window->xsize, tbc, 3, 3, window->xsize - 4, 20);
	putfonts8_asc(window->buf, window->xsize, 24, 4, tc, window->wtitle);
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
			window->buf[(5 + y) * window->xsize + (window->xsize - 40 + x)] = c;
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
	/* 改变窗口标题栏颜色 */
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
*                           菜单功能                           *
***************************************************************/

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

/***************************************************************
*                           按钮功能                           *
***************************************************************/

struct BUTTON *make_button(struct MEMMAN *memman, int width, int height, int buttonx, int buttony, char *title, int back_color, void(*onButtonClick)()) {
	/* 设置一个按钮 */
	struct BUTTON *button;
	button = (struct BUTTON *) memman_alloc_4k(memman, sizeof (struct BUTTON));		/* 给按钮(结构体)分配一个空间 */
	button->title = title;							/* 按钮标题 */
	button->height = height;						/* 按钮宽度 */
	button->width = width;							/* 按钮高度 */
	button->buttonx = buttonx;						/* 按钮X坐标 */
	button->buttony = buttony;						/* 按钮Y坐标 */
	button->back_color = back_color;				/* 按钮背景颜色 */
	button->flags = 1;								/* 启用(0是禁用) */
	button->click_old = 0;							/* (上一次)没有按下 */
	button->show = 0;								/* 不可视 */
	button->onButtonClick = onButtonClick;			/* 设置按钮功能 */
	return button;
}

struct SHEET *show_button(struct SHEET *sht, struct MEMMAN *memman, struct BUTTON *button) {
	/* 显示按钮 */
	struct SHEET *btn_sht;
	unsigned int *btn_buf;
	
	button->show = 1;			/* 设置可视 */
	
	/* 分配缓冲区和sheet */
	/* 按钮的sheet是在窗口或其他图层内的子sheet */
	if(sht->subctl == 0) {
		/* 如果还没有子图层 */
		sht->subctl = shtctl_init(memman, sht->buf, sht->bxsize, sht->bysize);					/* 初始化子图层的图层控制器 */
		sht->subctl->fsheet = sht;																/* 让子图层管理器指向父图层 */
	}
	btn_sht = sheet_alloc(sht->subctl);															/* 按钮图层 */
	btn_buf = (unsigned int *) memman_alloc_4k(memman, button->height * button->width * 4);		/* 分配按钮图层内存 */
	sheet_setbuf(btn_sht, btn_buf, button->width, button->height, -1);							/* 设置按钮图形缓冲区 */
	btn_sht->flags = SHEET_BUTTON;								/* 设置图层标签为按钮 */
	btn_sht->win = (unsigned int *)button;						/* 图层指向按钮 */
	button->sht = btn_sht;
	
	/* 画按钮 */
	boxfill8(button->sht->buf,	button->width,	button->back_color,					0,					0,		 button->width - 1,		button->height - 1);			/* 背景板 */
	boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,						 0,		button->height - 1);			/* 白色左竖线 */
	boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,		 button->width - 1,						 0);			/* 白色上横线 */
	boxfill8(button->sht->buf,	button->width,			 COL_DGREY, button->width - 2,					0,		 button->width - 1,		button->height - 1);			/* 灰色右竖线 */
	boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				0, button->height - 2,		 button->width - 1,		button->height - 2);			/* 灰色下横线 */
	boxfill8(button->sht->buf,	button->width,			 COL_BLACK,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* 黑色右竖线 */
	boxfill8(button->sht->buf,	button->width,			 COL_BLACK, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* 黑色下横线 */
	/* 写标题 */
	putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	
	/* 移动图层 */
	sheet_slide(button->sht, button->buttonx, button->buttony);
	if(button->sht->ctl->top < 0) {
		/* 如果还没有图层 */
		sheet_updown(button->sht, 0);
	} else {
		sheet_updown(button->sht, button->sht->ctl->top);
	}
	return btn_sht;
}

void change_button(struct BUTTON *button, struct SHEET *sht, char click) {
	/* 更改按钮凸起和按下或禁用的效果 */
	
	/****************************************
	*         这里还有问题需要解决          *
	****************************************/
	// if(button->click_old == click) {
		// /* 不用改变 */
		// return;
	// }
	// if(button->show == 0) {
		// /* 不可视 */
		// button->click_old = click;
		// return;
	// }
	// if(click == 0) {
		// /* 没有按下 */
		// button->click_old = click;
		// /* 画按钮 */
		// boxfill8(sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* 背景板 */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE,					0, 					0,						 0,		button->height - 1);			/* 白色左竖线 */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE,					0, 					0,		 button->width - 1,						 0);			/* 白色上横线 */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, button->width - 2,					0,		 button->width - 1,		button->height - 1);			/* 灰色右竖线 */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, 				0, button->height - 2,		 button->width - 1,		button->height - 2);			/* 灰色下横线 */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* 黑色右竖线 */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* 黑色下横线 */
		// /* 写标题 */
		// putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	// } else {
		// /* 按下 */
		// button->click_old = click;
		// /* 画按钮 */
		// boxfill8(sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* 背景板 */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK,					0, 					0,						 0,		button->height - 1);			/* 黑色左竖线 */
		// boxfill8(sht->buf,	button->width,			 COL_BLACK,					0, 					0,		 button->width - 1,						 0);			/* 黑色上横线 */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, 				1,					1,		 button->width - 2,						 1);			/* 灰色上横线 */
		// boxfill8(sht->buf,	button->width,			 COL_DGREY, 				1, 					1,						 1,		button->height - 2);			/* 灰色左竖线 */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* 白色右竖线 */
		// boxfill8(sht->buf,	button->width,			 COL_WHITE, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* 白色下横线 */
		// /* 写标题 */
		// putfonts8_asc(sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	// }
	// sheet_refresh(sht,0,0,sht->bxsize,sht->bysize);
	return;
}

void hide_button() {
	/* 隐藏按钮 */
	
}
