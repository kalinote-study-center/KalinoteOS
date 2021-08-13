/* C语言标准函数库 isdigit */
#include <ctype.h>

int isdigit(char c){
	return '0' <= c && c <= '9';
}
