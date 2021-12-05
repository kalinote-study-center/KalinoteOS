/* C语言标准函数库 string.h memcpy */
#include <stddef.h>

//=============================================================================
// copy SZ bytes of S to D
//=============================================================================
void* memcpy (void *d, void *s, size_t sz)
{
	void *tmp = d;
	char *dp = (char*)d;
	char *sp = (char*)s;

	while (sz--)
		*dp++ = *sp++;

	return tmp;
}
