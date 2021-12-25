/* 用于测试简单的动态窗口 */
#include "apilib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// https://zhuanlan.zhihu.com/p/30745861

#define WIDTH        256
#define HEIGHT       256
#define W  (WIDTH + 16)
#define H  (HEIGHT + 16 + 18)

#define TWO_PI 6.28318530718f
#define N 256

#define MAX_STEP 64
#define MAX_DISTANCE 5.0f
#define EPSILON 1e-6f

#define BIAS 1e-4f
#define MAX_DEPTH 5

#define BLACK { 0.0f, 0.0f, 0.0f }

// typedef struct { float sd, emissive; } Result;
// typedef struct { float sd, emissive, reflectivity; } Result;
// typedef struct { float sd, emissive, reflectivity, eta; } Result;
// typedef struct { float sd, emissive, reflectivity, eta, absorption; } Result;
typedef struct { float r, g, b; } Color;
typedef struct {
    float sd, reflectivity, eta;
    Color emissive, absorption;
} Result;

float circleSDF(float x, float y, float cx, float cy, float r) {
	/* 圆形的SDF */
    float ux = x - cx, uy = y - cy;
    return ((float)sqrtf((double)(ux * ux + uy * uy))) - r;
}

float planeSDF(float x, float y, float px, float py, float nx, float ny) {
	/* 平面SDF */
    return (x - px) * nx + (y - py) * ny;
}

float segmentSDF(float x, float y, float ax, float ay, float bx, float by) {
	/* 线段的SDF */
    float vx = x - ax, vy = y - ay, ux = bx - ax, uy = by - ay;
    float t = fmaxf(fminf((vx * ux + vy * uy) / (ux * ux + uy * uy), 1.0f), 0.0f);
    float dx = vx - ux * t, dy = vy - uy * t;
    return sqrtf(dx * dx + dy * dy);
}

float capsuleSDF(float x, float y, float ax, float ay, float bx, float by, float r) {
	/* 胶囊的SDF */
    return segmentSDF(x, y, ax, ay, bx, by) - r;
}

float boxSDF(float x, float y, float cx, float cy, float theta, float sx, float sy) {
	/* 矩形SDF */
    float costheta = cosf(theta), sintheta = sinf(theta);
    float dx = fabs((x - cx) * costheta + (y - cy) * sintheta) - sx;
    float dy = fabs((y - cy) * costheta - (x - cx) * sintheta) - sy;
    float ax = fmaxf(dx, 0.0f), ay = fmaxf(dy, 0.0f);
    return fminf(fmaxf(dx, dy), 0.0f) + sqrtf(ax * ax + ay * ay);
}

float triangleSDF(float x, float y, float ax, float ay, float bx, float by, float cx, float cy) {
	/* 三角形SDF */
    float d = fminf(fminf(
        segmentSDF(x, y, ax, ay, bx, by),
        segmentSDF(x, y, bx, by, cx, cy)),
        segmentSDF(x, y, cx, cy, ax, ay));
    return (bx - ax) * (y - ay) > (by - ay) * (x - ax) && 
           (cx - bx) * (y - by) > (cy - by) * (x - bx) && 
           (ax - cx) * (y - cy) > (ay - cy) * (x - cx) ? -d : d;
}

// float ngonSDF(float x, float y, float cx, float cy, float r, float n) {
    // float ux = x - cx, uy = y - cy, a = TWO_PI / n;
    // float t = fmodf(atan2f(uy, ux) + TWO_PI, a), s = sqrtf(ux * ux + uy * uy);	// fmodf、atan2f还没有实现
    // return planeSDF(s * cosf(t), s * sinf(t), r, 0.0f, cosf(a * 0.5f), sinf(a * 0.5f));
// }

Result unionOp(Result a, Result b) {
    return a.sd < b.sd ? a : b;
}

Result intersectOp(Result a, Result b) {
    Result r = a.sd > b.sd ? b : a;
    r.sd = a.sd > b.sd ? a.sd : b.sd;
    return r;
}

Result subtractOp(Result a, Result b) {
    Result r = a;
    r.sd = (a.sd > -b.sd) ? a.sd : -b.sd;
    return r;
}

Color colorAdd(Color a, Color b) {
	/* 色彩加法(叠加) */
    Color c = { a.r + b.r, a.g + b.g, a.b + b.b };
    return c;
}

Color colorMultiply(Color a, Color b) {
	/* 乘法(哈达马积) */
    Color c = { a.r * b.r, a.g * b.g, a.b * b.b };
    return c;
}

Color colorScale(Color a, float s) {
	/* 缩放(乘以纯量) */
    Color c = { a.r * s, a.g * s, a.b * s };
    return c;
}

Result scene(float x, float y) {
	/* 场景 */
	/********/
    // Result r1 = { circleSDF(x, y, 0.3f, 0.3f, 0.10f), 2.0f };
    // Result r2 = { circleSDF(x, y, 0.3f, 0.7f, 0.05f), 0.8f };
    // Result r3 = { circleSDF(x, y, 0.7f, 0.5f, 0.10f), 0.0f };
    // return unionOp(unionOp(r1, r2), r3);
	/********/
    // Result a = { circleSDF(x, y, 0.4f, 0.5f, 0.20f), 1.0f };
    // Result b = { circleSDF(x, y, 0.6f, 0.5f, 0.20f), 0.8f };
    // return intersectOp(a, b);
    // return subtractOp(a, b);
    // return subtractOp(b, a);
	// return unionOp(a, b);
	/********/
    // Result a = { circleSDF(x, y, 0.5f, 0.5f, 0.2f), 1.0f };
    // Result b = {  planeSDF(x, y, 0.0f, 0.5f, 0.0f, 1.0f), 0.8f };
    // return intersectOp(a, b);
	/********/
    // Result c = {  capsuleSDF(x, y, 0.4f, 0.4f, 0.6f, 0.6f, 0.1f), 1.0f };
    // return c;
	/********/
    // Result d = { boxSDF(x, y, 0.5f, 0.5f, TWO_PI / 16.0f, 0.3f, 0.1f), 1.0f };
    // return d;
	/********/
    // Result f = { triangleSDF(x, y, 0.5f, 0.2f, 0.8f, 0.8f, 0.3f, 0.6f), 1.0f };
    // return f;
	/********/
    // Result a = {  circleSDF(x, y, 0.4f,  0.2f, 0.1f), 2.0f };
    // Result b = {     boxSDF(x, y, 0.5f,  0.8f, TWO_PI / 16.0f, 0.1f, 0.1f), 0.0f };
    // Result c = {     boxSDF(x, y, 0.8f,  0.5f, TWO_PI / 16.0f, 0.1f, 0.1f), 0.0f };
    // return unionOp(unionOp(a, b), c);
	/********/
    // Result a = {  circleSDF(x, y, 0.4f,  0.2f, 0.1f), 2.0f, 0.0f };
    // Result b = {     boxSDF(x, y, 0.5f,  0.8f, TWO_PI / 16.0f, 0.1f, 0.1f), 0.0f, 0.9f };
    // Result c = {     boxSDF(x, y, 0.8f,  0.5f, TWO_PI / 16.0f, 0.1f, 0.1f), 0.0f, 0.9f };
    // return unionOp(unionOp(a, b), c);
	/********/
    // Result a = { circleSDF(x, y, 0.5f, -0.2f, 0.1f), 0.0f, 0.0f, { 10.0f, 10.0f, 10.0f }, BLACK };
    // Result b = {    boxSDF(x, y, 0.5f, 0.5f, 0.0f, 0.3, 0.2f), 0.0f, 1.5f, BLACK, { 4.0f, 4.0f, 1.0f} };
	Result a = { circleSDF(x, y, -0.2f, -0.2f, 0.1f), 0.0f, 0.0f, { 10.0f, 10.0f, 10.0f }, BLACK };
	Result b = { triangleSDF(x, y, 0.5f, 0.2f, 0.8f, 0.8f, 0.3f, 0.6f), 0.0f, 1.5f, BLACK, { 4.0f, 1.0f, 4.0f} };
    return unionOp(a, b);
}

void reflect(float ix, float iy, float nx, float ny, float* rx, float* ry) {
	/* 反射 */
    float idotn2 = (ix * nx + iy * ny) * 2.0f;
    *rx = ix - idotn2 * nx;
    *ry = iy - idotn2 * ny;
}

int refract(float ix, float iy, float nx, float ny, float eta, float* rx, float* ry) {
	/* 折射 */
    float idotn = ix * nx + iy * ny;
    float k = 1.0f - eta * eta * (1.0f - idotn * idotn);
    if (k < 0.0f)
        return 0; // 全内反射
    float a = eta * idotn + sqrtf(k);
    *rx = eta * ix - a * nx;
    *ry = eta * iy - a * ny;
    return 1;
}

float fresnel(float cosi, float cost, float etai, float etat) {
	/* 菲涅尔方程 */
    float rs = (etat * cosi - etai * cost) / (etat * cosi + etai * cost);
    float rp = (etai * cosi - etat * cost) / (etai * cosi + etat * cost);
    return (rs * rs + rp * rp) * 0.5f;
}

float schlick(float cosi, float cost, float etai, float etat) {
	/* Schlick 近似 */
    float r0 = (etai - etat) / (etai + etat);
    r0 *= r0;
    float a = 1.0f - (etai < etat ? cosi : cost);
    float aa = a * a;
    return r0 + (1.0f - r0) * aa * aa * a;
}

Color beerLambert(Color a, float d) {
	/* 比尔-朗伯定律 */
    Color c = { expf(-a.r * d), expf(-a.g * d), expf(-a.b * d) };
    return c;
}

void gradient(float x, float y, float* nx, float* ny) {
	/* 剃度 */
    *nx = (scene(x + EPSILON, y).sd - scene(x - EPSILON, y).sd) * (0.5f / EPSILON);
    *ny = (scene(x, y + EPSILON).sd - scene(x, y - EPSILON).sd) * (0.5f / EPSILON);
}

Color trace(float ox, float oy, float dx, float dy, int depth) {
	/* 光线追踪 */
    float t = 1e-3f;
    float sign = scene(ox, oy).sd > 0.0f ? 1.0f : -1.0f; // 内还是外？
	int i;
    for (i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
        float x = ox + dx * t, y = oy + dy * t;
        Result r = scene(x, y);
        if (r.sd * sign < EPSILON) { // 判断是否到达表面时要考虑内外
            Color sum = r.emissive;
            if (depth < MAX_DEPTH && (r.reflectivity > 0.0f || r.eta > 0.0f)) {
                float nx, ny, rx, ry, refl = r.reflectivity;
                gradient(x, y, &nx, &ny);
                nx *= sign; ny *= sign; // 在内的话，要反转法线
                if (r.eta > 0.0f) {
					if (refract(dx, dy, nx, ny, sign < 0.0f ? r.eta : 1.0f / r.eta, &rx, &ry)) {
						float cosi = -(dx * nx + dy * ny);
						float cost = -(rx * nx + ry * ny);
						refl = sign < 0.0f ? fresnel(cosi, cost, r.eta, 1.0f) : fresnel(cosi, cost, 1.0f, r.eta);
						sum = colorAdd(sum, colorScale(trace(x - nx * BIAS, y - ny * BIAS, rx, ry, depth + 1), 1.0f - refl));
					}
					else
						refl = 1.0f; // Total internal reflection
                }
                if (refl > 0.0f) {
                    reflect(dx, dy, nx, ny, &rx, &ry);
                     sum = colorAdd(sum, colorScale(trace(x + nx * BIAS, y + ny * BIAS, rx, ry, depth + 1), refl));
                }
            }
            return colorMultiply(sum, beerLambert(r.absorption, t));
        }
        t += r.sd * sign;
    }
	Color black = BLACK;
    return black;
}

Color sample(float x, float y) {
	int i;

    Color sum = BLACK;
    for (i = 0; i < N; i++) {
        float a = TWO_PI * (i + (float)rand() / RAND_MAX) / N;
        sum = colorAdd(sum, trace(x, y, cosf(a), sinf(a), 0));
    }
    return colorScale(sum, 1.0f / N);
}

void KaliMain(void){
	int buf[W * H * 4];
	int win, /*p,*/ x, y;
	Color c;

	win = api_openwin(buf, W, H, -1, "shape");
	
	// printf("%d\n",sizeof(buf));

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
			/* ***注意：使用api_point函数会降低程序效率，如果不是为了演示过程，请直接写入buf并刷新图层*** */
			c = sample((float)x / W, (float)y / H);
			// buf[x+y*W]=((int)(fminf(c.r * 255.0f, 255.0f))<<16)+((int)(fminf(c.g * 255.0f, 255.0f))<<8)+(int)(fminf(c.b * 255.0f, 255.0f));
			api_point(win, x + 8, y + 8 + 18, ((int)(fminf(c.r * 255.0f, 255.0f))<<16)+((int)(fminf(c.g * 255.0f, 255.0f))<<8)+(int)(fminf(c.b * 255.0f, 255.0f)));
		}
	}
	
	for (;;)
		if (api_getkey(1) == 0x1b)
			break;
		
	api_end();
}
