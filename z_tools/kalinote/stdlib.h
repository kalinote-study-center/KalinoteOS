/* C语言标准函数库 stdlib for KalinoteOS  */

#if (!defined(STDLIB_H))

#define STDLIB_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* stdlib .h 头文件定义了几个变量类型、一些宏和各种通用工具函数。 */

#include <stddef.h>		/* size_t */

typedef struct
{
  int quot;
  int rem;
} div_t;				// 商和余数结构体
typedef struct
{
  long quot;
  long rem;
} ldiv_t;				// 商和余数结构体

#if (!defined(NULL))
	#define NULL	((void *) 0)
#endif
#define	RAND_MAX		·	0x7fff			// 这个宏是 rand 函数返回的最大值。
#define EXIT_SUCCESS		0				// 这是 exit 函数成功时要返回的值。
#define EXIT_FAILURE		1				// 这是 exit 函数失败时要返回的值。

int abs(int n);															// 返回 x 的绝对值。
double atof(const char *s);                                             // 把参数 str 所指向的字符串转换为一个浮点数（类型为 double 型）。
int atoi(const char *s);                                                // 把参数 str 所指向的字符串转换为一个整数（类型为 int 型）。
long int atol(const char *s);											// 把参数 str 所指向的字符串转换为一个长整数（类型为 long int 型）。
void qsort(void *base, size_t n, size_t size,
	int (*cmp)(const void *, const void *));                            // 数组排序。
void srand(int seed);                                                   // 该函数播种由函数 rand 使用的随机数发生器。
int rand(void);                                                         // 返回一个范围在 0 到 RAND_MAX 之间的伪随机数。
extern unsigned int rand_seed;                                          // 随机数种
double strtod(const char *s, const char **endp);                        // 把参数 str 所指向的字符串转换为一个浮点数（类型为 double 型）。
long strtol(const char *s, const char **endp, int base);                // 把参数 str 所指向的字符串转换为一个长整数（类型为 long int 型）。
unsigned long strtoul(const char *s, const char **endp, int base);      // 把参数 str 所指向的字符串转换为一个无符号长整数（类型为 unsigned long int 型）。

/* for strdup() */
void *malloc(unsigned int nbytes);                                      // 分配所需的内存空间，并返回一个指向它的指针。
void *calloc(size_t nitems, size_t size);								// 分配所需的内存空间，并全部置0，然后返回一个指向它的指针。
void free(void *ap);                                                    // 释放之前调用 calloc、malloc 或 realloc 所分配的内存空间。
// void *realloc(void *ptr, size_t size);								// (由于安全性问题，不使用该函数)尝试重新调整之前调用 malloc 或 calloc 所分配的 ptr 所指向的内存块的大小。
// void abort(void);													// 由于某些原因，暂时没办法实现，可以使用api_end();系统调用
// void exit(int status);												// 由于某些原因，暂时没办法实现，可以使用api_end();系统调用
// int atexit(void (*func)(void));										// 由于某些原因，暂时没办法实现
// char *getenv(const char *name);										// KalinoteOS暂时还没办法实现这个功能:(


#if (defined(__cplusplus))
	}
#endif

#endif
