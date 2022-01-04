/* 系统时钟任务 */
#include "../bootpack.h"
#include <stdio.h>

void clock_task(struct SHEET *sht) {
	// /* 时钟任务主功能(任务运行主体) */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	struct TASK *task = task_now();
	struct TIMER *timer;
	int i;
	char s[9];
	
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 1);
	
	for(;;) {
		io_cli();
		if(fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();

			if(i == 1) {
				sysinfo->datetime.sec += 1;
				if(sysinfo->datetime.sec == 60) {
					/* 分钟进一 */
					sysinfo->datetime.sec = 0;
					sysinfo->datetime.min += 1;
				}
				if(sysinfo->datetime.min == 60) {
					/* 小时进一 */
					sysinfo->datetime.min = 0;
					sysinfo->datetime.hour += 1;
				}
				if(sysinfo->datetime.sec % 10 == 0) {
					/* 每10秒重新查询cmos进行校准 */
					sysinfo->datetime.year = get_year();
					sysinfo->datetime.month = get_mon_hex();
					sysinfo->datetime.day = get_day_of_month();
					sysinfo->datetime.hour = get_hour_hex();
					sysinfo->datetime.min = get_min_hex();
					sysinfo->datetime.sec = get_sec_hex();
				}
				sprintf(s,"%02d:%02d:%02d",sysinfo->datetime.hour, sysinfo->datetime.min, sysinfo->datetime.sec);
				putfonts8_asc_sht(sht, sht->bxsize - 70, sht->bysize - 20, COL_BLACK, COL_BGREY, s, 8);
				sheet_refresh(sht, sht->bxsize - 70, sht->bysize - 20, sht->bxsize - 70 + 8 * 8, sht->bysize - 50 + 16);		/* 暂时不显示日期了 */
				timer_settime(timer, 100);
			}
		}
	}
}

void clock_taskinit(struct TASK *task, struct SHEET *sht) {
	/* 初始化时钟任务 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int *clock_fifo = (int *)memman_alloc_4k(memman, 32 * 4);
	
	task->cons_stack = memman_alloc_4k(memman, 64 * 4);
	task->tss.eip = (int) &clock_task;
	task->tss.esp = task->cons_stack + 64 * 4;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	
	*((int *) (task->tss.esp + 4)) = (int) sht;
	
	fifo32_init(&task->fifo, 32, clock_fifo, task);
	
	task_run(task, 1, 1);		/* TODO：这里运行起来后console就没办法显示内容了，不知道为什么 */
	return;
}

void rtc_syn(void) {
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	
	sysinfo->datetime.year = get_year();
	sysinfo->datetime.month = get_mon_hex();
	sysinfo->datetime.day = get_day_of_month();	
	sysinfo->datetime.hour = get_hour_hex();	
	sysinfo->datetime.min = get_min_hex();	
	sysinfo->datetime.sec = get_sec_hex();	
	return;
}
