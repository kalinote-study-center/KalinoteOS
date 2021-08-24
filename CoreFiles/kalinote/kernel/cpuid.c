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

int cpu_64_check(void) {
	/* 检测CPU是否支持长模式(IntelCPU,通过CPUID:0x80000002) */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	int eax,ebx,ecx,edx;
	
	if (!sysinfo->cpuid_info.cpuid)return FALSE;											/* 如果不支持CPUID */
	if (!(strncmp(sysinfo->cpuid_info.oem, CPUID_VENDOR_INTEL, 12) == 0))return FALSE;		/* 不是intelCPU(查询方式不同) */
	debug_print("CPUID>64 bit Check\n");
	eax = read_cpuid(0x80000002, &ebx, &edx, &ecx);
	debug_print("EAX:0x%08x\n",eax);
	debug_print("EBX:0x%08x\n",ebx);
	debug_print("ECX:0x%08x\n",ecx);
	debug_print("EDX:0x%08x\n",edx);
	
	return FALSE;
}
