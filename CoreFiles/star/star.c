/* 绘制点的测试程序 */
#include <apilib.h>
#include <stdlib.h>

int main(){
	int *buf;
	int win, i, x, y;
	api_initmalloc();
	buf = api_malloc(150 * 100 * 4);
	win = api_openwin(buf, 150, 100, -1, "stars2");
	api_boxfilwin(win + 1,  6, 26, 143, 93, 0 /* 黑 */);
	for (i = 0; i < 50; i++) {
		x = (rand() % 137) +  6;
		y = (rand() %  67) + 26;
		api_point(win + 1, x, y, 0x00ffff00 /* 黄 */);
	}
	api_refreshwin(win,  6, 26, 144, 94);
	for (;;) {
		if (api_getkey(1) == 0x0a) {
			break; /* Enter to break; */
		}
	}
	return 0;
}
