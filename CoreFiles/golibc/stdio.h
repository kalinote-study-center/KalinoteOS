/* C语言标准函数库 stdio */

#if (!defined(STDIO_H))

#define STDIO_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#if (!defined(NULL))
	#define NULL	((void *) 0)
#endif

#include <stdarg.h>

/* golibc */
int sprintf(char *s, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);
int putchar(int c);
int printf(char *format, ...);
int scanf(const char *format, ...);
int getchar();
int puts(const char *str);
char *gets(char *str);

#if (defined(__cplusplus))
	}
#endif

#endif
