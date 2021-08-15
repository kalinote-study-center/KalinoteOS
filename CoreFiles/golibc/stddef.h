/* C语言标准函数库 stddef for KalinoteOS */

#if (!defined(STDDEF_H))

#define STDDEF_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* stddef .h 头文件定义了各种变量类型和宏。这些定义中的大部分也出现在其它头文件中。 */

#define NULL	((void *) 0)
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER);
/*
* offsetof
* 这会生成一个类型为 size_t 的整型常量，它是一个结构成员相对于结构开头的字节偏移量。
* 成员是由 member-designator 给定的，结构的名称是在 type 中给定的。
*/

typedef signed int ptrdiff_t;    // 这是有符号整数类型，它是两个指针相减的结果。
typedef unsigned int size_t;     // 这是无符号整数类型，它是 sizeof 关键字的结果。
typedef unsigned short wchar_t;  // 这是一个宽字符常量大小的整数类型。

#if (defined(__cplusplus))
	}
#endif

#endif
