/* ���ڲ��Լ򵥵Ķ�̬���� */
#include "apilib.h"
#include <stdlib.h>
#include <stdio.h>

void KaliMain(void){
	int r;
	srand(api_get_system_runtime()+api_get_sec_hex()*api_get_min_hex());		/* ����������� */
	r = rand();
	printf("rand int:%d", r);
	api_end();
}
