/* C语言标准函数库 math for KalinoteOS */

#if (!defined(MATH_H))

#define MATH_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#define M_PI  (3.1415926535897932384626433832795)
#define M_2PI  (M_PI * 2)
#define M_PI_2  (M_PI / 2)

double sin(double);
double cos(double);
double sqrt(double);
double ldexp(double x, int n);
double frexp(double x, int *exp);
double fabs(double x);				 // 获取浮点数绝对值

extern __inline__ double sin(double x)
{
	double res;
	__asm__ ("fsin" : "=t" (res) : "0" (x));
	return res;
}

extern __inline__ double cos(double x)
{
	double res;
	__asm__ ("fcos" : "=t" (res) : "0" (x));
	return res;
}

extern __inline__ double sqrt(double x)
{
	double res;
	__asm__ ("fsqrt" : "=t" (res) : "0" (x));
	return res;
}

#if (defined(__cplusplus))
	}
#endif

#endif
