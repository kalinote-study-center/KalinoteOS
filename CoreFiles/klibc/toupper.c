/* C语言标准函数库 toupper */
#include <ctype.h>

int toupper(int c) {
	/* 返回字母大写，否则返回原字符 */
	if ( c>='a' && c<='z' ) return c-32;
	return c;
}
