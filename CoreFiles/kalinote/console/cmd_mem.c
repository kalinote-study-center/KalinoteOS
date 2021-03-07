/* 查询内存指令 */
#include "../bootpack.h"
#include <stdio.h>

void cmd_mem(struct CONSOLE *cons, unsigned int memtotal){
	/* 查询系统内存使用情况 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	char s[80];
	debug_print("command>run mem at %d\n",cons);
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(s, "总计     %dMB\n可用  %dKB\n系统占用 %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024, SYS_MEMORY / 1024);
	} else {
		/* 其他模式 */
		sprintf(s, "total    %dMB\nfree  %dKB\nsystem   %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024, SYS_MEMORY / 1024);
	}
	cons_putstr0(cons, s);
	return;
}
