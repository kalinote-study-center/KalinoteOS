/* 多色彩测试[该程序有问题] */
#include "apilib.h"

void KaliMain(void)
{
	int *buf;
	int win, x, y;
	api_initmalloc();
	buf = api_malloc(271 * 291 * 4);
	win = api_openwin(buf, 271, 291, -1, "full color");
	api_putstr0("enjoy full color :)");
	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			api_point(win, x + 8, y + 28, 0x000000ff /* RGB色彩 */ );
		}
	}
	api_refreshwin(win, 8, 28, 136, 156);
	api_getkey(1); /* 等待按下任意键 */
	api_end();
}
