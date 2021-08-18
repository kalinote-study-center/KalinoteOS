/* C语言标准函数库 stdlib.h time */
#include <time.h>
#include <stdio.h>

char *ctime(const time_t *timer) {
	/* 由于某些原因，计算长整数会出错，这个暂时没办法处理 */
	static char str[50];	
	sprintf(str,"this function doesn't work yet. (%u)",timer);
	return str;
}
