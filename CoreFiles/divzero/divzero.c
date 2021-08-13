/* 除0测试 */
#include "apilib.h"
#include <stdio.h>

void KaliMain(void){
	int i = 15;
	if(i/0 < 0){
		printf("AAAAA");
	} else {
		printf("BBBBB");
	}
	api_end();
}
