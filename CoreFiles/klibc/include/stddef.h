/* C语言标准函数库 stddef for KalinoteOS */

#if (!defined(STDDEF_H))

#define STDDEF_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* stddef .h 头文件定义了各种变量类型和宏。这些定义中的大部分也出现在其它头文件中。 */

#if (!defined(NULL))
	#define NULL	((void *) 0)
#endif
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER);
/*
* offsetof
* 这会生成一个类型为 size_t 的整型常量，它是一个结构成员相对于结构开头的字节偏移量。
* 成员是由 member-designator 给定的，结构的名称是在 type 中给定的。
*/

typedef signed int ptrdiff_t;    // 这是有符号整数类型，它是两个指针相减的结果。
typedef unsigned int size_t;     // 这是无符号整数类型，它是 sizeof 关键字的结果。
typedef unsigned short wchar_t;  // 这是一个宽字符常量大小的整数类型。

/* 这个头文件里面包含了部分应该在yvals.h的头文件里的内容，但是网上找不到该头文件相关信息 */
/* 以后有需要再把这些内容单独放在yvals.h中 */
/* stdio */
#define _NULL	(void *)0
#define _FNAMAX 8		// 宏FILE_NAME_MAX的值(由于FAT12文件系统限制)
#define _FOPMAX 16		// 宏FOPEN_MAX的值
#define _TNAMAX 8		// 宏L_tmpnam的值


#if (defined(__cplusplus))
	}
#endif

#endif
