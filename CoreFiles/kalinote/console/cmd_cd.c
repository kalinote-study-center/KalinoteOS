/* �л�������Ŀ¼ */
#include "../bootpack.h"
#include <string.h>

void cmd_cd(struct CONSOLE *cons, char *parameter, int *fat) {
	struct TASK *task = task_now();
	if(strcmp(parameter, ".") == 0) {
		/* Ŀ¼���� */
		return;
	}
	/* �Ȳ���·���Ƿ���� */
	if(dir_check(parameter, fat)) {
		cons_putstr0(cons,"·�����ڡ�\n");
		if(strlen(parameter) < 256) {
			strcpy(task->dir,parameter);
			cons_printf(cons,"��ǰconsole·�����л�Ϊ��%s\n", task->dir);
		} else {
			cons_putstr0(cons,"·��������\n");
		}
	} else {
		cons_putstr0(cons,"·�������ڡ�\n");
	}
	
	return;
}
