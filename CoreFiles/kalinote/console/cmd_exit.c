/* 退出程序 */
#include "../bootpack.h"

void cmd_exit(struct CONSOLE *cons, int *fat){
	/* 退出程序 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) FIFO_ADDR);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	// taskbar_removewin(((struct WINDOW *)(cons->sht->win))->tskwinbtn);
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0) {
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);	/* 768～1023 */
	} else {
		/* 命令窗口已经被关闭 */
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);	/* 1024～2023 */
	}
	io_sti();
	for (;;) {
		task_sleep(task);
	}
}
