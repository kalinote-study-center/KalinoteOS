/* C语言标准函数库 stdlib.h realloc */
#include <stdlib.h>
#include <apilib.h>

/* 重新分配一段内存，暂时不做调整，只做重新申请 */

void *realloc(void *ptr, size_t size) {
	/* 注意：该函数可用性未知 */
	/* 注意：使用该函数重新分配内存长度，如果新内存长度小于原内存长度，可能造成数据丢失 */
	unsigned int *np = malloc(size);		/* 新内存空间 */
	char *q = ptr;		/* 原内存空间指针 */
	int s;		/* 原内存空间长度 */
	
	if(size == 0) {
		/* 如果新的长度为0，则直接释放内存 */
		free(ptr);
		return 0;
	}
	
	/* 获取原内存空间长度 */
	q-=16;
	s = *((int *) q);
	/* 复制内存内容 */
	memcpy(ptr, np, ((s<size)?s:size));
	/* 释放原内存 */
	free(ptr);
	return np;
}
