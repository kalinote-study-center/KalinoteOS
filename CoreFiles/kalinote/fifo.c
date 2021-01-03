/* FIFO������ */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task){
	/* ��ʼ��FIFO������ */
	fifo->size = size; /* �������ܴ�С */
	fifo->buf = buf; /* ��������ַ */
	fifo->free = size; /* ������(���в���)��С */
	fifo->flags = 0; /* �����־ */
	fifo->p = 0; /* ��һ������д��λ�� */
	fifo->q = 0; /* ��һ�����ݶ���λ�� */
	fifo->task = task; /* ������д��ʱ��Ҫ���ѵ����� */
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data){
	/* ��FIFO�����ݲ����� */
	if (fifo->free == 0) {
		/* �޿��࣬���*/
		fifo->flags |= FLAGS_OVERRUN; /* flags��1 */
		return -1;
	}
	fifo->buf[fifo->p] = data; /* ִ��д��(fifo->p��д��λ�ã����bootpack.h��fifo�ṹ�嶨��) */
	fifo->p++; /* ������һ������д���λ�� */
	if (fifo->p == fifo->size) {
		/* ��һ��д��λ�ó�����������С�����0���¿�ʼд */
		fifo->p = 0;
	}
	fifo->free--; /* ���ÿռ�-1 */
	if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* �������������״̬ */
			task_run(fifo->task, -1, 0); /* �������� */
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32 *fifo){
	/* ��FIFO���һ������ */
	int data;
	if (fifo->free == fifo->size) {
		/* ���������Ϊ�գ��򷵻�-1 */
		return -1;
	}
	data = fifo->buf[fifo->q]; /* ����(fifo->p�Ƕ���λ��) */
	fifo->q++; /* ������һ�ζ���λ�� */
	if (fifo->q == fifo->size) {
		/* ��һ������λ�ó�����������С�����0���¿�ʼ�� */
		fifo->q = 0;
	}
	fifo->free++; /* ��������ÿռ�+1 */
	return data; /* ���ض��������� */
}

int fifo32_status(struct FIFO32 *fifo){
	/* ���ػ��ܵ��������� */
	return fifo->size - fifo->free;
}
