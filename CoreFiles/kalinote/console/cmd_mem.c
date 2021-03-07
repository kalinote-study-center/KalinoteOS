/* ��ѯ�ڴ�ָ�� */
#include "../bootpack.h"
#include <stdio.h>

void cmd_mem(struct CONSOLE *cons, unsigned int memtotal){
	/* ��ѯϵͳ�ڴ�ʹ����� */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	char s[80];
	debug_print("command>run mem at %d\n",cons);
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(s, "�ܼ�     %dMB\n����  %dKB\nϵͳռ�� %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024, SYS_MEMORY / 1024);
	} else {
		/* ����ģʽ */
		sprintf(s, "total    %dMB\nfree  %dKB\nsystem   %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024, SYS_MEMORY / 1024);
	}
	cons_putstr0(cons, s);
	return;
}
