/* ����ṹ */
#include "bootpack.h"

void list_init (struct list* list) {
	/* ��ʼ��˫������list */
	list->head.prev = NULL;					// ((void*)0)����bootpack.h
	list->head.next = &list->tail;
	list->tail.prev = &list->head;
	list->tail.next = NULL;
}
