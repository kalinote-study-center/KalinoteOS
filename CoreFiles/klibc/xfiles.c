/* C语言标准函数 stdio.h Files数据类型对象文件 */
#include <stdio.h>

/* standard error buffer */
static unsigned char ebuf[80];
/* standard streams */
static FILE stdin = {
	_MOPENR, 0,
	NULL, NULL, &stdin._Cbuf,
	&stdin._Cbuf, NULL, &stdin._Cbuf,
	{0}, 0, 0, NULL
};
static FILE stdout = {
	_MOPENW, 1,
	NULL, NULL, &stdout._Cbuf,
	&stdout._Cbuf, NULL, &stdout._Cbuf,
	{0}, 0, 0, NULL
};
static FILE stderr = {
	_MOPENW|_MNBF, 2,
	ebuf, ebuf + sizeof(ebuf), ebuf,
	ebuf, NULL, ebuf,
	{0}, 0, 0, NULL
};
FILE *_Files[FOPEN_MAX] = {&stdin, &stdout, &stderr};
