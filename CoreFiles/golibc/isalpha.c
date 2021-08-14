/* C语言标准函数库 isalpha */
#include <ctype.h>

int isalpha(int c) {
	/* 字母返回8，否则返回0 */
	if ( (c>='a' && c<='z') || (c>='A' && c<='Z') ) return 1024;
	return 0;
}
