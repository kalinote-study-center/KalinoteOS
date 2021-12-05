/* C语言标准函数库 stdlib.h atof */

#include <stdlib.h>	/* strtod */
#include <stdio.h>	/* NULL */

double atof(const char *s) {
	/* 注意：该函数可用性未知 */
	return strtod(s, (const char **) NULL);
}
