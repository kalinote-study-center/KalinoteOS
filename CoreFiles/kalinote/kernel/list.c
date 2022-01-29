/* 链表结构 */
#include <bootpack.h>

void list_init(struct list* list) {
	/* 初始化双链表 */
	list->head.prev = NULL;
	list->head.next = &list->tail;
	list->tail.prev = &list->head;
	list->tail.next = NULL;
}

void list_insert_before(struct list_elem* before, struct list_elem* elem) { 
	/* 将链表元素elem插入到before之前 */
}

void list_push(struct list* plist, struct list_elem* elem) {
	/* 添加元素到列表队首,类似栈push操作 */
}

void list_append(struct list* plist, struct list_elem* elem) {
	/* 追加元素到链表队尾,类似队列的先进先出操作 */
}

void list_remove(struct list_elem* pelem) {
	/* 使元素pelem脱离链表 */
}

struct list_elem* list_pop(struct list* plist) {
	/* 将链表第一个元素弹出并返回,类似栈的pop操作 */
	return 0;
}

int elem_find(struct list* plist, struct list_elem* obj_elem) {
	/* 从链表中查找元素obj_elem,成功时返回所在位置,失败时返回-1 */
	return -1;
}

int list_empty(struct list* plist) {
	/* 判断链表是否为空,空时返回1,否则返回0 */
	return (plist->head.next == &plist->tail ? 1 : 0);
}
