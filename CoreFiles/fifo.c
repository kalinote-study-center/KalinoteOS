/* FIFO������ */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf){
	/* ��ʼ��FIFO������ */
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* ��������С */
	fifo->flags = 0;
	fifo->p = 0; /* ��һ������д��λ�� */
	fifo->q = 0; /* ��һ�����ݶ���λ�� */
	return;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data){
	/* ��FIFO�����ݲ����� */
	if (fifo->free == 0) {
		/* �޿��࣬��� */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	return 0;
}

int fifo8_get(struct FIFO8 *fifo){
	/* ��FIFO���һ������ */
	int data;
	if (fifo->free == fifo->size) {
		/* ���������Ϊ�գ��򷵻�-1 */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo8_status(struct FIFO8 *fifo){
	/* ���ػ��ܵ��������� */
	return fifo->size - fifo->free;
}
