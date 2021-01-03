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

void lock_acquire(struct lock* plock) {
	/* 获取plock锁 */
	/* 由于KalinoteOS暂时还没有做线程状态(运行，挂起，休眠等)，所以暂时不做 */
}


void sema_down(struct semaphore* psema) {
	/* 信号量down操作 */
	/* 暂时没有 */
}


void sema_up(struct semaphore* psema) {
	/* 信号量的up操作 */
	/* 暂时没有 */
}

void lock_release(struct lock* plock) {
	/* 释放锁plock */
	if (plock->holder_repeat_nr > 1) {
		plock->holder_repeat_nr--;
		return;
	}
	plock->holder = NULL;	   // 把锁的持有者置空放在V操作之前
	plock->holder_repeat_nr = 0;
	sema_up(&plock->semaphore);	   // 信号量的V操作,也是原子操作
}
