/* 定时器 */

#include "../bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

void init_pit(void){
	/* 初始化可编程间隔化定时器(PIT) */
	int i;
	struct TIMER *t;
	/* 改变PIT中断周期 */
	io_out8(PIT_CTRL, 0x34); // 固定值
	io_out8(PIT_CNT0, 0x9c); // 中断周期低8位
	io_out8(PIT_CNT0, 0x2e); // 中断周期高8位
	/* 上面发送的终端周期为0x2e9c，换算成十进制是11932，设定这个值的中断频率为100Hz，即每10ms发生一次中断，每秒产生100次中断 */
	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; /* 未使用 */
	}
	/* 哨兵程序，一直守在最后一个的timer */
	t = timer_alloc(); /* 取得一个 */
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0; /* 末尾 */
	timerctl.t0 = t; /* 因为现在只有哨兵，所以他就在最前面 */
	timerctl.next = 0xffffffff; /* 因为只有哨兵，所以下一个超时时刻就是哨兵的时刻 */
	/*
	* "哨兵"是在整个timer线性表中的最后一个，且永远不会超时
	* 使用哨兵程序防止了"处于运行中的定时器只有一个"和"定时器插入到最后"这两种情况的发生
	* 减少了定时器程序的不确定情况
	*/
	return;
}

struct TIMER *timer_alloc(void){
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			timerctl.timers0[i].flags2 = 0;
			return &timerctl.timers0[i];
		}
	}
	return 0; /* 没找到 */
}

void timer_free(struct TIMER *timer){
	timer->flags = 0; /* 未使用 */
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data){
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout){
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	t = timerctl.t0;
	if (timer->timeout <= t->timeout) {
		/* 插入最前面的情况 */
		timerctl.t0 = timer;
		timer->next = t; /* 下面设定t */
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	/* 搜寻插入位置 */
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			/* 插入s和t之间的情况 */
			s->next = timer; /* s的下一个是timer */
			timer->next = t; /* timer的下一个是t */
			io_store_eflags(e);
			return;
		}
	}
}

void inthandler20(int *esp){
	/* 时钟中断程序 */
	struct TIMER *timer;
	char ts = 0;
	io_out8(PIC0_OCW2, 0x60);	/* 把IRQ-00信号接收完了的信息通知给中断(PIC)，0+0x60号端口 */
	timerctl.count++;			//定时器计数
	if (timerctl.next > timerctl.count) {
		return; /* 还不到下一个时刻，所以结束 */
	}
	timer = timerctl.t0;	// 首先把最前面的地址赋给timer
	for (;;) {
		/* 因为timers的定时器都处于运行状态，所以不确认flags */
		if (timer->timeout > timerctl.count) {
			break;
		}
		/* 超时 */
		timer->flags = TIMER_FLAGS_ALLOC;
		if (timer != task_timer) {
			fifo32_put(timer->fifo, timer->data);
		} else {
			ts = 1; /* task_timer超时 */
		}
		timer = timer->next; /* 将下一个计时器的地址赋给timer */
	}
	timerctl.t0 = timer;
	timerctl.next = timer->timeout;
	if (ts != 0) {
		/* 切换任务在所有中断程序执行完成以后，避免出现错误 */
		task_switch();
	}
	return;
}

int timer_cancel(struct TIMER *timer)
{
	int e;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* 在设置过程中禁止改变定时器状态 */
	if (timer->flags == TIMER_FLAGS_USING) {	/* 是否需要取消 */
		if (timer == timerctl.t0) {
			/* 第一个定时器的取消处理 */
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		} else {
			/* 非第一个定时器的取消处理 */
			/* 找到timer前一个定时器 */
			t = timerctl.t0;
			for (;;) {
				if (t->next == timer) {
					break;
				}
				t = t->next;
			}
			t->next = timer->next; /* 将之前"timer的下一个"指向"timer的下一个" */
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		io_store_eflags(e);
		return 1;	/* 取消处理成功 */
	}
	io_store_eflags(e);
	return 0; /* 不需要取消处理 */
}

void timer_cancelall(struct FIFO32 *fifo)
{
	int e, i;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* 在设置过程中禁止改变定时器状态 */
	for (i = 0; i < MAX_TIMER; i++) {
		t = &timerctl.timers0[i];
		if (t->flags != 0 && t->flags2 != 0 && t->fifo == fifo) {
			timer_cancel(t);
			timer_free(t);
		}
	}
	io_store_eflags(e);
	return;
}
