/* ����ṹ */
#include <bootpack.h>

void list_init(struct list* list) {
	/* ��ʼ��˫���� */
	list->head.prev = NULL;
	list->head.next = &list->tail;
	list->tail.prev = &list->head;
	list->tail.next = NULL;
}

void list_insert_before(struct list_elem* before, struct list_elem* elem) { 
	/* ������Ԫ��elem���뵽before֮ǰ */
}

void list_push(struct list* plist, struct list_elem* elem) {
	/* ���Ԫ�ص��б����,����ջpush���� */
}

void list_append(struct list* plist, struct list_elem* elem) {
	/* ׷��Ԫ�ص������β,���ƶ��е��Ƚ��ȳ����� */
}

void list_remove(struct list_elem* pelem) {
	/* ʹԪ��pelem�������� */
}

struct list_elem* list_pop(struct list* plist) {
	/* �������һ��Ԫ�ص���������,����ջ��pop���� */
	return 0;
}

int elem_find(struct list* plist, struct list_elem* obj_elem) {
	/* �������в���Ԫ��obj_elem,�ɹ�ʱ��������λ��,ʧ��ʱ����-1 */
	return -1;
}

int list_empty(struct list* plist) {
	/* �ж������Ƿ�Ϊ��,��ʱ����1,���򷵻�0 */
	return (plist->head.next == &plist->tail ? 1 : 0);
}
