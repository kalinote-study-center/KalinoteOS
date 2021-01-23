/* 测试系统功能专用指令 */
#include "../bootpack.h"
#include <stdio.h>

void cmd_systest(struct CONSOLE *cons) {
	/* 测试系统功能专用指令 */
	char buf[1025],c[50];
	hd_read_sectors(10, buf, 2, 1);
	buf[1025] = 0;
	sprintf(c,"%s\n",*buf);
	cons_putstr0(cons,c);
	return;
}
