/* fork相关功能 */
#include <bootpack.h>

int copy_mem(int task_num, struct TASK *task) {
	/* (TODO：)这个函数用于设置新任务的代码、数据段和基址、限长并复制内存 */
	
	return 0;
}

int copy_process(unsigned int fsize, unsigned int segsiz) {
	/*
	* 对于该函数的说明：
	* 该函数原本仿照linux中的copy_process而设计，但是KalinoteOS中的任务数据结构经过初始化
	* 且由task_alloc进行分配，所以不需要复制父进程，可以直接使用task_alloc进行分配，继承父进程pid即可
	* 在内存复制方面，linux中使用的是页内存管理，而KalinteOS从最初设计就使用的是段内存管理
	* 现在再进行改造有点麻烦，所以只能先这样。
	*/
	// struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *now_task = task_now();		/* 当前task(执行进程复制的task) */
	struct TASK *task = task_alloc();
	// int alloc_pid = task->task_id_num.pid;

	
	if(!task)
		return -1;				/* task分配失败 */
	
	/* (已废弃)复制进程 */
	// *task = *now_task;		/* 复制task数据 */
	/* 对复制得到的task的数据结构进行修改，作为新的task数据结构 */
	// task->flags = TASK_UNINTERRUPTIBLE;							/* (已设置)设置不可中断，防止系统执行 */
	// task->task_id_num.pid = alloc_pid;							/* (已设置)将pid设置回来 */
	task->task_id_num.father = now_task->task_id_num.pid;			/* 设置父进程号 */
	// task->task_id_num.father = 0;								/* (已设置)父进程号(pid)默认0 */
	// task->signal = 0;											/* (已设置)信号位图置0 */
	// task->task_id_num.leader = 0;								/* (已设置)不继承task领导权 */
	// task->task_time.utime = 0;									/* (已设置)运行时间全部置0 */
	// task->task_time.stime = 0;	                                // (已设置)
	// task->task_time.cutime = 0;	                                // (已设置)
	// task->task_time.cstime = 0;	                                // (已设置)
	// task->task_time.start_time = timerctl.count;					/* (已设置)启动时间是系统当前tik */
	/* 这里修改TSS(TODO) */
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	// set_segmdesc(task->ldt + 0, fsize	- 1, (int) p, AR_CODE32_ER_R3);			/* R3特权级，应用程序使用 */	/* 可读可执行不可写 */
	// set_segmdesc(task->ldt + 1, segsiz	- 1, (int) q, AR_DATA32_RW_R3);			/* 可读写不可执行 */
	// task->tss.iomap = 0x40000000;												/* (已设置)iomap */
	/* (TODO-必要性评估)协处理器状态保存 */
	
	task_run(task, now_task->level, now_task->priority);			/* 运行任务 */
	return 0;
}
