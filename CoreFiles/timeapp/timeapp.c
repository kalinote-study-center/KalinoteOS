/* 图形时钟APP */
#include "apilib.h"
#include "stdlib.h"

#include <stdio.h>
#include <math.h>

#define High 240
#define Width 320
#define PI 3.14159

void KaliMain(void) {
	int *win_buf;	/* 窗口内存缓冲区 */
	int win;		/* 窗口句柄 */
	int i;			/* 角度 */
	int x,y;		/* 坐标 */
	int r = 50;		/* 半径 */

	int center_x = Width/2, center_y = High/2;

	int secondLength = Width/5;
	int minuteLength = Width/6;
	int hourLength = Width/7;

	int secondEnd_x, secondEnd_y;
	int minuteEnd_x, minuteEnd_y;
	int hourEnd_x, hourEnd_y;

	api_initmalloc();						/* 初始化内存 */
	win_buf = api_malloc(Width * High * 4);		/* 分配窗口内存缓冲区 */
	win = api_openwin(win_buf, Width, High, -1, "Time App");	/* 绘制窗口 */

	while(1){
		/* 涂黑 */
		api_boxfilwin(win,  4, 22, Width - 4, High - 4, 0 /* 黑色 */);
		for(i = 0;i < 360;i++){
			/* 尝试画圆 */
			// x = (int)(cos(PI * 2 / 360 * i) * r) + 100;		/* 计算x坐标 */
			// y = (int)(sin(PI * 2 / 360 * i) * r) + 150;		/* 计算y坐标 */
			// api_point(win, x, y, 0xffffff);		/* 画点 */
			cos(PI / 2);
		}
	}

	api_end();
}
