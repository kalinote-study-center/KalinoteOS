/* 生成一个窗口的应用程序的演示代码 */
#include <apilib.h>

int main(){
	int *buf;
	int win;

	api_initmalloc();
	buf = api_malloc(150 * 50 * 4);
	win = api_openwin(buf, 150, 50, -1, "hello");
	api_boxfilwin(win,  8, 36, 141, 43, 0x0000ffff /* 青 */);
	api_putstrwin(win, 28, 28, 0 /* 黑 */, 12, "hello, world");
	for (;;) {
		if (api_getkey(1) == 0x0a) {
			break; /* Enter结束 */
		}
	}
	// api_end();
	return 0;
}
