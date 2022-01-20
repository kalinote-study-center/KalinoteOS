/* 用于测试简单的动态窗口 */
#include <apilib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*
* 这个KAL还需要实现math库的fminf、sqrtf、sinf、cosf
*/

// https://zhuanlan.zhihu.com/p/30745861

#define WIDTH        512
#define HEIGHT       512
#define W  (WIDTH + 16)
#define H  (HEIGHT + 16 + 18)

#define TWO_PI 6.28318530718f
#define N 64

#define MAX_STEP 10
#define MAX_DISTANCE 2.0f
#define EPSILON 1e-6f

float circleSDF(float x, float y, float cx, float cy, float r) {
    float ux = x - cx, uy = y - cy;
    return ((float)sqrtf((double)(ux * ux + uy * uy))) - r;
}

float trace(float ox, float oy, float dx, float dy) {
    float t = 0.0f;
	int i;
    for (i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
        float sd = circleSDF(ox + dx * t, oy + dy * t, 0.5f, 0.5f, 0.1f);
        if (sd < EPSILON)
            return 2.0f;
        t += sd;
    }
    return 0.0f;
}

float sample(float x, float y) {
    float sum = 0.0f;
	int i;
    for (i = 0; i < N; i++) {
        // float a = TWO_PI * rand() / RAND_MAX;					// 均匀采样
		// float a = TWO_PI * i / N;								// 分层采样
        float a = TWO_PI * (i + (float)rand() / RAND_MAX) / N;		// 抖动采样
		sum += trace(x, y, (float)cos((double)a), (float)sin((double)a));
    }
    return sum / N;
}

int main(){
	int *buf;
	int win, p, x, y;
	
	api_initmalloc();
	buf = api_malloc(W * H * 4);
	win = api_openwin(buf, W, H, -1, "light");
	

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
			// printf("%d,%d\n",x,y);
			p = (int)(fminf(sample((float)x / WIDTH, (float)y / HEIGHT) * 255.0f, 255.0f));
			// printf("%d\n",p);
			// printf("%08x\n",((p<<16)+(p<<8)+p));
			/* ***注意：使用api_point函数会降低程序效率，如果不是为了演示过程，请直接写入buf并刷新图层*** */
			api_point(win, x + 8, y + 8 + 18, (p<<16)+(p<<8)+p);
		}
	}
	
	for (;;)
		if (api_getkey(1) == 0x1b)
			break;
		
	return 0;
}
