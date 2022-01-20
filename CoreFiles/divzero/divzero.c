/* 除0测试 */
#include <apilib.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
	int i = rand();
	if(i/0 < 0){
		printf("AAAAA");
	} else {
		printf("BBBBB");
	}
	return 0;
}
