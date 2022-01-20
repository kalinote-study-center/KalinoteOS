/* 应用程序C运行时初始化程序 */
/*
* 这里是应用程序的入口初始化程序
*/
#include <apilib.h>
#include <stdlib.h>
#include <string.h>

extern int main(int argc, char* argv[]);
void __main(void){}		/* (已过期)我不知道为什么kal程序编译成汇编以后会call __main，如果不加这个会报错(或许是编译器设置的原因？) */
/* 接上：确实是编译器的问题，目前暂时只能用这种办法来解决 */
/* 接上：__main会在整个程序执行前最先执行(根据观察源代码及实际测试得出该结论) */

void KaliStartup(void) {
	int argc = 0;
	char* argv[16] = {0};
	
	int result = main(argc, argv);		

	api_end();
}
