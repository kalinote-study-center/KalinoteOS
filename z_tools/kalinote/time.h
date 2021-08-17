/* C语言标准函数库 time for KalinoteOS  */

#if (!defined(TIME_H))

#define TIME_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#include <stddef.h>

struct tm {				/* 这是一个用来保存时间和日期的结构。 */
   int tm_sec;  		/* 秒，范围从 0 到 59        */
   int tm_min;  		/* 分，范围从 0 到 59        */
   int tm_hour; 		/* 小时，范围从 0 到 23        */
   int tm_mday; 		/* 一月中的第几天，范围从 1 到 31    */
   int tm_mon;  		/* 月，范围从 0 到 11        */
   int tm_year; 		/* 自 1900 年起的年数        */
   int tm_wday; 		/* 一周中的第几天，范围从 0 到 6    */
   int tm_yday; 		/* 一年中的第几天，范围从 0 到 365    */
   int tm_isdst;		/* 夏令时                */
};
typedef unsigned long time_t;		/* 这是一个适合存储日历时间类型。 */

char *asctime(const struct tm *timeptr);				// 返回一个指向字符串的指针，它代表了结构 timeptr 的日期和时间。

#if (defined(__cplusplus))
	}
#endif

#endif
