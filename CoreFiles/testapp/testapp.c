/* ���ڲ��Լ򵥵Ķ�̬���� */
#include "apilib.h"
#include <stdio.h>
#include <time.h>

void KaliMain(void){
	time_t curtime;
	time(&curtime);
	printf("��ǰʱ��(��) = %u\n", curtime);
	api_end();
}
