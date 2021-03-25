/* string.h */
#include "../apilib.h"
#include "../stdlib.h"

int strchr(const char *str, char c){
	for (; *str; str++)
		if (*str == c)
			return 1;
	return 0;
}
