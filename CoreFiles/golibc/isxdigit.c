/* C语言标准函数库 isxdigit */
#include <ctype.h>

int isxdigit(int c) {
	/* 十六进制数返回4096，否则返回0 */
	if ( isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') ) return 4096;
	return 0;
}
