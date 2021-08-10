/* 除0测试 */
#include "apilib.h"
#include "stdlib.h"

void KaliMain(void){
	int i = 15;
	if(i/0 < 0){
		printf('A');
	} else {
		printf('B');
	}
	api_end();
}
