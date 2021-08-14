/* C语言标准函数库 isalnum */
#include <ctype.h>

int isalnum(int c) {
	/* 数字或字母返回8，否则返回0 */
	if ( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') ) return 8;
	return 0;
}
