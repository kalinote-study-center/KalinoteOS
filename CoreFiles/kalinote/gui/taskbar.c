/* �ײ������� */

#include "../bootpack.h"

void onOpenConsoleClick(void);			/* ����̨ѡ����� */
void onShutdownClick(void);				/* �ػ�ѡ����� */
void onUselessClick(void);				/* û�õ�ѡ�� */

struct MENU *init_taskbar(struct MEMMAN *memman, int *vram, int x, int y){
	/* ��ʼ�������� */
	struct MENU *start_menu;
	boxfill8(vram, x, COL_BGREY,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL_WHITE,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL_BGREY,  0,     y - 26, x -  1, y -  1);
	
	boxfill8(vram, x, COL_WHITE,  3,     y - 24, 74,     y - 24);			/* ��ɫ�Ϻ��� */
	boxfill8(vram, x, COL_WHITE,  2,     y - 24,  2,     y -  4);			/* ��ɫ������ */
	boxfill8(vram, x, COL_DGREY,  3,     y -  4, 74,     y -  4);			/* ��ɫ������ */
	boxfill8(vram, x, COL_DGREY, 74,     y - 23, 74,     y -  5);			/* ��ɫ�º��� */
	boxfill8(vram, x, COL_BLACK,  2,     y -  3, 74,     y -  3);
	boxfill8(vram, x, COL_BLACK, 75,     y - 24, 75,     y -  3);
	
	boxfill8(vram, x, COL_DGREY, x - 75, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL_DGREY, x - 75, y - 23, x - 75, y -  4);
	boxfill8(vram, x, COL_WHITE, x - 75, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL_WHITE, x -  3, y - 24, x -  3, y -  3);
	start_menu = make_menu(memman, 5, 610);
	add_options(start_menu, "open console", onOpenConsoleClick);
	add_options(start_menu, "shutdown", onShutdownClick);
	add_options(start_menu, "useless1", onUselessClick);		/* ����ʹ�� */
	add_options(start_menu, "useless2", onUselessClick);		/* ����ʹ�� */
	add_options(start_menu, "useless3", onUselessClick);		/* ����ʹ�� */
	remove_options(start_menu, 3);								/* ����ʹ�ã��Ƴ�useless2 */
	add_options(start_menu, "useless4", onUselessClick);		/* ����ʹ�� */
	return start_menu;
}

void onOpenConsoleClick(void) {
	/* ����̨ѡ����� */
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0x0fec);
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
