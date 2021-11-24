/* CPUID */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void cpu_init(void) {
	/* 检测CPU相关信息 */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	unsigned int eax,ebx,ecx,edx;
	
	sysinfo->cpuid_info.cpuid = (check_cpuid()?TRUE:FALSE);
	if (!sysinfo->cpuid_info.cpuid)return;		/* 如果不支持CPUID */
	
	/* 获取CPU厂商信息 */
	eax = read_cpuid(0x00, &ebx, &edx, &ecx);
	sysinfo->cpuid_info.max_cpuid_num = eax;
	memcpy(sysinfo->cpuid_info.oem,&ebx,4);
	memcpy((sysinfo->cpuid_info.oem)+4,&edx,4);
	memcpy((sysinfo->cpuid_info.oem)+8,&ecx,4);
	sysinfo->cpuid_info.oem[12] = 0;			/* 终止符 */
	
	/* 检查是否支持Brand String */
	eax = read_cpuid(0x80000000, &ebx, &edx, &ecx);
	// debug_print("CPUID>Brand String support: %s\n", eax >= 0x80000004? "yes":"no");		// 想什么呢，这个时候debug窗口还没有初始化
	sysinfo->cpuid_info.brandString = ((eax>=0x80000004)?TRUE:FALSE);

	return;
}

int cpu_64_check(void) {
	/* 检测CPU是否支持长模式(IntelCPU,通过CPUID:0x80000002) */
	/* 这里的功能是否正常仍有待检查 */
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	char CPUName[17];
	unsigned int eax,ebx,ecx,edx;
	int i;
	
	if (!sysinfo->cpuid_info.cpuid)return FALSE;											/* 如果不支持CPUID */
	if (!(strncmp(sysinfo->cpuid_info.oem, CPUID_VENDOR_INTEL, 12) == 0))return FALSE;		/* 不是intelCPU(查询方式不同) */
	debug_print("CPUID>64 bit Check\n");
	// eax = read_cpuid(0x80000002, &ebx, &edx, &ecx);
	// debug_print("EAX:0x%08x\n",eax);
	// debug_print("EBX:0x%08x\n",ebx);
	// debug_print("ECX:0x%08x\n",ecx);
	// debug_print("EDX:0x%08x\n",edx);
	if(sysinfo->cpuid_info.brandString){
		/* 如果支持Brand String */
		debug_print("CPUID>");
		for(i = 0x80000002; i <= 0x80000004; i++){
			eax = read_cpuid(i, &ebx, &edx, &ecx);
			*(unsigned int*)&CPUName[0] = eax;
			*(unsigned int*)&CPUName[4] = ebx;
			*(unsigned int*)&CPUName[8] = ecx;
			*(unsigned int*)&CPUName[12] = edx;
			CPUName[16] = '\0';
			debug_print("%s",CPUName);
		}
		debug_print("\n");
	}
	
	/* 获取Family/Model/Stepping等信息 */
	/**
     *  [0:3]    stepping
     *  [4:7]    model
     *  [8:11]    family
     *  [12:13]    processor type
     *  [16:19]    extended model ID
     *  [20:27]    extended family ID
	*/
	// eax = read_cpuid(0x01, &ebx, &edx, &ecx);
    // debug_print("CPUID>Extended Family\t: %d\n", (0xff00000 & eax) >> 20);
    // debug_print("CPUID>Extended Model\t: %d\n", (0xf0000 & eax) >> 16);
    // debug_print("CPUID>Processor type\t: %d\n", (0x3000 & eax) >> 12);
    // debug_print("CPUID>Family\t\t: %d\n", (0xf00 & eax) >> 8);
    // debug_print("CPUID>Model\t\t: %d\n", (0xf0 & eax) >> 4);
    // debug_print("CPUID>Stepping:\t: %d\n", (0xf & eax));

	return FALSE;
}
