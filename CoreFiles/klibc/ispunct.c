/* C语言标准函数库 ispunct */
#include <ctype.h>

int ispunct(int c) {
	/* 如果是标点符号返回4，否则返回0 */
	if ( isprint(c) && !isalnum(c) && !isspace(c) ) return 4;
	return 0;
}
