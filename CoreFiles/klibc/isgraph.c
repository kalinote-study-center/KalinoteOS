/* C语言标准函数库 isgraph */
#include <ctype.h>

int isgraph(int c) {
	/* 有图形打印方法返回32768，否则返回0 */
	if ( c >= 0x21 && c <= 0x7e ) return 32768;
	return 0;
}
