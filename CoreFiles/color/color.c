/* 多色彩测试[该程序有问题] */
#include "apilib.h"

void KaliMain(void) {
	int buf[385 * 200 * 4];
	int win, i;
	api_initmalloc();
	win = api_openwin(buf, 383, 200, -1, "Full color!");
	for (i = 0; i <= 63; i++) {
		api_linewin(win, i + 2, 30, i + 2, 190, 0x00ff0000 + 0x00000100 * i * 4);
	}
	for (i = 0; i <= 63; i++) {
		api_linewin(win, i + 65, 30, i + 65, 190, 0x00ffff00 - 0x00010000 * i * 4);
	}
	for (i = 0; i <= 63; i++) {
		api_linewin(win, i + 128, 30, i + 128, 190, 0x0000ff00 + 0x00000001 * i * 4);
	}
	for (i = 0; i <= 63; i++) {
		api_linewin(win, i + 191, 30, i + 191, 190, 0x0000ffff - 0x00000100 * i * 4);
	}
	for (i = 0; i <= 63; i++) {
		api_linewin(win, i + 254, 30, i + 254, 190, 0x000000ff + 0x00010000 * i * 4);
	}
	for (i = 0; i <= 63; i++) {
		api_linewin(win, i + 317, 30, i + 317, 190, 0x00ff00ff - 0x00000001 * i * 4);
	}
	api_refreshwin(win, 0, 0, 200, 190);
	api_getkey(1);
	api_end();
}
