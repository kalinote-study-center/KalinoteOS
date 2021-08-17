/* C语言标准函数库 stdlib.h bsearch */
#include <stddef.h>		/* size_t */
#include <stdlib.h>

void *bsearch(const void *key, const void *base0, size_t nitems,
	size_t size, int (*compar)(const void *, const void *)) {
	/* 注意：该函数可用性未知 */
	
	const char *base = (const char *) base0;
	int lim, cmp;
	const void *p;

	for (lim = nitems; lim != 0; lim >>= 1) {
		p = base + (lim >> 1) * size;
		cmp = (*compar)(key, p);
		if (cmp == 0)
			return (void *)p;
		if (cmp > 0) {	/* key > p: move right */
			base = (const char *)p + size;
			lim--;
		} /* else move left */
	}
	return (NULL);
}
