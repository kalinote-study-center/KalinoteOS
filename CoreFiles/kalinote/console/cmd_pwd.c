/* 查看当前命令行路径 */
#include <bootpack.h>

void cmd_pwd(struct CONSOLE *cons) {
	struct TASK *task = task_now();
	cons_printf(cons, "%s\n", task->dir);
	return;
}
