/* ��ť */
#include "../bootpack.h"

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

void show_button(struct SHEET *sht, struct MEMMAN *memman, struct BUTTON *button) {
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
	return;
}

void change_button(struct BUTTON *button, struct SHEET *fsht, char click) {
	/* ���İ�ť͹��Ͱ��»���õ�Ч�� */
	
	/****************************************
	*         ���ﻹ��������Ҫ���          *
	****************************************/
	if(button->click_old == click) {
		/* ���øı� */
		return;
	}
	if(button->show == 0) {
		/* ������ */
		button->click_old = click;
		return;
	}
	if(click == 0) {
		/* û�а��� */
		button->click_old = click;
		/* ����ť */
		boxfill8(button->sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,						 0,		button->height - 1);			/* ��ɫ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE,					0, 					0,		 button->width - 1,						 0);			/* ��ɫ�Ϻ��� */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, button->width - 2,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				0, button->height - 2,		 button->width - 1,		button->height - 2);			/* ��ɫ�º��� */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* ��ɫ�º��� */
		/* д���� */
		putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	} else {
		/* ���� */
		button->click_old = click;
		/* ����ť */
		boxfill8(button->sht->buf,	button->width,	button->back_color,					0,					0,		 	 button->width,			button->height);			/* ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK,					0, 					0,						 0,		button->height - 1);			/* ��ɫ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_BLACK,					0, 					0,		 button->width - 1,						 0);			/* ��ɫ�Ϻ��� */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				1,					1,		 button->width - 2,						 1);			/* ��ɫ�Ϻ��� */
		boxfill8(button->sht->buf,	button->width,			 COL_DGREY, 				1, 					1,						 1,		button->height - 2);			/* ��ɫ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE,	button->width - 1,					0,		 button->width - 1,		button->height - 1);			/* ��ɫ������ */
		boxfill8(button->sht->buf,	button->width,			 COL_WHITE, 				0, button->height - 1,		 button->width - 1,		button->height - 1);			/* ��ɫ�º��� */
		/* д���� */
		putfonts8_asc(button->sht->buf, button->width, 2, ((button->height) / 2) - 8, COL_BLACK, button->title);
	}
	sheet_refresh(button->sht,0,0,button->sht->bxsize,button->sht->bysize);		/* ˢ����ͼ�� */
	sheet_refresh(fsht, button->sht->vx0, button->sht->vy0, button->sht->vx0 + button->sht->bxsize, button->sht->vy0 + button->sht->bysize);		/* ˢ�¸�ͼ�� */
	return;
}

void hide_button() {
	/* ���ذ�ť */
	
}

void click_button(struct BUTTON *button) {
	/* �����ť */
	
	/* ���г��� */
	(*(button->onButtonClick))();
}
