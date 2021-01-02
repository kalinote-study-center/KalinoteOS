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
