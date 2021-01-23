/* 查询系统硬盘数量 */
#include "../bootpack.h"
#include <stdio.h>

void cmd_hdnum(struct CONSOLE *cons) {
	/* 查询系统硬盘数量 */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	char s[50];
	// sprintf(s,"hard disk number:%d\n",hd->disk_num);
	sprintf(s,"hard disk number:%d\n",(*((unsigned char *) IDE_HD_NUM_ADDR)));
	cons_putstr0(cons,s);
	return;
}
