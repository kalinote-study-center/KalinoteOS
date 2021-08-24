/* 启动一个应用程序并且不打开命令窗口 */
#include "../bootpack.h"

void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal){
	/* 启动一个应用程序并且不打开命令窗口 */
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	/* 将命令行输入的字符串逐字复制到新的命令行窗口中 */
	for (i = 4; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}
