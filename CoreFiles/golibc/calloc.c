/* C语言标准函数库 stdlib.h calloc */
#include <stdlib.h>
#include "../apilib.h"

/* 分配一段内存，并将内容置0 */

void *calloc(size_t nitems, size_t size) {
	/* 注意：该函数可用性未知 */
	size_t nbytes = nitems*size;
	/* 分配内存 */
	int *p = api_malloc(nbytes + 16);
	int *i;
	if(p != 0){
		/* 如果内存分配成功 */
		/* 向这个地址写入了请求的内存大小，方便在free时使用 */
		*((int *) p) = nbytes;
		p+=16;
		
		/* 置0 */
		for(i = p; i < nbytes + p; i++){
			/* 置0是从申请的内存的第16字节开始 */
			/* 因为前16个字节保存了申请内存的大小 */
			*i = 0;
		}
	}

	return p;
}
