/* C语言标准函数库 isprint */
#include <ctype.h>

int isprint(int c) {
	/* 可打印字符返回16384，否则返回0 */
	if ( c >= 0x20 && c <= 0x7e ) return 16384;
	return 0;
}
