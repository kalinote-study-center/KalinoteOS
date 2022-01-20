/* 获取系统运行时间 */
#include <apilib.h>
#include <stdio.h>

int main() {
	printf("%u ticks(10ms)\n", api_get_system_runtime());
	return 0;
}
