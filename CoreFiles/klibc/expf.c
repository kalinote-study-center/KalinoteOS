/* C语言标准函数库 expf */
#include <math.h>

float expf(float x) {
	/* e^x */
	return((float)pow(M_E, (double)x));
}
