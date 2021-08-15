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

int abs(int n);
double atof(const char *s);
int atoi(const char *s);
void qsort(void *base, size_t n, size_t size,
	int (*cmp)(const void *, const void *));
void srand(int seed);
int rand(void);
extern unsigned int rand_seed;
double strtod(const char *s, const char **endp);
long strtol(const char *s, const char **endp, int base);
unsigned long strtoul(const char *s, const char **endp, int base);

/* for strdup() */
void *malloc(unsigned int nbytes);
void free(void *ap);

#if (defined(__cplusplus))
	}
#endif

#endif
