/* C语言标准函数库 stdlib.h atof */

#include <stdlib.h>	/* strtol */
#include <stdio.h>	/* NULL */

int atoi(const char *s){
	/* 注意：该函数可用性未知 */
	return (int) strtol(s, (const char **) NULL, 10);
}
