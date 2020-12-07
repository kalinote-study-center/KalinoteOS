/* 用于测试简单的动态窗口 */
#include "apilib.h"
#include "stdlib.h"

void KaliMain(void){
	int *buf;
	int win, i, timer;
	
	api_initmalloc();
	buf = api_malloc(200 * 200 * 4);
	win = api_openwin(buf, 200, 200, -1, "testWin");
	timer = api_alloctimer();
	api_inittimer(timer, 128);
	api_boxfilwin(win, 5, 20, 195, 195, 0x000000);
	printf("->标准函数打印测试<-");
	for(i=10;i<180;i++){
		api_boxfilwin(win, i, 20, i + 10, 120, 0xffff00);
		api_settimer(timer, 10);
		for(;;){
			if (api_getkey(1) == 128) {
				break;
			}
		}
		api_boxfilwin(win, i, 20, i + 10, 120, 0x000000);
	}
	api_end();
}
