/* C语言标准函数 stdio close */
#include <stdio.h>
#include <apilib.h>

int fclose(FILE *str) {
	/* TODO：这个函数需要先把系统调用进一步完善以后再做 */
	api_fclose(str->_Handle);
	return 0;
}
