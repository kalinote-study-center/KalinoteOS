/* C语言标准函数库 格式化输出 */
#include <stdio.h>
#include <stdarg.h>
#include "../apilib.h"

int printf(char *format, ...){
	va_list ap;
	char s[1000];
	int i;
	
	va_start(ap, format);
	i = vsprintf(s, format, ap);
	api_putstr0(s);
	va_end(ap);
	return i;
}
