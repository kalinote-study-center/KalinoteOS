/* C语言标准函数 stdlib.h atexit */
#include <stdlib.h>

int atexit(void (*function)(void)) {
	/* 注意：该函数可用性未知 */
	/* 注意：该函数执行可能没有效果 */
	if (_atexit.count >= ATEXIT_MAX)
		return 1;
	
	int i;
	
	for (i = 0; i < _atexit.count; ++i)
		if (_atexit.functions[i] == function)
			return 0;
	_atexit.functions[_atexit.count++] = function;
	return 0;
}
