/* 获取系统运行时间 */
#include "apilib.h"
#include <stdio.h>

void KaliMain(void) {
	printf("%u ticks(10ms)\n", api_get_system_runtime());
	api_end();
}
