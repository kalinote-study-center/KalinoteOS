/* ´°¿ÚÍ¸Ã÷Í¼²ã²âÊÔ³ÌÐò */
#include "apilib.h"

void KaliMain(void)
{
	int win;
	int buf[150 * 70 * 4];
	win = api_openwin(buf, 150, 70, 255, "notrec");
	api_boxfilwin(win,   0, 50,  34, 69, 255);
	api_boxfilwin(win, 115, 50, 149, 69, 255);
	api_boxfilwin(win,  50, 30,  99, 49, 255);
	for (;;) {
		if (api_getkey(1) == 0x0a) {
			break; /* Enter to break; */
		}
	}
	api_end();
}
