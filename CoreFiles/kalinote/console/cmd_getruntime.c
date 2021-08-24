/* 获取系统启动运行时间 */
#include "../bootpack.h"

void cmd_getruntime(struct CONSOLE *cons) {
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	cons_printf(cons, "%u ticks(10ms)\n", sysinfo->time_counter);
	return;
}
