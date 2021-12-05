/* C语言标准函数库 stdlib.h labs */
#include <stdlib.h>

long labs(long int x) {
	if (x < 0) return -x;
	return x;
}
