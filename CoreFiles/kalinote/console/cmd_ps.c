/* ps命令(查看所有task) */
#include "../bootpack.h"

void cmd_ps(struct CONSOLE *cons) {
	int tasks_num = 0;		// 任务总数
	int i;
	cons_printf(cons, "TASK ID     STATE    CMDLINE\n");
	for(i = 0; i < MAX_TASKS; i++) {
		if(taskctl->tasks0[i].flags != 0) {
			cons_printf(cons, "%04d        %02d       %s\n", i, taskctl->tasks0[i].flags, taskctl->tasks0[i].cmdline);
			tasks_num++;
		}
	}
	cons_printf(cons,"total %d tasks.\n",tasks_num);
	return;
}
