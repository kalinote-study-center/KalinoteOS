/* 兼容性标准函数 */

/*
* 等有空了把这些函数集成到golibc.lib(C语言标准函数库)中去
*/

/* string.h */
int strchr(const char *str, char c);
/* malloc.h(stdlib.h) */
void *malloc(int size);
void free(void *p);
/* stdio.h */
int putchar(int c);
int printf(char *format, ...);
int scanf(const char *format, ...);
int getchar();
int puts(const char *str);
char *gets(char *str);
/* ctype.h */
int isspace(char c);
int isdigit(char c);
/* stdlib.h */
void exit(int status);
