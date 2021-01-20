/* �л�ϵͳģʽ */
#include "../bootpack.h"
#include <stdio.h>

void cmd_sysmode(struct CONSOLE *cons, char *cmdline) {
	/* �л�ϵͳģʽ */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) 0x10000);
	struct TASK *task = task_now();
	char mode = cmdline[8] - '0';
	char s[64];
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
	} else if (mode == 1) {
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
	} else {
		sysinfo->sysmode = mode;
		if (task->langmode == 1) {
			/* ����ģʽ */
			sprintf(s,"��ϵͳģʽδ���壬ģʽ�룺%d\n",sysinfo->sysmode);
			cons_putstr0(cons,s);
		} else {
			/* Ӣ�Ļ�����ģʽ */
			sprintf(s,"This system mode is not defined,mode code:%d\n",sysinfo->sysmode);
			cons_putstr0(cons,s);
		}
	}
	return;
}
