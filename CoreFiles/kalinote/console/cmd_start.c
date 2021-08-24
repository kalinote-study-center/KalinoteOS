/* �˳����� */
#include "../bootpack.h"

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal){
	/* ���µ����������һ��Ӧ�ó��� */
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
	struct SHEET *sht = open_console(shtctl, memtotal, 0);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* ��������������ַ������ָ��Ƶ��µ������д����� */
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}
