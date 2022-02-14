/* ≤‚ ‘ π”√ */
#include <bootpack.h>

void cmd_testfunc(struct CONSOLE *cons, int *fat) {
	// struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	// struct FILEINFO *root_finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	// struct FILEINFO *file = file_search("aobench.kal", root_finfo, 224);
	// cons_printf(cons, "%d\n",file_delete(fat, file));
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	/* ÕºœÒ÷°ª∫¥Êµÿ÷∑ */
	debug_print("VREAM = %08x",binfo->vram);
	
	return;
}
