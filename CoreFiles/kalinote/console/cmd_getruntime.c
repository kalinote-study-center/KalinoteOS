/* 获取系统启动运行时间 */
#include "../bootpack.h"

void cmd_getruntime(struct CONSOLE *cons) {
	cons_printf(cons, "%u ticks(10ms)\n", timerctl.count);
	return;
}
