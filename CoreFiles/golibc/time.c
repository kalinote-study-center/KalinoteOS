/* C语言标准函数库 stdlib.h time */
#include <time.h>
#include <../apilib.h>

time_t time(time_t *seconds) {
	/* 注意：该函数证实可用 */
	unsigned int year = api_get_year();
	unsigned int mon = api_get_mon_hex();
	time_t total_sec = 0;
    if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
         mon += 12;      /* Puts Feb last since it has leap day */
         year -= 1;
    }
    total_sec =  (((
             (time_t) (year/4 - year/100 + year/400 + 367*mon/12 + api_get_day_of_month()) +
             year*365 - 719499
          )*24 + api_get_hour_hex() /* now have hours */
       )*60 + api_get_min_hex() /* now have minutes */
    )*60 + api_get_sec_hex(); /* finally seconds */
	if(seconds != NULL){*seconds=total_sec;};
	return total_sec;
}
