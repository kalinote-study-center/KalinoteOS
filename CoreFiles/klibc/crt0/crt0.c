/* Ӧ�ó���C����ʱ��ʼ������ */
/*
* ������Ӧ�ó������ڳ�ʼ������
*/
#include <apilib.h>

extern void KaliMain(void);

void KaliStartup(void) {

	KaliMain();

	api_end();
}
