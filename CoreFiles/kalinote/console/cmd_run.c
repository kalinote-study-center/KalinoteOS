/* ����һ��Ӧ�ó����Ҳ�������� */
#include "../bootpack.h"

void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal){
	/* ����һ��Ӧ�ó����Ҳ�������� */
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	/* ��������������ַ������ָ��Ƶ��µ������д����� */
	for (i = 4; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}
