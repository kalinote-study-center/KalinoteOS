/* C语言标准函数库 isspace */
#include <ctype.h>

int isspace(char c){
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}
