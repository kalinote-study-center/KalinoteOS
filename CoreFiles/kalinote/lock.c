/* ������ */
#include "bootpack.h"

void sema_init(struct semaphore* psema, unsigned char value) {
	/* ��ʼ���ź��� */
	psema->value = value;						// Ϊ�ź�������ֵ
	list_init(&psema->waiters);					//��ʼ���ź����ĵȴ�����
}

void lock_init(struct lock* plock) {
	/* ��ʼ����plock */
	plock->holder = NULL;
	plock->holder_repeat_nr = 0;
	sema_init(&plock->semaphore, 1);			// �ź�����ֵΪ1
}

void lock_acquire(struct lock* plock) {
	/* ��ȡplock�� */
	/* ����KalinoteOS��ʱ��û�����߳�״̬(���У��������ߵ�)��������ʱ���� */
}


void sema_down(struct semaphore* psema) {
	/* �ź���down���� */
	/* ��ʱû�� */
}


void sema_up(struct semaphore* psema) {
	/* �ź�����up���� */
	/* ��ʱû�� */
}

void lock_release(struct lock* plock) {
	/* �ͷ���plock */
	if (plock->holder_repeat_nr > 1) {
		plock->holder_repeat_nr--;
		return;
	}
	plock->holder = NULL;	   // �����ĳ������ÿշ���V����֮ǰ
	plock->holder_repeat_nr = 0;
	sema_up(&plock->semaphore);	   // �ź�����V����,Ҳ��ԭ�Ӳ���
}
