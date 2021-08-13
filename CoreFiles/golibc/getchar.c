/* C语言标准函数库 getchar */
#include "../apilib.h"
#include <stdio.h>

int getchar(){
	int c = api_getkey(1);
	putchar(c);
	return c;
}
