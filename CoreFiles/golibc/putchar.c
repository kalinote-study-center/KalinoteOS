/* C语言标准函数库 putchar */
#include <stdio.h>
#include "../apilib.h"

int putchar(int c){
	api_putchar(c);
	return c;
}
