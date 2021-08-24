/* 输出系统相关信息 */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void cmd_sysinfo(struct CONSOLE *cons) {
	// struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	if(check_cpuid()) {
		/* 可用CPUID */
		cons_printf(cons, "CPUID available\n");
		int eax=0,ebx=0,edx=0,ecx=0;
		
		/* id:0 获取CPU供应商ID */
		char oem[12];
		eax = read_cpuid(0x00, &ebx, &edx, &ecx);
		memcpy(oem,&ebx,4);
		memcpy(oem+4,&edx,4);
		memcpy(oem+8,&ecx,4);
		oem[12] = 0;		/* 终止符 */
		cons_printf(cons,"MAX CPUID number:%d\n",eax);
		cons_printf(cons,"CPU OEM:%s\n",oem);
		if(!(strcmp(oem, CPUID_VENDOR_INTEL) == 0)) {
			/* 先不管非intel的CPU */
			cons_printf(cons,"Not Intel CPU!\n",oem);
			return;
		}
		
		/* id:1 CPU签名和功能位 */
		eax=0;ebx=0;edx=0;ecx=0;						/* 把变量重置一下，虽然不重置应该也没什么问题，但是重置了总归安心点:) */
		eax = read_cpuid(0x01, &ebx, &edx, &ecx);
		unsigned char logic_p_num = (ebx>>16)&0xff;	/* 逻辑处理器数量 */
		cons_printf(cons,"Logic processor number:%d\n",logic_p_num);
		
	} else {
		/* CPUID指令用不了 */
		cons_printf(cons, "CPUID not available\n");
	}
	
	return;
}
