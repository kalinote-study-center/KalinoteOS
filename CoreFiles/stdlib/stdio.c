/* stdio.h */
#include "../apilib.h"
#include "../stdlib.h"
#include <stdio.h>
#include <stdarg.h>
// #include <string.h>

#define MAX_BUF 1024

long int strtol(const char *str, char **endptr, int base);


int printf(char *format, ...){
	va_list ap;
	char s[1000];
	int i;
	
	va_start(ap, format);
	i = vsprintf(s, format, ap);
	api_putstr0(s);
	va_end(ap);
	return i;
}

int putchar(int c){
	api_putchar(c);
	return c;
}

int scanf(const char *format, ...){
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

int getchar(){
	int c = api_getkey(1);
	putchar(c);
	return c;
}

int puts(const char *str){
	api_putstr0((char *)str);
	putchar('\n');
	return strlen(str);
}

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
