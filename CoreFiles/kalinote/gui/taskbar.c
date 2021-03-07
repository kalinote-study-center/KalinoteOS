/* �ײ������� */

#include "../bootpack.h"

void onStartButtonClick(void);							/* ����Function��ť */
void onOpenConsoleClick(void);							/* ����̨ѡ����� */
void onShutdownClick(void);								/* �ػ�ѡ����� */
void onUselessClick(void);								/* û�õ�ѡ�� */
struct MENU *start_menu;								/* Function�˵� */
void onWinButtonClick();								/* ���ƴ�����С�� */

void init_taskbar(struct MEMMAN *memman, struct SHEET *sht) {
	/* ��ʼ�������� */
	struct SHEET *tb_back;		/* ������������ */
	unsigned int *tb_buf;		/* �������ڴ滺���� */
	struct TASKBARCTL *tbctl = (struct TASKBARCTL *) *((int *) TASKBARCTL_ADDR);
	struct BUTTON *func_button;
	int i;
	
	/* ����һ��ͼ������������� */
	if(sht->subctl == 0) {
		/* �����û����ͼ�� */
		sht->subctl = shtctl_init(memman, sht->buf, sht->bxsize, sht->bysize);					/* ��ʼ����ͼ���ͼ������� */
		sht->subctl->fsheet = sht;																/* ����ͼ�������ָ��ͼ�� */
	}
	tb_back = sheet_alloc(sht->subctl);
	tb_buf = (unsigned int *) memman_alloc_4k(memman, sht->bxsize * sht->bysize * 4);		/* ���䱳��ͼ���ڴ� */
	sheet_setbuf(tb_back, tb_buf, sht->bxsize, sht->bysize, -1);								/* ���ñ���ͼ�㻺���� */
	tb_back->flags = SHEET_BACK;															/* ����ͼ���ǩΪ���� */
	tb_back->win = 0;																		/* ����ָ����ʱ������ */
	
	/* �������������� */
	boxfill8(tb_back->buf, tb_back->bxsize, COL_BGREY,  0,     tb_back->bysize - 28, tb_back->bxsize -  1, tb_back->bysize - 28);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE,  0,     tb_back->bysize - 27, tb_back->bxsize -  1, tb_back->bysize - 27);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_BGREY,  0,     tb_back->bysize - 26, tb_back->bxsize -  1, tb_back->bysize -  1);
	
	/* 
	* ��һ��������ǰ��������Function��ť�ģ�
	* ����ֱ��ʹ��make_button�����ɰ�ť�ˣ����Ծ��ò�����
	*/
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE,  3,     tb_back->bysize - 24, 74,     tb_back->bysize - 24);			/* ��ɫ�Ϻ��� */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE,  2,     tb_back->bysize - 24,  2,     tb_back->bysize -  4);			/* ��ɫ������ */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY,  3,     tb_back->bysize -  4, 74,     tb_back->bysize -  4);			/* ��ɫ������ */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY, 74,     tb_back->bysize - 23, 74,     tb_back->bysize -  5);			/* ��ɫ�º��� */
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_BLACK,  2,     tb_back->bysize -  3, 74,     tb_back->bysize -  3);
	// boxfill8(tb_back->buf, tb_back->bxsize, COL_BLACK, 75,     tb_back->bysize - 24, 75,     tb_back->bysize -  3);
	
	boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY, tb_back->bxsize - 75, tb_back->bysize - 24, tb_back->bxsize -  4, tb_back->bysize - 24);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_DGREY, tb_back->bxsize - 75, tb_back->bysize - 23, tb_back->bxsize - 75, tb_back->bysize -  4);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE, tb_back->bxsize - 75, tb_back->bysize -  3, tb_back->bxsize -  4, tb_back->bysize -  3);
	boxfill8(tb_back->buf, tb_back->bxsize, COL_WHITE, tb_back->bxsize -  3, tb_back->bysize - 24, tb_back->bxsize -  3, tb_back->bysize -  3);
	
	/* �ƶ�������ͼ�� */
	sheet_slide(tb_back, 0, 0);
	sheet_updown(tb_back, 0);
	
	/* ׼��Function�˵��� */
	start_menu = make_menu(memman, 5, 610);
	add_options(start_menu, "open console", onOpenConsoleClick);
	add_options(start_menu, "shutdown", cmd_shutdown);
	add_options(start_menu, "useless1", onUselessClick);					/* ����ʹ�� */
	add_options(start_menu, "useless2", onUselessClick);					/* ����ʹ�� */
	add_options(start_menu, "useless3", onUselessClick);					/* ����ʹ�� */
	remove_options(start_menu, 3);											/* ����ʹ�ã��Ƴ�useless2 */
	add_options(start_menu, "useless4", onUselessClick);					/* ����ʹ�� */
	
	/* Function��ť */
	func_button = make_button(memman, 70, 23, 2, 4, "Function", COL_BGREY, onStartButtonClick);
	show_button(sht, memman, func_button);
	
	/* ��ʼ��tackbarctl�ṹ�� */
	tbctl->menu = start_menu;			/* Function�˵� */
	tbctl->button = func_button;		/* Fcuntion��ť */
	tbctl->num = 0;						/* ��ǰ�������� */
	tbctl->now_row = 0;					/* ��ǰ��ʾ���� */
	tbctl->total_row = 0;				/* ������-1 */
	for(i = 0;i <= MAX_TSKWINBTN; i++) {
		/* flagsȫ����0 */
		tbctl->tskwinbtns[i].flags = 0;
	}
	
	return;
}

/***************************************************
*                  �˵������ڲ���                  *
***************************************************/

int taskbar_addwin(struct WINDOW *window) {
	/* ����������һ������ */
	/*
	* ���ڰ�ť�̶�Ϊ���߶�23�����105��yλ�ù̶�4��
	* xλ�ù�ʽΪ��x=85+110*index������index��0��ʼ
	* һ�����7������
	*/
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = (struct SHEET *) *((int *) TASKBAR_ADDR);
	struct TASKBARCTL *tbctl = (struct TASKBARCTL *) *((int *) TASKBARCTL_ADDR);
	struct TSKWINBTN tskwinbtn;
	int i;
	
	if(tbctl->num < MAX_TSKWINBTN) {
		/* �����Է�����������ť */
		debug_print("taskbar>add a taskbar button\n");	
		tskwinbtn.index = tbctl->num;
		tskwinbtn.flags = 2;						/* Ĭ�ϴ� */
		tskwinbtn.row = tskwinbtn.index / 7;		/* �������� */
		tskwinbtn.win = window;
		tskwinbtn.button = make_button(memman, 105, 23, 85 + (110 * (tskwinbtn.index % 7)), 4, window->wtitle, COL_BGREY, onWinButtonClick);
		if(tskwinbtn.row > tbctl->total_row) {
			/* ����һ�Ų�������ʾ */
			tbctl->total_row += 1;
			/* ����Ҫ���ľ��ǰ���һ�а�ťȫ�����ص�(hide_button) */
			/* ���ҵ�������ʾ�İ�ť��������ʾ�İ�ťindex�Ǵ�tbctl->now_row*7��(tbctl->now_row*7)+6 */
			for(i = (tbctl->now_row)*7; i <= ((tbctl->now_row)*7)+6; i++) {
				debug_print("taskbar>hide index:%d\n",i);
				hide_button(tbctl->tskwinbtns[i].button);
			}
			/* ˢ�¸�ͼ�� */
			sheet_refresh(sht, 85, 4, (85 + (110 * 6)) + 105, 4 + 23);
			/* ������ʾ(��������Ӵ��ڣ�����������µ�һ�У�һ���ǵ�һ�����������ж�) */
			tbctl->now_row = tbctl->total_row;
		}
		show_button(sht, memman, tskwinbtn.button);
		/* ˢ�°�ť����ͼ�� */
		sheet_refresh(sht, 85 + (110 * (tskwinbtn.index % 7)), 4, (85 + (110 * (tskwinbtn.index % 7))) + 105, 4 + 23);
		tbctl->tskwinbtns[tbctl->num] = tskwinbtn;
		tbctl->num += 1;
		debug_print("taskbar>new button index:%d, row:%d\n",tskwinbtn.index,tskwinbtn.row);
	} else {
		/* ��������ť�Ѿ����� */
		debug_print("taskbar>Button index has been used up");
		return -1;
	}
	return tskwinbtn.index;
}

void taskbar_removewin(int index) {
	/* ��������ɾ��һ������ */
	/* ��Ųο�menu��option��˼· */
	/* ��index+1��ʼ��ÿ��������ǰ�ƶ�һλ��ֱ��tbctl->num-1(ָ����ѭ������i����Ϊindex�ȶ�Ӧ��num��1�������һ�����ڵ�index��0���ڶ������ڵ�index��1���Դ�����) */
	/* ͬʱ���¼���ÿ�������µ����������¼���tbctl����������������ʾ */
	/* ���������ǽ�index�ʹ��ڰ󶨣������ڵ����갴ť�Ժ�Ҫ�ǵ��޸Ĵ��ڶ�Ӧ��index�������Ҫ�������� */
	int i;
	
	struct TASKBARCTL *tbctl = (struct TASKBARCTL *) *((int *) TASKBARCTL_ADDR);
	struct SHEET *sht = (struct SHEET *) *((int *) TASKBAR_ADDR);
	release_button(tbctl->tskwinbtns[index].button);
	// sheet_refresh(sht, 85, 4, (85 + (110 * 6)) + 105, 4 + 23);
	
	/* ���Դ��� */
	debug_print("taskbar>close window %d\n",index);
	debug_print("taskbar>tbctl->num - 1 = %d\n",tbctl->num - 1);
	
	for(i = index; i < tbctl->num - 1; i++) {
		/* �ر����һ�����ڲ���������ѭ�� */
		debug_print("taskbar>do window %d\n",i);
		tbctl->tskwinbtns[i + 1].win->tskwinbtn -= 1;						/* ��������index */
		tbctl->tskwinbtns[i + 1].index -= 1;								/* ������Ӧ��ťindex */
		tbctl->tskwinbtns[i] = tbctl->tskwinbtns[i + 1];
		tbctl->tskwinbtns[i].row = tbctl->tskwinbtns[i].index / 7;	/* ���¼��������� */
		tbctl->tskwinbtns[i].button->buttonx = 85 + (110 * (tbctl->tskwinbtns[i].index % 7));		/* ���¼��㰴ťλ�� */
		sheet_slide(tbctl->tskwinbtns[i].button->sht, tbctl->tskwinbtns[i].button->buttonx, tbctl->tskwinbtns[i].button->buttony);		/* �ƶ���ťλ�� */
	}
	
	/* 
	* ��������ж��Ƿ���һ�����һ���Ĵ��룬��������һ��������һ�ŵ�ȫ����ʾ
	* ��total_row-1
	* ����ǵ�һ��(��0��)���򲻴���
	* ͬ�����ҵ����һ�ŵİ�ťindex�ķ���Ϊtbctl->total_row*7��(tbctl->total_row*7)+6
	* �����0�ŵ�index��0-6
	*/
	
	/* �����һ����flags��0 */
	tbctl->tskwinbtns[tbctl->num - 1].flags = 0;		/* δʹ�� */
	tbctl->num -= 1;
	
	/* ˢ�¸�ͼ�� */
	sheet_refresh(sht, 85, 4, (85 + (110 * 6)) + 105, 4 + 23);
	
	return;
}

void onWinButtonClick() {
	/* ����������ڿ��ƴ�����С�� */
	return;
}

/***************************************************
*                 Function�˵�����                 *
***************************************************/

void onStartButtonClick(void) {
	/* ����Function��ť */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
	switch(start_menu->flags) {
		case 0:show_menu(shtctl, memman, start_menu);break;
		case 1:hide_menu(memman, start_menu);break;
	}
	return;
}

void onOpenConsoleClick(void) {
	/* ����̨ѡ����� */
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) FIFO_ADDR);
	fifo32_put(fifo, 2);		/* 2�Ǵ��µ�����ڣ���ϸ�ο���ϵͳ֧���ĵ��� */
	return;
}

void onShutdownClick(void) {
	/* �ػ�ѡ����� */
	cmd_shutdown();
	return;
}

void onUselessClick(void) {
	/* û�õ�ѡ�� */
	return;
}
