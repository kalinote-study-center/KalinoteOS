/* �鿴��ǰ������·�� */
#include <bootpack.h>

void cmd_pwd(struct CONSOLE *cons) {
	struct TASK *task = task_now();
	cons_printf(cons, "%s\n", task->dir);
	return;
}
