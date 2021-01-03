/* FIFO缓冲区 */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task){
	/* 初始化FIFO缓冲区 */
	fifo->size = size; /* 缓冲区总大小 */
	fifo->buf = buf; /* 缓冲区地址 */
	fifo->free = size; /* 缓冲区(空闲部分)大小 */
	fifo->flags = 0; /* 溢出标志 */
	fifo->p = 0; /* 下一个数据写入位置 */
	fifo->q = 0; /* 下一个数据读出位置 */
	fifo->task = task; /* 有数据写入时需要唤醒的任务 */
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data){
	/* 向FIFO传数据并保存 */
	if (fifo->free == 0) {
		/* 无空余，溢出*/
		fifo->flags |= FLAGS_OVERRUN; /* flags置1 */
		return -1;
	}
	fifo->buf[fifo->p] = data; /* 执行写入(fifo->p是写入位置，详见bootpack.h的fifo结构体定义) */
	fifo->p++; /* 更新下一个数据写入的位置 */
	if (fifo->p == fifo->size) {
		/* 下一个写入位置超过缓冲区大小，则从0重新开始写 */
		fifo->p = 0;
	}
	fifo->free--; /* 可用空间-1 */
	if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* 如果任务处于休眠状态 */
			task_run(fifo->task, -1, 0); /* 将任务唤醒 */
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32 *fifo){
	/* 从FIFO获得一个数据 */
	int data;
	if (fifo->free == fifo->size) {
		/* 如果缓冲区为空，则返回-1 */
		return -1;
	}
	data = fifo->buf[fifo->q]; /* 读出(fifo->p是读出位置) */
	fifo->q++; /* 更新下一次读出位置 */
	if (fifo->q == fifo->size) {
		/* 下一个读出位置超过缓冲区大小，则从0重新开始读 */
		fifo->q = 0;
	}
	fifo->free++; /* 读出后可用空间+1 */
	return data; /* 返回读出的数据 */
}

int fifo32_status(struct FIFO32 *fifo){
	/* 返回积攒的数据数量 */
	return fifo->size - fifo->free;
}
