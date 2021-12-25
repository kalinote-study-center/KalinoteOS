/* C语言标准函数库 math for KalinoteOS */

#if (!defined(MATH_H))

#define MATH_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#define M_PI	(3.1415926535897932384626433832795)
#define M_2PI	(M_PI * 2)
#define M_PI_2	(M_PI / 2)
#define M_E		(2.7182818284590452353602874713526)
// #define fminf(a,b) ((a)<(b)?(a):(b))

double sin(double);
float sinf(float);
double cos(double);
float cosf(float);
double sqrt(double);
float sqrtf(float);
double ldexp(double x, int n);
double frexp(double x, int *exp);
double fabs(double x);					// 获取浮点数绝对值
float fminf(float x, float y);
float fmaxf(float x, float y);
double pow(double x, double y);
float expf(float x);

extern __inline__ double sin(double x) {
	double res;
	__asm__ ("fsin" : "=t" (res) : "0" (x));
	return res;
}

extern __inline__ double cos(double x) {
	double res;
	__asm__ ("fcos" : "=t" (res) : "0" (x));
	return res;
}

extern __inline__ float sinf(float x) {
	float res;
	__asm__ ("fsin" : "=t" (res) : "0" (x));
	return res;
}

extern __inline__ float cosf(float x) {
	float res;
	__asm__ ("fcos" : "=t" (res) : "0" (x));
	return res;
}

extern __inline__ double sqrt(double x) {
	double res;
	__asm__ ("fsqrt" : "=t" (res) : "0" (x));
	return res;
}

extern __inline__ float sqrtf(float x) {
	float res;
	__asm__ ("fsqrt" : "=t" (res) : "0" (x));
	return res;
}

#if (defined(__cplusplus))
	}
#endif

#endif
