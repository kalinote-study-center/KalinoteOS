/* 窗口相关程序 */

#include "../bootpack.h"
#include <string.h>

struct WINDOW *make_window8(struct SHEET *sht, int xsize, int ysize, int act_color, int deact_color, char *title, char act){
	/* 窗口窗体 */
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
	
	/* 向任务栏添加按钮 */
	if(strcmp(title, "DEBUG console") != 0){
		/* 不是DEBUG console */
		window->tskwinbtn = taskbar_addwin(window);
	}
	
	debug_print("window>create new window,index:%d\n",window->tskwinbtn);
	
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
