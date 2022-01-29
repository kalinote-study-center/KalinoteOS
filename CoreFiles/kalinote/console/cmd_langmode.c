/* 切换系统语言模式 */
#include <bootpack.h>

void cmd_langmode(struct CONSOLE *cons, char *cmdline){
	/* 切换系统语言模式 */
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 1) {
		if (mode == 0) {
			task->langmode = mode;
			cons_putstr0(cons,"Switch to English ASCII mode\n");
		} else if (mode == 1) {
			task->langmode = mode;
			cons_putstr0(cons,"切换到中文模式\n");
		}
	} else {
		cons_putstr0(cons, "mode number error.\n");
	}
	cons_newline(cons);
	return;
}
