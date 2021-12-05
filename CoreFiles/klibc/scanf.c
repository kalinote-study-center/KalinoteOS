/* C语言标准函数库 scanf */
#include <apilib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUF 1024

int scanf(const char *format, ...){
	/* 未测试 */
    int count = 0, base = 0;
    char *temp, buffer[MAX_BUF];
    va_list ap;
    va_start(ap, format);
    while (*format) {
		while (isspace(*format))
		    format++;
		if (*format == '%') {
		    format++;
		    for (; *format; format++)
				if (strchr("dibouxcsefg%", *format))
				    break;
		    if (*format == 's') {
		    	char c = getchar();
				while (isspace(c))
					c = getchar();
				temp = va_arg(ap, char *);
				while (!isspace(c)) {
					*temp = c;
					temp++;
					c = getchar();
				}
				*temp = '\0';
		    } else if (*format == 'c') {
		    	char c = getchar();
				while (isspace(c))
					c = getchar();
				temp = va_arg(ap, char *);
				*temp = c;
		    } else if (strchr("dobxu", *format)) {
		    	char c = getchar();
				while (isspace(c))
					c = getchar();
				if (*format == 'd' || *format == 'u')
				    base = 10;
				else if (*format == 'x')
				    base = 16;
				else if (*format == 'o')
				    base = 8;
				else if (*format == 'b')
				    base = 2;
				temp = buffer;
				while (!isspace(c)) {
					*temp = c;
					temp++;
					c = getchar();
				}
				*temp = '\0';
				int *numptr = va_arg(ap, int *);
				*numptr = strtol(buffer, NULL, base);
		    }
			count++;
		    format++;
		} else if (*format) {
			char c = getchar();
		    while (isspace(c)) 
				c = getchar();
		    if (*format != c)
				break;
		    else 
		    	format++;
		}
	}
	va_end(ap);
    return (count);
}
