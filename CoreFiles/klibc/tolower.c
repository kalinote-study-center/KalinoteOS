/* C语言标准函数库 tolower */
#include <ctype.h>

int tolower(int c) {
	/* 返回转换成的小写字母 */
	return c|32;
}
