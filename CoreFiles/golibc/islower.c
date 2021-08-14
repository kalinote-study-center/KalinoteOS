/* C语言标准函数库 islower */
#include <ctype.h>

int islower(int c) {
	/* 小写字母返回512，否则返回0 */
	if ( c>='a' && c<='z' ) return 512;
	return 0;
}
