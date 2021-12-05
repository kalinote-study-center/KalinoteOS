/* C语言标准函数库 stdlib.h ldiv */
#include <stdlib.h>

ldiv_t ldiv (long numer, long denom){
	/* 注意：该函数可用性未知 */
	ldiv_t result;
	result.quot = numer / denom;
	result.rem = numer % denom;
	return result;
}
