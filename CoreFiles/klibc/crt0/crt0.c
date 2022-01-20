/* 应用程序C运行时初始化程序 */
/*
* 这里是应用程序的入口初始化程序
*/
#include <apilib.h>

extern int main(int argc, char* argv[]);
void __main(void){}		/* TODO：我不知道为什么kal程序编译成汇编以后会call __main，如果不加这个会报错(或许是编译器设置的原因？) */
/* 接上：确实是编译器的问题，目前暂时只能用这种办法来解决 */

void KaliStartup(void) {
	int result;			/* 用于接收程序main返回的状态码 */
	int argc = 0;
	char *argv[16] = {0};
	
	result = main(argc, argv);

	api_end();
}
