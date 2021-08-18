/* C语言标准函数库 stdlib.h asctime */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// extern char *str;

char *asctime(const struct tm *timeptr) {
	/* 注意：该函数可用性未知 */
	const char *wday[] = {"Mon", "Tue", "Wed", "Thurs", "Fri", "Sat", "Sun"};
	const char *mon[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
	static char str[50];
	sprintf(str, "%s %s %d %02d:%02d:%02d %d",
		wday[(timeptr->tm_wday >= 0 || timeptr->tm_wday <= 7)?timeptr->tm_wday:0],
		mon[(timeptr->tm_mon >= 0 || timeptr->tm_mon <= 12)?timeptr->tm_mon:0],
		timeptr->tm_mday,
		timeptr->tm_hour,
		timeptr->tm_min,
		timeptr->tm_sec,
		timeptr->tm_year+1900
		);
	return str;
}
