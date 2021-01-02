/* 链表结构 */
#include "bootpack.h"

void list_init (struct list* list) {
	/* 初始化双向链表list */
	list->head.prev = NULL;					// ((void*)0)，见bootpack.h
	list->head.next = &list->tail;
	list->tail.prev = &list->head;
	list->tail.next = NULL;
}
