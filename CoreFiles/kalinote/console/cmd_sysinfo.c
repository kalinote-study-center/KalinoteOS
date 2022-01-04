/* 输出系统相关信息 */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void cmd_sysinfo(struct CONSOLE *cons, unsigned int memtotal) {
	// struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);

	/* 打印CPU信息 */
	cons_printf(cons, "CPU info:\n");
	if(sysinfo->cpuid_info.cpuid) {
		/* 可用CPUID */
		cons_col_putstr0(cons, "CPUID available\n", COL_DGREEN, COL_WHITE);
		cons_printf(cons,"MAX CPUID number:\t%d\n",sysinfo->cpuid_info.max_cpuid_num);
		cons_printf(cons,"CPU OEM:%s\n",sysinfo->cpuid_info.oem);
		cons_printf(cons,"Brand String support: %s\n", sysinfo->cpuid_info.brandString?"yes":"no");
		if(sysinfo->cpuid_info.brandString){
			/* 如果可以使用Brand String */
			cons_printf(cons,"CPU Brand String:%s\n",sysinfo->cpuid_info.CPUName);
		}
		cpu_64_check();
	} else {
		/* CPUID指令用不了 */
		cons_printf(cons, "CPUID not available\n");
	}
	
	/* 打印Disk信息 */
	// cons_printf(cons, "Disk info:\n");
	// cons_printf(cons, "Disk Number:\t%d\n", sysinfo->ide_hd_num);

	/* 打印内存信息 */
	// cmd_mem(cons,memtotal);
	// cons_printf(cons, "Menory info:\n");
	// cons_printf(cons, "total    %dMB\nfree  %dKB\nsystem   %dKB\n", memtotal / (1024 * 1024), memman_total(memman) / 1024, SYS_MEMORY / 1024);

	/* 打印系统版本号 */
	// cons_printf(cons, "OS info:\n");
	// cons_printf(cons, "SysMode:\t\t%d\n", sysinfo->sysmode);
	// cons_printf(cons, "Main ver:\t\t%d\n", sysinfo->sysmmainver);
	// cons_printf(cons, "Sub ver:\t\t%d\n", sysinfo->sysver);

	return;
}
