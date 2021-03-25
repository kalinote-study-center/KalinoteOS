/* ctype.h */
#include "../apilib.h"
#include "../stdlib.h"

int isspace(char c){
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int isdigit(char c){
	return '0' <= c && c <= '9';
}
