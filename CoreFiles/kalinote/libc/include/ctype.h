/* C语言标准函数库 ctype for KalinoteOS */

#if (!defined(CTYPE_H))

#define CTYPE_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* golibc */
int isspace(char c);	// 该函数检查所传的字符是否是空字符。
int isdigit(char c);	// 该函数检查所传的字符是否是数字。
int isalnum(int c);		// 该函数检查所传的字符是否是字母和数字。
int isalpha(int c);		// 该函数检查所传的字符是否是字母。
int iscntrl(int c);		// 该函数检查所传的字符是否是控制字符。
int isgraph(int c);		// 该函数检查所传的字符是否有图形表示法。
int islower(int c);		// 该函数检查所传的字符是否是小写字母。
int isprint(int c);		// 该函数检查所传的字符是否是可打印的。
int ispunct(int c);		// 该函数检查所传的字符是否是标点符号字符。
int isupper(int c);		// 该函数检查所传的字符是否是大写字母。
int isxdigit(int c);	// 该函数检查所传的字符是否是十六进制数字。
int tolower(int c);		// 该函数把大写字母转换为小写字母。
int toupper(int c);		// 该函数把小写字母转换为大写字母。

#if (defined(__cplusplus))
	}
#endif

#endif
