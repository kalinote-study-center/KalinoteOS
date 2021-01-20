/* �л�ϵͳ����ģʽ */
#include "../bootpack.h"

void cmd_langmode(struct CONSOLE *cons, char *cmdline){
	/* �л�ϵͳ����ģʽ */
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 3) {
		if (mode == 0) {
			task->langmode = mode;
			cons_putstr0(cons,"Switch to English ASCII mode\n");
		} else if (mode == 1) {
			task->langmode = mode;
			cons_putstr0(cons,"�л�������ģʽ\n");
		} else if (mode == 2) {
			cons_putstr0(cons,"There is a fatal error in the Japanese display of the system, please wait for the repair\n");
		} else if (mode == 3) {
			cons_putstr0(cons,"There is a fatal error in the Japanese display of the system, please wait for the repair\n");
		}
	} else {
		cons_putstr0(cons, "mode number error.\n");
	}
	cons_newline(cons);
	return;
}
