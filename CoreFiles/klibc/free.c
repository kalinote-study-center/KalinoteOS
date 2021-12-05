/* C语言标准函数 释放内存 */
#include <stdlib.h>
#include <apilib.h>

void free(void *ap){
	char *q = ap;
	int size;
	if(q != 0){
		q -= 16;
		size = *((int *) q);		/* 从内存空间的前16字节取到内存空间大小 */
		api_free(q,size + 16);
	}
	return;
}
