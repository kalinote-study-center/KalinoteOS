/* ϵͳ��� */
#include "../bootpack.h"
#include <stdio.h>

void cmd_echo(struct CONSOLE *cons, char *cmdline) {
	/* ϵͳ��� */
	char *s = (char *)0x2f000;
	sprintf(s,"%s\n", cmdline + 5);
	cons_putstr0(cons,s);
	return;
}
