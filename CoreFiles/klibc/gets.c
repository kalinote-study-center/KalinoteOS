/* C语言标准函数库 gets */
#include <apilib.h>
#include <stdio.h>

char *gets(char *str){
	char *ptr = str;
	while (1) {
		char c = getchar();
		if (c == '\r' || c == '\n') {
			*ptr = '\0';
			return str;
		} else {
			*ptr = c;
			ptr++;
		}
	}
}

