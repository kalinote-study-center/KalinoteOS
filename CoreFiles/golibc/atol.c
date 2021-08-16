/* C语言标准函数库 stdlib.h atol */

#include <stdlib.h>	/* strtod */
#include <stdio.h>	/* NULL */

long atol(const char *s) {
	/* 注意：该函数可用性未知 */
	return strtol (s, (const char **) NULL, 10);
}
