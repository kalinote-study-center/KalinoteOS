/* ��ѯϵͳӲ������ */
#include "../bootpack.h"
#include <stdio.h>

void cmd_hdnum(struct CONSOLE *cons) {
	/* ��ѯϵͳӲ������ */
	struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	char s[50];
	sprintf(s,"hard disk number:%d\n",hd->disk_num);
	cons_putstr0(cons,s);
	return;
}
