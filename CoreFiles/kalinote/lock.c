/* 进程锁 */
#include "bootpack.h"

void sema_init(struct semaphore* psema, unsigned char value) {
	/* 初始化信号量 */
	psema->value = value;						// 为信号量赋初值
	list_init(&psema->waiters);					//初始化信号量的等待队列
}

void lock_init(struct lock* plock) {
	/* 初始化锁plock */
	plock->holder = NULL;
	plock->holder_repeat_nr = 0;
	sema_init(&plock->semaphore, 1);			// 信号量初值为1
}
