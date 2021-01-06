/* 多任务处理 */

#include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *task_now(void){
	/* 返回现在正在活动中的struct TASK地址 */
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

void task_add(struct TASK *task){
	/* 向struct TASKLEVEL中添加一个进程 */
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2; /* 活动中 */
	return;
}

void task_remove(struct TASK *task){
	/* 向struct TASKLEVEL中删除一个进程 */
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	/* 寻找task所在位置 */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* 在这里 */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; /* 需要移动成员，要相应地处理 */
	}
	if (tl->now >= tl->running) {
		/* 如果now的值出现异常，则进行修正 */
		tl->now = 0;
	}
	task->flags = 1; /* 休眠中 */

	/* 移动 */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
}

void task_switchsub(void){
	/* 用于进程切换时改变进程level */
	int i;
	/* 寻找最上层的level */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /* 找到了 */
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

void task_idle(void){
	/* 底层闲置任务，防止任务数为0时出现问题 */
	for (;;) {
		io_hlt();
	}
}

struct TASK *task_init(struct MEMMAN *memman){
	/* 任务初始化 */
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		taskctl->tasks0[i].tss.ldtr = (TASK_GDT0 + MAX_TASKS + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
		set_segmdesc(gdt + TASK_GDT0 + MAX_TASKS + i, 15, (int) taskctl->tasks0[i].ldt, AR_LDT);
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}
	
	task = task_alloc();
	task->flags = 2;	/* 活动中标志 */
	task->priority = 2; /* 优先级2(0.02秒进行任务切换) */
	task->level = 0;	/* 最高等级(后面还可以通过task_run重新设置level) */
	task_add(task);
	task_switchsub();	/* level设定 */
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);

	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int) &task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	task_run(idle, MAX_TASKLEVELS - 1, 1);

	return task;
}

struct TASK *task_alloc(void){
	/* 初始化任务结构 */
	int i;
	struct TASK *task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 正在使用的标志 */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* 先置0 */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.iomap = 0x40000000;		/* 固定值 */
			task->tss.ss0 = 0;
			return task;
		}
	}
	return 0; /* 全部正在使用 */
}

void task_run(struct TASK *task, int level, int priority){
	/* 运行任务 */
	if (level < 0) {
		level = task->level; /* 不改变level */
	}
	if (priority > 0) {
		/* 
		* 这里判定了传入的priority是否大于0，如果大于0则可以更改优先级
		* 如果小于等于0就不改变进程优先级
		* 同时使用task_now()也可以对于正在运行的进程仅修改优先级
		* 优先级工作原理是设定进程切换时间，以10ms为单位，如果优先级为1则是10ms进行进程切换
		* 如果是2则是20ms进行进程切换，以此类推
		*/
		task->priority = priority;  /* 设置进程优先级 */
	}

	if (task->flags == 2 && task->level != level) { /* 改变活动中的level */
		task_remove(task); /* 这里执行之后flag的值会变为1，于是下面的if语块也会被执行 */
	}
	if (task->flags != 2) {
		/* 从休眠状态唤醒的情形 */
		task->level = level;
		task_add(task);
	}

	taskctl->lv_change = 1; /* 下次任务切换检查level */
	return;
}

void task_switch(void){
	/* 切换任务 */
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		/* level改变 */
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
}

void task_sleep(struct TASK *task)
{
	struct TASK *now_task;
	if (task->flags == 2) {
		/* 如果处于活动状态 */
		now_task = task_now();
		task_remove(task); /* 执行此语句的话flags将变为1 */
		if (task == now_task) {
			/* 如果是让自己休眠则需要进行任务切换 */
			task_switchsub();
			now_task = task_now(); /* 在设定后获取当前任务的值 */
			farjmp(0, now_task->sel);
		}
	}
	return;
}
