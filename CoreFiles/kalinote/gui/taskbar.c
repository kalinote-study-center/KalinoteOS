/* 底部任务栏 */

#include "../bootpack.h"

void onStartButtonClick(void);							/* 单击Function按钮 */
void onOpenConsoleClick(void);							/* 命令台选项被单击 */
void onShutdownClick(void);								/* 关机选项被单击 */
void onUselessClick(void);								/* 没用的选项 */
struct MENU *start_menu;								/* Function菜单 */
void onWinButtonClick();								/* 控制窗口最小化 */

void init_taskbar(struct MEMMAN *memman, struct SHEET *sht) {
	/* 初始化任务栏 */
	struct SHEET *tb_back;		/* 任务栏背景层 */
	unsigned int *tb_buf;		/* 背景层内存缓冲区 */
	struct TASKBARCTL *tbctl = (struct TASKBARCTL *) *((int *) TASKBARCTL_ADDR);
	struct BUTTON *func_button;
	int i;
	
	/* 分配一个图层给任务栏背景 */
	if(sht->subctl == 0) {
		/* 如果还没有子图层 */
		sht->subctl = shtctl_init(memman, sht->buf, sht->bxsize, sht->bysize);					/* 初始化子图层的图层控制器 */
		sht->subctl->fsheet = sht;																/* 让子图层管理器指向父图层 */
	}
	tb_back = sheet_alloc(sht->subctl);
	tb_buf = (unsigned int *) memman_alloc_4k(memman, sht->bxsize * sht->bysize * 4);		/* 分配背景图层内存 */
	sheet_setbuf(tb_back, tb_buf, sht->bxsize, sht->bysize, -1);								/* 设置背景图层缓冲区 */
	tb_back->flags = SHEET_BACK;															/* 设置图层标签为背景 */
	tb_back->win = 0;																		/* 窗口指针暂时不设置 */
	
	/* 绘制任务栏背景 */
	boxfill8(tb_back->buf, tb_back->bxsize, COL_BGREY,  0,     tb_back->bysize - 28, tb_back->bxsize -  1, tb_back->bysize - 28);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE,  0,     tb_back->bysize - 27, tb_back->bxsize -  1, tb_back->bysize - 27);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_BGREY,  0,     tb_back->bysize - 26, tb_back->bxsize -  1, tb_back->bysize -  1);
	
	/* 
	* 这一部分是以前用来绘制Function按钮的，
	* 现在直接使用make_button来生成按钮了，所以就用不上了
	*/
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE,  3,     tb_back->bysize - 24, 74,     tb_back->bysize - 24);			/* 白色上横线 */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE,  2,     tb_back->bysize - 24,  2,     tb_back->bysize -  4);			/* 白色左竖线 */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY,  3,     tb_back->bysize -  4, 74,     tb_back->bysize -  4);			/* 灰色右竖线 */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY, 74,     tb_back->bysize - 23, 74,     tb_back->bysize -  5);			/* 灰色下横线 */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_BLACK,  2,     tb_back->bysize -  3, 74,     tb_back->bysize -  3);
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_BLACK, 75,     tb_back->bysize - 24, 75,     tb_back->bysize -  3);
	
	boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY, tb_back->bxsize - 75, tb_back->bysize - 24, tb_back->bxsize -  4, tb_back->bysize - 24);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY, tb_back->bxsize - 75, tb_back->bysize - 23, tb_back->bxsize - 75, tb_back->bysize -  4);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE, tb_back->bxsize - 75, tb_back->bysize -  3, tb_back->bxsize -  4, tb_back->bysize -  3);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE, tb_back->bxsize -  3, tb_back->bysize - 24, tb_back->bxsize -  3, tb_back->bysize -  3);
	
	/* 移动背景子图层 */
	sheet_slide(tb_back, 0, 0);
	sheet_updown(tb_back, 0);
	
	/* 准备Function菜单栏 */
	start_menu = make_menu(memman, 5, 610);
	add_options(start_menu, "open console", onOpenConsoleClick);
	add_options(start_menu, "shutdown", cmd_shutdown);
	add_options(start_menu, "useless1", onUselessClick);					/* 测试使用 */
	add_options(start_menu, "useless2", onUselessClick);					/* 测试使用 */
	add_options(start_menu, "useless3", onUselessClick);					/* 测试使用 */
	remove_options(start_menu, 3);											/* 测试使用：移除useless2 */
	add_options(start_menu, "useless4", onUselessClick);					/* 测试使用 */
	
	/* Function按钮 */
	func_button = make_button(memman, 70, 23, 2, 4, "Function", COL_BGREY, onStartButtonClick);
	show_button(sht, memman, func_button);
	
	/* 初始化tackbarctl结构体 */
	tbctl->menu = start_menu;			/* Function菜单 */
	tbctl->button = func_button;		/* Fcuntion按钮 */
	tbctl->num = 0;						/* 当前窗口数量 */
	tbctl->now_row = 0;					/* 当前显示排数 */
	tbctl->total_row = 0;				/* 总排数-1 */
	for(i = 0;i <= MAX_TSKWINBTN; i++) {
		/* flags全部置0 */
		tbctl->tskwinbtns[i].flags = 0;
	}
	
	return;
}

/***************************************************
*                  菜单栏窗口操作                  *
***************************************************/

int taskbar_addwin(struct WINDOW *window) {
	/* 向任务栏加一个窗口 */
	/*
	* 窗口按钮固定为：高度23，宽度105，y位置固定4，
	* x位置公式为：x=85+110*index，其中index从0开始
	* 一排最多7个窗口
	*/
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = (struct SHEET *) *((int *) TASKBAR_ADDR);
	struct TASKBARCTL *tbctl = (struct TASKBARCTL *) *((int *) TASKBARCTL_ADDR);
	struct TSKWINBTN tskwinbtn;
	int i;
	
	if(tbctl->num < MAX_TSKWINBTN) {
		/* 还可以分配任务栏按钮 */
		debug_print("taskbar>add a taskbar button\n");	
		tskwinbtn.index = tbctl->num;
		tskwinbtn.flags = 2;						/* 默认打开 */
		tskwinbtn.row = tskwinbtn.index / 7;		/* 所在排数 */
		tskwinbtn.win = window;
		tskwinbtn.button = make_button(memman, 105, 23, 85 + (110 * (tskwinbtn.index % 7)), 4, window->wtitle, COL_BGREY, onWinButtonClick);
		if(tskwinbtn.row > tbctl->total_row) {
			/* 增加一排并重新显示 */
			tbctl->total_row += 1;
			/* 这里要做的就是把这一行按钮全部隐藏掉(hide_button) */
			/* 先找到正在显示的按钮，正在显示的按钮index是从tbctl->now_row*7到(tbctl->now_row*7)+6 */
			for(i = (tbctl->now_row)*7; i <= ((tbctl->now_row)*7)+6; i++) {
				debug_print("taskbar>hide index:%d\n",i);
				hide_button(tbctl->tskwinbtns[i].button);
			}
			/* 刷新父图层 */
			sheet_refresh(sht, 85, 4, (85 + (110 * 6)) + 105, 4 + 23);
			/* 重新显示(由于是添加窗口，所以如果是新的一行，一定是第一个，不用再判断) */
			tbctl->now_row = tbctl->total_row;
		}
		show_button(sht, memman, tskwinbtn.button);
		/* 刷新按钮区域父图层 */
		sheet_refresh(sht, 85 + (110 * (tskwinbtn.index % 7)), 4, (85 + (110 * (tskwinbtn.index % 7))) + 105, 4 + 23);
		tbctl->tskwinbtns[tbctl->num] = tskwinbtn;
		tbctl->num += 1;
		debug_print("taskbar>new button index:%d, row:%d\n",tskwinbtn.index,tskwinbtn.row);
	} else {
		/* 任务栏按钮已经满了 */
		debug_print("taskbar>Button index has been used up");
		return -1;
	}
	return tskwinbtn.index;
}

void taskbar_removewin(int index) {
	/* 从任务栏删除一个窗口 */
	/* 大概参考menu中option的思路 */
	/* 从index+1开始，每个窗口往前移动一位，直到tbctl->num-1(指的是循环计数i，因为index比对应的num少1，比如第一个窗口的index是0，第二个窗口的index是1，以此类推) */
	/* 同时重新计算每个窗口新的排数，重新计算tbctl的总排数，重新显示 */
	/* 由于现在是将index和窗口绑定，所以在调整完按钮以后要记得修改窗口对应的index，这很重要！！！！ */
	int i;
	
	struct TASKBARCTL *tbctl = (struct TASKBARCTL *) *((int *) TASKBARCTL_ADDR);
	struct SHEET *sht = (struct SHEET *) *((int *) TASKBAR_ADDR);
	release_button(tbctl->tskwinbtns[index].button);
	// sheet_refresh(sht, 85, 4, (85 + (110 * 6)) + 105, 4 + 23);
	
	/* 测试代码 */
	debug_print("taskbar>close window %d\n",index);
	debug_print("taskbar>tbctl->num - 1 = %d\n",tbctl->num - 1);
	
	for(i = index; i < tbctl->num - 1; i++) {
		/* 关闭最后一个窗口不会进入这个循环 */
		debug_print("taskbar>do window %d\n",i);
		tbctl->tskwinbtns[i + 1].win->tskwinbtn -= 1;						/* 调整窗口index */
		tbctl->tskwinbtns[i + 1].index -= 1;								/* 调整对应按钮index */
		tbctl->tskwinbtns[i] = tbctl->tskwinbtns[i + 1];
		tbctl->tskwinbtns[i].row = tbctl->tskwinbtns[i].index / 7;	/* 重新计算所在排 */
		tbctl->tskwinbtns[i].button->buttonx = 85 + (110 * (tbctl->tskwinbtns[i].index % 7));		/* 重新计算按钮位置 */
		sheet_slide(tbctl->tskwinbtns[i].button->sht, tbctl->tskwinbtns[i].button->buttonx, tbctl->tskwinbtns[i].button->buttony);		/* 移动按钮位置 */
	}
	
	/* 
	* 这里加入判断是否是一排最后一个的代码，如果是最后一个，则将上一排的全部显示
	* 且total_row-1
	* 如果是第一排(第0排)，则不处理
	* 同样，找到最后一排的按钮index的方法为tbctl->total_row*7到(tbctl->total_row*7)+6
	* 比如第0排的index是0-6
	*/
	
	/* 把最后一个的flags置0 */
	tbctl->tskwinbtns[tbctl->num - 1].flags = 0;		/* 未使用 */
	tbctl->num -= 1;
	
	/* 刷新父图层 */
	sheet_refresh(sht, 85, 4, (85 + (110 * 6)) + 105, 4 + 23);
	
	return;
}

void onWinButtonClick() {
	/* 这个函数用于控制窗口最小化 */
	return;
}

/***************************************************
*                 Function菜单功能                 *
***************************************************/

void onStartButtonClick(void) {
	/* 单击Function按钮 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
	switch(start_menu->flags) {
		case 0:show_menu(shtctl, memman, start_menu);break;
		case 1:hide_menu(memman, start_menu);break;
	}
	return;
}

void onOpenConsoleClick(void) {
	/* 命令台选项被单击 */
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) FIFO_ADDR);
	fifo32_put(fifo, 2);		/* 2是打开新的命令窗口，详细参考《系统支持文档》 */
	return;
}

void onShutdownClick(void) {
	/* 关机选项被单击 */
	cmd_shutdown();
	return;
}

void onUselessClick(void) {
	/* 没用的选项 */
	return;
}
