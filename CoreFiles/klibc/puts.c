/* C语言标准函数库 puts */
#include <apilib.h>
#include <stdio.h>
#include <string.h>

int puts(const char *str){
	api_putstr0((char *)str);
	putchar('\n');
	return strlen(str);
}
