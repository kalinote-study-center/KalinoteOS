/* ����һ�����ڵ�Ӧ�ó������ʾ���� */
#include "apilib.h"

char buf[150 * 50];

void KaliMain(void)
{
	char *buf;
	int win;
	
	api_initmalloc();
	buf = api_malloc(150 * 50);
	win = api_openwin(buf, 150, 50, -1, "hello");
	api_boxfilwin(win,  8, 36, 141, 43, 3 /* ��ɫ */);
	api_putstrwin(win, 28, 28, 0 /* ��ɫ */, 12, "hello, world");
	api_end();
}
