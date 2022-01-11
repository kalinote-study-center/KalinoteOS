/* C���Ա�׼���� stdio.h fopen */
#include <stdio.h>
#include <stdlib.h>
#include <apilib.h>

FILE *fopen(const char *name, const char *modes) {
	/* ��һ���ļ� */
	FILE *str = 0;
	size_t i;
	
	for(i = 0; i < FOPEN_MAX; ++i) {
		if(_Files[i] == NULL) {
			/* ��_Files������Ѱ��һ��û�б�ʹ�õ�FILE */
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
	/* TODO����kalinoteOS���ļ����ת����FILE */
	(void)modes;
	return str;
}
