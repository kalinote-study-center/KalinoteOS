/* �л�ϵͳģʽ */
#include "../bootpack.h"
#include <stdio.h>

void cmd_sysmode(struct CONSOLE *cons, char *cmdline) {
	/* �л�ϵͳģʽ */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	struct TASK *task = task_now();
	char mode = cmdline[8] - '0';
	struct SHEET *sht_debug_cons = (struct SHEET *) *((int *) DEBUG_ADDR);
	if(sysinfo->sysmode == mode) {
		/* ���� */
		cons_putstr0(cons,"System mode has no change\n");
		return;
	}
	if (mode == 0) {
		sysinfo->sysmode = mode;
		/* ����DEBUG console */
		sheet_updown(sht_debug_cons, -1);
		if (task->langmode == 1) {
			/* ����ģʽ */
			cons_putstr0(cons,"ϵͳ�л�������ģʽ\n");
		} else {
			/* Ӣ�Ļ�����ģʽ */
			cons_putstr0(cons,"System switch to normal mode\n");
		}
		sysinfo->sysmode = mode;
	} else {
		sysinfo->sysmode = mode;
		/* ��ʾDEBUG console */
		sheet_updown(sht_debug_cons, sht_debug_cons->ctl->top);
		cons_putstr0(sht_debug_cons->task->cons,"DEBUG MODE\n");
		if (task->langmode == 1) {
			/* ����ģʽ */
			cons_putstr0(cons,"ϵͳ�л�������ģʽ\n");
		} else {
			/* Ӣ�Ļ�����ģʽ */
			cons_putstr0(cons,"System switch to debug mode\n");
		}
	}
	return;
}
