/* 切换系统模式 */
#include "../bootpack.h"
#include <stdio.h>

void cmd_sysmode(struct CONSOLE *cons, char *cmdline) {
	/* 切换系统模式 */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) 0x10000);
	struct TASK *task = task_now();
	char mode = cmdline[8] - '0';
	char s[64];
	struct SHEET *sht_debug_cons = (struct SHEET *) *((int *) DEBUG_ADDR);
	
	if(sysinfo->sysmode == mode) {
		/* 不变 */
		cons_putstr0(cons,"System mode has no change\n");
		return;
	}
	if (mode == 0) {
		sysinfo->sysmode = mode;
		/* 隐藏DEBUG console */
		sheet_updown(sht_debug_cons, -1);
		if (task->langmode == 1) {
			/* 中文模式 */
			cons_putstr0(cons,"系统切换到正常模式\n");
		} else {
			/* 英文或日文模式 */
			cons_putstr0(cons,"System switch to normal mode\n");
		}
		sysinfo->sysmode = mode;
	} else if (mode == 1) {
		sysinfo->sysmode = mode;
		/* 显示DEBUG console */
		sheet_updown(sht_debug_cons, sht_debug_cons->ctl->top);
		cons_putstr0(sht_debug_cons->task->cons,"DEBUG MODE\n");
		if (task->langmode == 1) {
			/* 中文模式 */
			cons_putstr0(cons,"系统切换到调试模式\n");
		} else {
			/* 英文或日文模式 */
			cons_putstr0(cons,"System switch to debug mode\n");
		}
	} else {
		sysinfo->sysmode = mode;
		if (task->langmode == 1) {
			/* 中文模式 */
			sprintf(s,"该系统模式未定义，模式码：%d\n",sysinfo->sysmode);
			cons_putstr0(cons,s);
		} else {
			/* 英文或日文模式 */
			sprintf(s,"This system mode is not defined,mode code:%d\n",sysinfo->sysmode);
			cons_putstr0(cons,s);
		}
	}
	return;
}
