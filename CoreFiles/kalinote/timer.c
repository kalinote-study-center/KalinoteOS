/* ��ʱ�� */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC		1	/* ������״̬ */
#define TIMER_FLAGS_USING		2	/* ��ʱ�������� */

void init_pit(void){
	/* ��ʼ���ɱ�̼������ʱ��(PIT) */
	int i;
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; /* δʹ�� */
	}
	t = timer_alloc(); /* ȡ��һ�� */
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0; /* ĩβ */
	timerctl.t0 = t; /* ��Ϊ����ֻ���ڱ���������������ǰ�� */
	timerctl.next = 0xffffffff; /* ��Ϊֻ���ڱ���������һ����ʱʱ�̾����ڱ���ʱ�� */
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
	return 0; /* û�ҵ� */
}

void timer_free(struct TIMER *timer){
	timer->flags = 0; /* δʹ�� */
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
		/* ������ǰ������ */
		timerctl.t0 = timer;
		timer->next = t; /* �����趨t */
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	/* ��Ѱ����λ�� */
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			/* ����s��t֮������ */
			s->next = timer; /* s����һ����timer */
			timer->next = t; /* timer����һ����t */
			io_store_eflags(e);
			return;
		}
	}
}

void inthandler20(int *esp){
	struct TIMER *timer;
	char ts = 0;
	io_out8(PIC0_OCW2, 0x60);	/* ��IRQ-00�źŽ������˵���Ϣ֪ͨ���ж�(PIC) */
	timerctl.count++;			//��ʱ������
	if (timerctl.next > timerctl.count) {
		return; /* ��������һ��ʱ�̣����Խ��� */
	}
	timer = timerctl.t0;	// ���Ȱ���ǰ��ĵ�ַ����timer
	for (;;) {
		/* ��Ϊtimers�Ķ�ʱ������������״̬�����Բ�ȷ��flags */
		if (timer->timeout > timerctl.count) {
			break;
		}
		/* ��ʱ */
		timer->flags = TIMER_FLAGS_ALLOC;
		if (timer != task_timer) {
			fifo32_put(timer->fifo, timer->data);
		} else {
			ts = 1; /* task_timer��ʱ */
		}
		timer = timer->next; /* ����һ����ʱ���ĵ�ַ����timer */
	}
	timerctl.t0 = timer;
	timerctl.next = timer->timeout;
	if (ts != 0) {
		task_switch();
	}
	return;
}

int timer_cancel(struct TIMER *timer)
{
	int e;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* �����ù����н�ֹ�ı䶨ʱ��״̬ */
	if (timer->flags == TIMER_FLAGS_USING) {	/* �Ƿ���Ҫȡ�� */
		if (timer == timerctl.t0) {
			/* ��һ����ʱ����ȡ������ */
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		} else {
			/* �ǵ�һ����ʱ����ȡ������ */
			/* �ҵ�timerǰһ����ʱ�� */
			t = timerctl.t0;
			for (;;) {
				if (t->next == timer) {
					break;
				}
				t = t->next;
			}
			t->next = timer->next; /* ��֮ǰ"timer����һ��"ָ��"timer����һ��" */
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		io_store_eflags(e);
		return 1;	/* ȡ�������ɹ� */
	}
	io_store_eflags(e);
	return 0; /* ����Ҫȡ������ */
}

void timer_cancelall(struct FIFO32 *fifo)
{
	int e, i;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* �����ù����н�ֹ�ı䶨ʱ��״̬ */
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