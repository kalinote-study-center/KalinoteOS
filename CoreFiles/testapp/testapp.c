/* 用于测试简单的动态窗口 */
#include "apilib.h"
#include <stdio.h>
#include <time.h>

void KaliMain(void){
	time_t curtime;
	time(&curtime);
	printf("当前时间(戳) = %u\n", curtime);
	api_end();
}
