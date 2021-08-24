/* 输出系统相关信息 */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void cmd_sysinfo(struct CONSOLE *cons) {
	// struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	if(sysinfo->cpuid_info.cpuid) {
		/* 可用CPUID */
		cons_printf(cons, "CPUID available\n");
		cons_printf(cons,"MAX CPUID number:%d\n",sysinfo->cpuid_info.max_cpuid_num);
		cons_printf(cons,"CPU OEM:%s\n",sysinfo->cpuid_info.oem);
	} else {
		/* CPUID指令用不了 */
		cons_printf(cons, "CPUID not available\n");
	}
	
	return;
}
