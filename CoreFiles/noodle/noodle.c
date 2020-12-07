/* 简单的定时器程序 */
#include <stdio.h>
#include "apilib.h"

void KaliMain(void){
	int *buf;
	char s[12];
	int win, timer, sec = 0, min = 0, hou = 0;
	api_initmalloc();
	buf = api_malloc(150 * 50 * 4);
	win = api_openwin(buf, 150, 50, -1, "noodle");
	timer = api_alloctimer();
	api_inittimer(timer, 128);
	for (;;) {
		sprintf(s, "%5d:%02d:%02d", hou, min, sec);
		api_boxfilwin(win, 28, 27, 115, 41, 0x00ffffff /* 白 */);
		api_putstrwin(win, 28, 27, 0 /* 黑 */, 11, s);
		api_settimer(timer, 100);	/* 1秒 */
		if (api_getkey(1) != 128) {
			break;
		}
		sec++;
		if (sec == 60) {
			sec = 0;
			min++;
			if (min == 60) {
				min = 0;
				hou++;
			}
		}
	}
	api_end();
}
