/* 按钮 */
#include "../bootpack.h"

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

void show_button(struct SHEET *sht, struct MEMMAN *memman, struct BUTTON *button) {
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
	return;
}

void change_button(struct BUTTON *button, struct SHEET *fsht, char click) {
	/* 更改按钮凸起和按下或禁用的效果 */
	
	/****************************************
	*         这里还有问题需要解决          *
	****************************************/
	if(button->click_old == click) {
		/* 不用改变 */
		return;
	}
	if(button->show == 0) {
		/* 不可视 */
		button->click_old = click;
		return;
	}
	if(click == 0) {
		/* 没有按下 */
		button->click_old = click;
		/* 画按钮 */
		boxfill8(button->sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* 背景板 */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,						 0,		button->height - 1);			/* 白色左竖线 */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,		 button->width - 1,						 0);			/* 白色上横线 */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, button->width - 2,					0,		 button->width - 1,		button->height - 1);			/* 灰色右竖线 */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				0, button->height - 2,		 button->width - 1,		button->height - 2);			/* 灰色下横线 */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* 黑色右竖线 */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* 黑色下横线 */
		/* 写标题 */
		putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	} else {
		/* 按下 */
		button->click_old = click;
		/* 画按钮 */
		boxfill8(button->sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* 背景板 */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK,					0, 					0,						 0,		button->height - 1);			/* 黑色左竖线 */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK,					0, 					0,		 button->width - 1,						 0);			/* 黑色上横线 */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				1,					1,		 button->width - 2,						 1);			/* 灰色上横线 */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				1, 					1,						 1,		button->height - 2);			/* 灰色左竖线 */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* 白色右竖线 */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* 白色下横线 */
		/* 写标题 */
		putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	}
	sheet_refresh(button->sht,0,0,button->sht->bxsize,button->sht->bysize);		/* 刷新子图层 */
	sheet_refresh(fsht, button->sht->vx0, button->sht->vy0, button->sht->vx0 + button->sht->bxsize, button->sht->vy0 + button->sht->bysize);		/* 刷新父图层 */
	return;
}

void hide_button() {
	/* 隐藏按钮 */
	
}

void click_button(struct BUTTON *button) {
	/* 点击按钮 */
	
	/* 运行程序 */
	(*(button->onButtonClick))();
}
