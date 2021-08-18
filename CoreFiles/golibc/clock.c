/* C语言标准函数库 stdlib.h clock */
#include <time.h>
#include <../apilib.h>

clock_t clock(void) {
	return api_get_system_runtime();
}
