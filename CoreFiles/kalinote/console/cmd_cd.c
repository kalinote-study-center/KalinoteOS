/* 切换命令行目录 */
#include "../bootpack.h"
#include <string.h>

void cmd_cd(struct CONSOLE *cons, char *parameter, int *fat) {
	struct TASK *task = task_now();
	if(strcmp(parameter, ".") == 0) {
		/* 目录不变 */
		return;
	}
	/* 先查找路径是否存在 */
	if(dir_check(parameter, fat)) {
		cons_putstr0(cons,"路径存在。\n");
		if(strlen(parameter) < 256) {
			strcpy(task->dir,parameter);
			cons_printf(cons,"当前console路径已切换为：%s\n", task->dir);
		} else {
			cons_putstr0(cons,"路径过长。\n");
		}
	} else {
		cons_putstr0(cons,"路径不存在。\n");
	}
	
	return;
}
