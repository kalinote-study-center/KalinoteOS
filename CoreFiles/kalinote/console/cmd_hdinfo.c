/* 显示硬盘信息 */
#include "../bootpack.h"

void cmd_hdinfo(struct CONSOLE *cons, char *cmdline) {
	/* 显示硬盘信息 */
	struct IDE_HARD_DISK *hda = (struct IDE_HARD_DISK *) HDA_ADDR;
	struct IDE_HARD_DISK *hdb = (struct IDE_HARD_DISK *) HDB_ADDR;
	char hd = cmdline[7] - '0';
	if(hd == 0) {
		cons_putstr0(cons, "hda:\n");
		hd_identify(cons, hda);
	} else if (hd == 1) {
		cons_putstr0(cons, "hdb:\n");
		hd_identify(cons, hdb);
	} else {
		cons_putstr0(cons, "hd number error.\n");
	}
	return;
}
