/* ���������Գ��� */
#include "apilib.h"

void KaliMain(void)
{
	int i, timer;
	timer = api_alloctimer();
	api_inittimer(timer, 128);
	for (i = 20000; i <= 20000000; i += i / 100) {
		/* 20Hz��20KHz : �������������������Χ */
		/* i��1%���ٶȵݼ� */
		api_beep(i);
		api_settimer(timer, 1);		/* 0.01�� */
		if (api_getkey(1) != 128) {
			break;
		}
	}
	api_beep(0);	/* ֹͣ���� */
	api_end();
}