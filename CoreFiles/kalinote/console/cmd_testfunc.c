/* 测试使用 */
#include "../bootpack.h"

void cmd_testfunc(struct CONSOLE *cons) {
	struct TASK *task = task_now();
	struct FIFO32 read_fifo;
	int fifo_buf[128];
	fifo32_init(&read_fifo, 128, fifo_buf, task);
	fifo32_put(&task->fifo, (1 << 12 | 0));			// [1 << 12 | 扇区号]
	fifo32_put(&task->fifo, 1);						// 扇区数
	fifo32_put(&task->fifo, (int)&read_fifo);		// fifo
	fdc_task();
	cons_printf(cons, "code:%d", fifo32_get(&read_fifo));
	return;
}
