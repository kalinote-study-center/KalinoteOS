/* Ӧ�ó���C����ʱ��ʼ������ */
/*
* ������Ӧ�ó������ڳ�ʼ������
*/
#include <apilib.h>

extern int main(int argc, char* argv[]);
void __main(void){}		/* TODO���Ҳ�֪��Ϊʲôkal�������ɻ���Ժ��call __main�������������ᱨ��(�����Ǳ��������õ�ԭ��) */
/* ���ϣ�ȷʵ�Ǳ����������⣬Ŀǰ��ʱֻ�������ְ취����� */

void KaliStartup(void) {
	int result;			/* ���ڽ��ճ���main���ص�״̬�� */
	int argc = 0;
	char *argv[16] = {0};
	
	result = main(argc, argv);

	api_end();
}
