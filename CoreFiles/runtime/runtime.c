/* 获取系统运行时间 */
#include "apilib.h"
#include <stdio.h>

void KaliMain(void) {
	printf("%u sec\n", api_get_system_runtime());
	api_end();
}
