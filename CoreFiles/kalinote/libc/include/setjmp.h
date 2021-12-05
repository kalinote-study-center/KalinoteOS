/* C语言标准函数库 setjmp for KalinoteOS */

#if (!defined(SETJMP_H))

#define SETJMP_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/*
* setjmp.h 头文件定义了宏 setjmp()、函数 longjmp() 和变量类型 jmp_buf，
* 该变量类型会绕过正常的函数调用和返回规则。
*/

typedef int jmp_buf[3]; /* EBP, EIP, ESP */

#define setjmp(env)			__builtin_setjmp(env)
#define longjmp(env, val)	__builtin_longjmp(env, val)

#if (defined(__cplusplus))
	}
#endif

#endif
