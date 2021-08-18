/* C语言标准函数库 stdlib.h difftime */
#include <time.h>

double difftime(time_t time1, time_t time2) {
	/* 注意：该函数可用性未知 */
	return (double)(time1-time2);
}
