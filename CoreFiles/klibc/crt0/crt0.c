/* Ӧ�ó���C����ʱ��ʼ������ */
/*
* ������Ӧ�ó������ڳ�ʼ������
*/
#include <apilib.h>
#include <stdlib.h>
#include <string.h>

extern int main(int argc, char* argv[]);
void __main(void){}		/* (�ѹ���)�Ҳ�֪��Ϊʲôkal�������ɻ���Ժ��call __main�������������ᱨ��(�����Ǳ��������õ�ԭ��) */
/* ���ϣ�ȷʵ�Ǳ����������⣬Ŀǰ��ʱֻ�������ְ취����� */
/* ���ϣ�__main������������ִ��ǰ����ִ��(���ݹ۲�Դ���뼰ʵ�ʲ��Եó��ý���) */

void KaliStartup(void) {
	int argc = 0;
	char* argv[16] = {0};
	
	int result = main(argc, argv);		

	api_end();
}
