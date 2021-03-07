/* 测试系统功能专用指令 */
#include "../bootpack.h"
#include <stdio.h>

void cmd_systest(struct CONSOLE *cons) {
	/* 测试系统功能专用指令 */
	debug_print("command>run systest at %d\n",cons);
	return;
}
