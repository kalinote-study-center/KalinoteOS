/* �л�������Ŀ¼ */
#include "../bootpack.h"
#include <string.h>

void cmd_cd(struct CONSOLE *cons, char *parameter, int *fat) {
	if(strcmp(parameter, ".") == 0) {
		/* Ŀ¼���� */
		return;
	}
	/* �Ȳ���·���Ƿ���� */
	if(dir_check(parameter, fat)) {
		cons_putstr0(cons,"·�����ڡ�\n");
	} else {
		cons_putstr0(cons,"·�������ڡ�\n");
	}
	
	return;
}
