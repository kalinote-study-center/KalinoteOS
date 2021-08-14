/* C语言标准函数库 locale for KalinoteOS */
#if (!defined(LOCALE_H))
#define LOCALE_H			1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* locale.h 头文件定义了特定地域的设置，比如日期格式和货币符号。*/

typedef struct {
   char *decimal_point;     // 用于非货币值的小数点字符。
   char *thousands_sep;     // 用于非货币值的千位分隔符。
   char *grouping;          // 一个表示非货币量中每组数字大小的字符串。每个字符代表一个整数值，每个整数指定当前组的位数。值为 0 意味着前一个值将应用于剩余的分组。
   char *int_curr_symbol;   // 国际货币符号使用的字符串。前三个字符是由 ISO 4217:1987 指定的，第四个字符用于分隔货币符号和货币量。
   char *currency_symbol;   // 用于货币的本地符号。
   char *mon_decimal_point; // 用于货币值的小数点字符。
   char *mon_thousands_sep; // 用于货币值的千位分隔符。
   char *mon_grouping;      // 一个表示货币值中每组数字大小的字符串。每个字符代表一个整数值，每个整数指定当前组的位数。值为 0 意味着前一个值将应用于剩余的分组。
   char *positive_sign;     // 用于正货币值的字符。
   char *negative_sign;     // 用于负货币值的字符。
   char int_frac_digits;    // 国际货币值中小数点后要显示的位数。
   char frac_digits;        // 货币值中小数点后要显示的位数。
   char p_cs_precedes;      // 如果等于 1，则 currency_symbol 出现在正货币值之前。如果等于 0，则 currency_symbol 出现在正货币值之后。
   char p_sep_by_space;     // 如果等于 1，则 currency_symbol 和正货币值之间使用空格分隔。如果等于 0，则 currency_symbol 和正货币值之间不使用空格分隔。
   char n_cs_precedes;      // 如果等于 1，则 currency_symbol 出现在负货币值之前。如果等于 0，则 currency_symbol 出现在负货币值之后。
   char n_sep_by_space;     // 如果等于 1，则 currency_symbol 和负货币值之间使用空格分隔。如果等于 0，则 currency_symbol 和负货币值之间不使用空格分隔。
   char p_sign_posn;        // 表示正货币值中正号的位置。
   char n_sign_posn;        // 表示负货币值中负号的位置。
} lconv

/*
* 下面的值用于 p_sign_posn 和 n_sign_posn:
* 0		封装值和 currency_symbol 的括号。
* 1		放置在值和 currency_symbol 之前的符号。
* 2		放置在值和 currency_symbol 之后的符号。
* 3		紧挨着放置在值和 currency_symbol 之前的符号。
* 4		紧挨着放置在值和 currency_symbol 之后的符号。
*/

/*
* 这些功能后面有空再来实现吧:)
* 因为有亿点复杂:) 
*/

// char *setlocale(int category, const char *locale);		// 设置地域化信息。
// struct lconv *localeconv(void);							// 读取地域化信息。

#if (defined(__cplusplus))
	}
#endif

#endif