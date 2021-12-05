/* C语言标准函数库 limits for KalinoteOS */

#if (!defined(LIMITS_H))

#define LIMITS_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/*
* limits.h 头文件决定了各种变量类型的各种属性。
* 定义在该头文件中的宏限制了各种变量类型（比如 char、int 和 long）的值。
* 这些限制指定了变量不能存储任何超出这些限制的值
 */

#define CHAR_BIT	8					// 定义一个字节(char)的比特数
#define	CHAR_MAX	(+127)              // 定义类型 char 的最大值，如果 char 表示负值，则它的值等于 SCHAR_MAX，否则等于 UCHAR_MAX。
#define CHAR_MIN	0                   // 定义类型 char 的最小值，如果 char 表示负值，则它的值等于 SCHAR_MIN，否则等于 0。
#define INT_MAX		(+0x7fffffff)       // 定义一个整型(int)的最大值。
#define INT_MIN		(-0x7fffffff)       // 定义一个整型(int)的最小值。
#define LONG_MAX    INT_MAX             // 定义一个长整型(long)的最大值。
#define	LONG_MIN	INT_MIN             // 定义一个长整型(long)的最小值。
#define SCHAR_MAX	(+127)              // 定义一个有符号字符(char)的最大值。
#define	SCHAR_MIN	(-127)              // 定义一个有符号字符(char)的最小值。
#define	SHRT_MAX	(+0x7fff)           // 定义一个短整型(short)的最大值。
#define SHRT_MIN	(-0x7fff)           // 定义一个短整型(short)的最小值。
#define UCHAR_MAX	(+0xff)             // 定义一个无符号字符(unsigned char)的最大值。
#define UINT_MAX	(+0xffffffff)       // 定义一个无符号整型(unsigned int)的最大值。
#define ULONG_MAX	UINT_MAX            // 定义一个无符号长整型(unsigned long)的最大值
#define USHRT_MAX	(+0xffff)           // 定义一个无符号短整型(unsigned short)的最大值

#if (defined(__cplusplus))
	}
#endif

#endif
