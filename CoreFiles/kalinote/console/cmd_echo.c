/* 系统输出(等待完善) */
#include "../bootpack.h"
#include <stdio.h>

void cmd_echo(struct CONSOLE *cons, char *cmdline) {
	/* 系统输出 */
	char *s = (char *)0x2f000;
	debug_print("command>run echo at %d\n",cons);
	sprintf(s,"%s\n", cmdline + 5);
	cons_putstr0(cons,s);
	return;
}
