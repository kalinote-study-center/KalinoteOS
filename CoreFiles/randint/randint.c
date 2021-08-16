/* 用于测试简单的动态窗口 */
#include "apilib.h"
#include <stdlib.h>
#include <stdio.h>

void KaliMain(void){
	int r;
	srand(api_get_system_runtime()+api_get_sec_hex()*api_get_min_hex());		/* 设置随机种子 */
	r = rand();
	printf("rand int:%d", r);
	api_end();
}
