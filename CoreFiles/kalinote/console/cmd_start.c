/* 退出程序 */
#include "../bootpack.h"

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal){
	/* 在新的命令窗口启动一个应用程序 */
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
	struct SHEET *sht = open_console(shtctl, memtotal, 0);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* 将命令行输入的字符串逐字复制到新的命令行窗口中 */
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}
