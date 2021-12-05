/* C语言标准函数库 stdlib.h div */
#include <stdlib.h>

div_t div (int numer, int denom){
	/* 注意：该函数可用性未知 */
	div_t result;
	result.quot = numer / denom;
	result.rem = numer % denom;
	return result;
}
