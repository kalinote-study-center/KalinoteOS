/* C语言标准函数库 ctype */

#if (!defined(CTYPE_H))

#define CTYPE_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* golibc */
int isspace(char c);
int isdigit(char c);

#if (defined(__cplusplus))
	}
#endif

#endif
