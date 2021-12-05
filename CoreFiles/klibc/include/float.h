/* C语言标准函数库 float for KalinoteOS */
#if (!defined(FLOAT_H))
#define FLOAT_H			1

#if (defined(__cplusplus))
	extern "C" {
#endif

/*
* C 标准库的 float.h 头文件包含了一组与浮点值相关的依赖于平台的常量。
* 这些常量是由 ANSI C 提出的，这让程序更具有可移植性。
 */

/*
* 所有的实例 FLT 是指类型 float，DBL 是指类型 double，LDBL 是指类型 long double
*/

#define FLT_RADIX		2
/*
* FLT_RADIX
* 这个宏定义了指数表示的基数。基数 2 表示二进制，基数 10 表示十进制，基数 16 表示十六进制。
*/
#define FLT_ROUNDS 		1
/*
* FLT_ROUNDS
* 定义浮点加法的舍入模式，它可以是下列任何一个值：
* -1	- 无法确定
* 0		- 趋向于零
* 1		- 取最近的值
* 2		- 趋向于正无穷
* 3		- 趋向于负无穷
*/
#define FLT_DIG			6
/*
* FLT_DIG
* 这个宏定义了舍入后不会改变表示的十进制数字的最大值（基数 10）。
*/
#define FLT_EPSILON		1.19209290e-07F
/*
* FLT_EPSILON
* 这个宏定义了可表示的最小有效数字。
*/
#define FLT_MANT_DIG	24
/*
* FLT_MANT_DIG
* 这个宏定义了 FLT_RADIX 基数中的位数。
*/
#define FLT_MAX			3.40282347e+38F
/*
* FLT_MAX
* 这个宏定义最大的有限浮点值。
*/
#define FLT_MAX_EXP		(+128)
/*
* FLT_MAX_EXP
* 这个宏定义了基数为 FLT_RADIX 时的指数的最大整数值。
*/
#define FLT_MIN			1.17549435e-38F
/*
* FLT_MIN
* 这个宏定义了最小的浮点值。
*/
#define FLT_MIN_EXP		(-125)
/*
* FLT_MAX_EXP
* 这个宏定义了基数为 FLT_RADIX 时的指数的最小负整数值。
*/

#define DBL_DIG			15
#define DBL_EPSILON		2.2204460492503131e-16
#define DBL_MANT_DIG	53
#define DBL_MAX			1.7976931348623157e+308
#define DBL_MAX_EXP		1024
#define DBL_MIN			2.2250738585072014e-308
#define DBL_MIN_EXP		(-1021)

#if (defined(__cplusplus))
	}
#endif

#endif

