/* �л�������Ŀ¼ */
#include "../bootpack.h"
#include <string.h>

void cmd_cd(struct CONSOLE *cons, char *parameter, int *fat) {
	struct TASK *task = task_now();
	char apath[350];
	int i;
	if(strcmp(parameter, ".") == 0) {
		/* Ŀ¼���� */
		return;
	}
	
	/* ���������'/'��ͷ���������·�� */
	if(parameter[0]!='/'){
		/* ���·��������ʱ���ϵ�ǰ·�� */
		strcpy(apath, task->dir);
		strcat(apath, parameter);
	} else {
		/* ����·�� */
		strcpy(apath, parameter);
	}
	
	/* �Ȳ���·���Ƿ���� */
	if(dir_check(apath, fat)) {
		cons_putstr0(cons,"·�����ڡ�\n");
		if(strlen(apath) < 255) {
			i = strlen(apath);
			if(apath[i]!='/'){
				/* ���·��������'/'��β�����ڽ�β�����һ�� */
				apath[i] = '/';
				apath[i+1] = 0;
			}
			strcpy(task->dir,apath);
			cons_printf(cons,"��ǰconsole·�����л�Ϊ��%s\n", task->dir);
		} else {
			cons_putstr0(cons,"·��������\n");
		}
	} else {
		cons_putstr0(cons,"·�������ڡ�\n");
	}
	
	return;
}
