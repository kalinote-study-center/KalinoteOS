/* C语言标准函数 stdio.h fopen */
#include <stdio.h>
#include <stdlib.h>
#include <apilib.h>

FILE *fopen(const char *name, const char *modes) {
	/* 打开一个文件 */
	FILE *str = 0;
	size_t i;
	
	for(i = 0; i < FOPEN_MAX; ++i) {
		if(_Files[i] == NULL) {
			/* 从_Files数组中寻找一个没有被使用的FILE */
			str = (FILE *)malloc(sizeof(FILE));
			if(str == NULL)
				return NULL;
			_Files[i] = str;
			str->_Mode = _MALFIL;
			str->_Handle = api_fopen(name);
			break;
		} else if (_Files[i]->_Mode == 0) {
			str = _Files[i];
			break;
		}
	}
	if (i > FOPEN_MAX)
		return NULL;
	// api_fopen(name)
	/* TODO：把kalinoteOS的文件句柄转换成FILE */
	(void)modes;
	return str;
}
