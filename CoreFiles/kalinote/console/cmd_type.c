/* 输出文件内容 */
#include "../bootpack.h"

void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline){
	/* 输出文件内容 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	char *p;
	int i;
	debug_print("command>run type at %d\n",cons);
	if (finfo != 0) {
		/* 找到文件的情况 */
		p = (char *) memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));	/* 磁盘映像中的地址 = clustno * 512 +0x003e00 */
		for (i = 0; i < finfo->size; i++) {
			cons_putchar(cons, p[i], 1);
		}
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		/* 没有找到文件的情况 */
		putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL_WHITE, COL_BLACK, "File not found.", 25);
		cons_newline(cons);
	}
	cons_newline(cons);
	return;
}
