/* C���Ա�׼������ stdio freopen */
#include <stdio.h>

FILE *freopen(const char *name, const char *mods, FILE *str) {
	unsigned short mode = str->_Mode & _MALFIL;
	
	str->_Mode &= ~_MALFIL;
	fclose(str);
	str->_Mode = mode;
	str->_Handle = api_fopen(name);
	/* TODO����kalinoteOS���ļ����ת����FILE */
	(void)modes;
	return str;
}
