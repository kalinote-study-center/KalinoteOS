/* C语言标准函数库 isupper */
#include <ctype.h>

int isupper(int c) {
	/* 大写字母返回256，否则返回0 */
	if ( c>='A' && c<='Z' ) return 256;
	return 0;
}
