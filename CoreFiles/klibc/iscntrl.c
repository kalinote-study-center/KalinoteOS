/* C语言标准函数库 iscntrl */
#include <ctype.h>

int iscntrl(int c) {
	/* 控制字符返回2，否则返回0 */
	if ( c == 127 || (c >= 0 && c <= 31) ) return 2;
	return 0;
}
