/* ���ڲ��Լ򵥵Ķ�̬���� */
#include "apilib.h"
#include <stdlib.h>

void KaliMain(void){
	char s[256];
	srand(api_get_system_runtime()+api_get_sec_hex()*api_get_min_hex());		/* ����������� */
	sprintf(s, "rand int:%d", rand());
	api_putstr0(s);
	api_end();
}
