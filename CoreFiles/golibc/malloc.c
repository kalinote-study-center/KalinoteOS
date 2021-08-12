/* C语言标准函数 分配内存 */
#include <stdlib.h>
#include "../apilib.h"

void *malloc(unsigned int nbytes){
	int *p = api_malloc(nbytes + 16);
	if(p != 0){
		*((int *) p) = nbytes;
		p+=16;
	}
	return p;
}