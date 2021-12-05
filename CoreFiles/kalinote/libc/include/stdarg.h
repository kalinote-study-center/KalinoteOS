/* C语言标准函数库 stdarg for KalinoteOS */

#if (!defined(STDARG_H))

#define STDARG_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/*
* stdarg.h 头文件定义了一个变量类型 va_list 和四个宏，
* 这三个宏可用于在参数个数未知（即参数个数可变）时获取函数中的参数。
*/
/* 可变参数的函数通在参数列表的末尾是使用省略号(,...)定义的。 */

#define va_start(v,l)	__builtin_stdarg_start((v),l)
#define va_end			__builtin_va_end
#define va_arg			__builtin_va_arg
#define va_copy(d,s)	__builtin_va_copy((d),(s))
#define	va_list			__builtin_va_list

#if (defined(__cplusplus))
	}
#endif

#endif
