/* CPUID */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void cpu_init(void) {
	/* 检测CPU相关信息 */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	int eax,ebx,ecx,edx;
	
	sysinfo->cpuid_info.cpuid = (check_cpuid()?TRUE:FALSE);
	if (!sysinfo->cpuid_info.cpuid)return;		/* 如果不支持CPUID */
	
	eax = read_cpuid(0x00, &ebx, &edx, &ecx);
	sysinfo->cpuid_info.max_cpuid_num = eax;
	memcpy(sysinfo->cpuid_info.oem,&ebx,4);
	memcpy((sysinfo->cpuid_info.oem)+4,&edx,4);
	memcpy((sysinfo->cpuid_info.oem)+8,&ecx,4);
	sysinfo->cpuid_info.oem[12] = 0;			/* 终止符 */
	
	return;
}
