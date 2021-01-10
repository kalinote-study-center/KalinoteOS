/* 底部任务栏 */

#include "bootpack.h"

struct MENU *init_taskbar(struct MEMMAN *memman, int *vram, int x, int y){
	/* 初始化任务栏 */
	struct MENU *start_menu;
	boxfill8(vram, x, COL_BGREY,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL_WHITE,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL_BGREY,  0,     y - 26, x -  1, y -  1);
	
	boxfill8(vram, x, COL_WHITE,  3,     y - 24, 74,     y - 24);
	boxfill8(vram, x, COL_WHITE,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL_DGREY,  3,     y -  4, 74,     y -  4);
	boxfill8(vram, x, COL_DGREY, 74,     y - 23, 74,     y -  5);
	boxfill8(vram, x, COL_BLACK,  2,     y -  3, 74,     y -  3);
	boxfill8(vram, x, COL_BLACK, 75,     y - 24, 75,     y -  3);
	
	boxfill8(vram, x, COL_DGREY, x - 75, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL_DGREY, x - 75, y - 23, x - 75, y -  4);
	boxfill8(vram, x, COL_WHITE, x - 75, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL_WHITE, x -  3, y - 24, x -  3, y -  3);
	start_menu = make_menu(memman, 5, 610);
	add_options(start_menu, "open console", 0);
	add_options(start_menu, "shutdown", 1);
	add_options(start_menu, "useless1", 2);
	add_options(start_menu, "useless2", 3);
	add_options(start_menu, "useless3", 4);
	return start_menu;
}
