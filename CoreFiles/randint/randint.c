/* ���ڲ��Լ򵥵Ķ�̬���� */
#include "apilib.h"
#include <stdlib.h>
#include <stdio.h>

void KaliMain(void){
	srand(api_get_system_runtime()+api_get_sec_hex()*api_get_min_hex());		/* ����������� */
	printf("rand int:%d", rand());
	api_end();
}
