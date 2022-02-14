/* 输入框 */
#include <bootpack.h>

void make_textbox_old(struct SHEET *sht, int x0, int y0, int sx, int sy, int c){
	/* 因为在open_console中用到了这个函数(it dose work)，所以不敢乱动 */
	/* 只能新开一个函数写了 */
	int x1 = x0 + sx, y1 = y0 + sy;
	boxfill8(sht->buf, sht->bxsize, COL_DGREY, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	boxfill8(sht->buf, sht->bxsize, COL_DGREY, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL_WHITE, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL_WHITE, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL_BLACK, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	boxfill8(sht->buf, sht->bxsize, COL_BLACK, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	boxfill8(sht->buf, sht->bxsize, COL_BGREY, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL_BGREY, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, 		c, x0 - 1, y0 - 1, x1 + 0, y1 + 0);
	return;
}

struct TEXTBOX *make_textbox(struct SHTCTL *shtctl, int x0, int y0, int height, int width, int back_color, unsigned int *buf, int char_num) {
	/* 创建一个textbox结构体 */
	/*
	* 参数：
	* 	struct SHTCTL *shtctl			// 父图层的shtctl
	* 	int x0;                         // textbox的x位置
	* 	int y0;                         // textbox的y位置
	* 	int height;                     // textbox的x长度
	*	int width;                      // textbox的y长度
	*	int c;                          // textbox的背景颜色
	* 返回值：
	* 	struct TEXTBOX *textbox			// 生成的textbox数据结构
	*/
	// struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	// struct SHEET *sht = sheet_alloc(shtctl);
	// struct TEXTBOX *textbox = (struct TEXTBOX *)memman_alloc_4k(memman,sizeof(struct TEXTBOX));
	
	// /* 初始化结构体信息(TODO)这里会出问题 */
	// textbox->x = x0;
	// textbox->y = y0;
	// textbox->height = height;
	// textbox->width = width;
	// textbox->sht = sht;
	// textbox->flags = 0x1;		/* 激活状态，不可视，单行(bitmap,低1位标识是否激活，低2位表示是否可视，低3位标识是否多行) */
	// textbox->cur_x = 0;
	// textbox->cur_y = 0;
	// textbox->cur_c = 0;			/* 默认关闭光标 */
	// textbox->char_num = char_num;
	// textbox->back_color = back_color;

	// /* (TODO)这里把图层放上去，然后将图层放置到-1层 */
	// /* textbox的sheet设置 */
	// textbox->sht->buf = buf;
	// make_textbox_old(textbox->sht, 0, 0, textbox->width, textbox->height, textbox->back_color);
	// sheet_slide(textbox->sht, textbox->x, textbox->y);
	// sheet_updown(textbox->sht, -1);
	
	// return textbox;
}

void release_textbox(struct TEXTBOX *textbox) {
	return;
}

void show_textbox(struct TEXTBOX *textbox) {
	// textbox->flags |= (1 << 1);		/* 第二位置1，表示可视 */
	// /* (TODO)把图层提上来 */
	// sheet_updown(textbox->sht, textbox->sht->ctl->top + 1);
	return;
}
