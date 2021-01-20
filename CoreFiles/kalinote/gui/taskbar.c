/* 底部任务栏 */

#include "../bootpack.h"

void onOpenConsoleClick(void);			/* 命令台选项被单击 */
void onShutdownClick(void);				/* 关机选项被单击 */
void onUselessClick(void);				/* 没用的选项 */

struct MENU *init_taskbar(struct MEMMAN *memman, int *vram, int x, int y){
	/* 初始化任务栏 */
	struct MENU *start_menu;
	boxfill8(vram, x, COL_BGREY,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL_WHITE,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL_BGREY,  0,     y - 26, x -  1, y -  1);
	
	boxfill8(vram, x, COL_WHITE,  3,     y - 24, 74,     y - 24);			/* 白色上横线 */
	boxfill8(vram, x, COL_WHITE,  2,     y - 24,  2,     y -  4);			/* 白色左竖线 */
	boxfill8(vram, x, COL_DGREY,  3,     y -  4, 74,     y -  4);			/* 灰色右竖线 */
	boxfill8(vram, x, COL_DGREY, 74,     y - 23, 74,     y -  5);			/* 灰色下横线 */
	boxfill8(vram, x, COL_BLACK,  2,     y -  3, 74,     y -  3);
	boxfill8(vram, x, COL_BLACK, 75,     y - 24, 75,     y -  3);
	
	boxfill8(vram, x, COL_DGREY, x - 75, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL_DGREY, x - 75, y - 23, x - 75, y -  4);
	boxfill8(vram, x, COL_WHITE, x - 75, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL_WHITE, x -  3, y - 24, x -  3, y -  3);
	start_menu = make_menu(memman, 5, 610);
	add_options(start_menu, "open console", onOpenConsoleClick);
	add_options(start_menu, "shutdown", onShutdownClick);
	add_options(start_menu, "useless1", onUselessClick);		/* 测试使用 */
	add_options(start_menu, "useless2", onUselessClick);		/* 测试使用 */
	add_options(start_menu, "useless3", onUselessClick);		/* 测试使用 */
	remove_options(start_menu, 3);								/* 测试使用：移除useless2 */
	add_options(start_menu, "useless4", onUselessClick);		/* 测试使用 */
	return start_menu;
}

void onOpenConsoleClick(void) {
	/* 命令台选项被单击 */
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0x0fec);
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
