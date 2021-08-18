/* C语言标准函数库 time for KalinoteOS  */

#if (!defined(TIME_H))

#define TIME_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#include <stddef.h>

#define CLOCKS_PER_SEC		100

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
typedef unsigned long clock_t;		/* 这是一个适合存储处理器时间的类型。 */

char *asctime(const struct tm *timeptr);				// 返回一个指向字符串的指针，它代表了结构 timeptr 的日期和时间。
clock_t clock(void);									// 返回程序执行起（一般为程序的开头），处理器时钟所使用的时间。
time_t time(time_t *seconds);							// 计算当前日历时间，并把它编码成 time_t 格式
char *ctime(const time_t *timer);						// 返回一个表示当地时间的字符串，当地时间是基于参数 timer。
double difftime(time_t time1, time_t time2);			// 返回 time1 和 time2 之间相差的秒数 (time1-time2)。

#if (defined(__cplusplus))
	}
#endif

#endif
