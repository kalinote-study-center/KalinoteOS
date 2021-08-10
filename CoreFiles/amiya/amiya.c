/* 显示图片API测试 */
#include "apilib.h"

#define HEIGHT	450
#define WIDTH	450

void KaliMain(void) {
	int *buf;
	int win;
	
	api_initmalloc();
	buf = api_malloc(WIDTH * HEIGHT * 4);
	win = api_openwin(buf, WIDTH, HEIGHT, -1, "amiya");
	
	api_showjpeg_center("amiya.jpg", WIDTH, HEIGHT, win);
	
	for (;;) {
		if (api_getkey(1) == 0x0a) {
			break; /* Enter结束 */
		}
	}
	api_end();
}
