/* C语言标准函数库 格式化输出 */
#include <stdio.h>
#include <stdarg.h>
#include <apilib.h>

int printf(char *format, ...){
	/* printf需要分配1k的stack，所以调用该函数的kal需要配置1k以上的stack */
	va_list ap;
	char s[1000];
	int i;
	
	va_start(ap, format);
	i = vsprintf(s, format, ap);
	api_putstr0(s);
	va_end(ap);
	return i;
}
