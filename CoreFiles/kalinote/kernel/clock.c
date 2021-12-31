/* ϵͳʱ������ */
#include "../bootpack.h"

void clock_task(struct SHEET *sht) {
	/* ʱ������������(������������) */
	// struct TASK *task = task_now();
	// struct TIMER *timer;
	
	// timer = timer_alloc();
	// timer_init(timer, &task->fifo, 1);
	// timer_settime(timer, 1);
	
	for(;;) {
		io_hlt();
	}
}

void clock_taskinit(struct TASK *task, struct SHEET *sht) {
	/* ��ʼ��ʱ������ */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int *clock_fifo = (int *)memman_alloc_4k(memman, 32 * 4);
	
	task->cons_stack = memman_alloc_4k(memman, 65536);
	task->tss.eip = (int) &clock_task;
	task->tss.esp = task->cons_stack + 65536 - 8;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	
	*((int *) (task->tss.esp + 4)) = (int) sht;
	
	fifo32_init(&task->fifo, 32, clock_fifo, task);
	
	task_run(task, 1, 1);		/* TODO����������������console��û�취��ʾ�����ˣ���֪��Ϊʲô */
	task_sleep(task);
	return;
}


