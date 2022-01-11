/* C语言标准函数库 stdio freopen */
#include <stdio.h>

FILE *freopen(const char *name, const char *mods, FILE *str) {
	unsigned short mode = str->_Mode & _MALFIL;
	
	str->_Mode &= ~_MALFIL;
	fclose(str);
	str->_Mode = mode;
	str->_Handle = api_fopen(name);
	/* TODO：把kalinoteOS的文件句柄转换成FILE */
	(void)modes;
	return str;
}
