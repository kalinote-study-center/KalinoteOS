/* 输出系统相关信息 */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void cmd_sysinfo(struct CONSOLE *cons) {
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	if(check_cpuid()) {
		/* 可用CPUID */
		cons_printf(cons, "CPUID available\n");
		unsigned int eax=0x1,ebx=0x1,edx=0x1,ecx=0x1;
		char s[13];
		eax = read_cpuid(0, &ebx, &edx, &ecx);
		memcpy(s,&ebx,4);
		memcpy(s+4,&edx,4);
		memcpy(s+8,&ecx,4);
		s[13] = 0;		/* 终止符 */
		// cons_printf(cons,"EAX:0x%x\n",eax);
		// cons_printf(cons,"EBX:0x%x\n",ebx);
		// cons_printf(cons,"EDX:0x%x\n",edx);
		// cons_printf(cons,"ECX:0x%x\n",ecx);
		cons_printf(cons,"CPU OEM:%s\n",s);
	} else {
		cons_printf(cons, "CPUID not available\n");
	}
	
	return;
}
