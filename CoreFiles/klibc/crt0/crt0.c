/* 应用程序C运行时初始化程序 */
/*
* 这里是应用程序的入口初始化程序
*/
#include <apilib.h>

extern void KaliMain(void);

void KaliStartup(void) {

	KaliMain();

	api_end();
}
