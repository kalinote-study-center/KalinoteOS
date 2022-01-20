/* CPU异常中断处理 */
/*
* 这个文件里面记录了(大部分)底层硬件异常时的(部分)处理方式
* 这些函数一般通过nasmfunc.asm中的汇编代码来进行调用
*/
#include "../bootpack.h"
#include <stdio.h>

int *inthandler_divzero(int *esp){
	/* 除零异常 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	if (task->langmode == 1) {
		/* 中文模式 */
		cons_printf(cons, WARMSG_CH, "除零异常", "栈除零异常通常出现在程序尝试以0做为除数时。");
	} else {
		cons_printf(cons, WARMSG_EN, "Zero Division Exception", "The Zero Division Exception usually occurs when a program attempts to divide by 0.");
	}
	cons_printf(cons, "EIP = 0x%08X\n", esp[11]);
	return &(task->tss.esp0);
}

int *inthandler_debug(int *esp){
	/* 调试异常 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_nmi(int *esp){
	/* NMI中断 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_int3(int *esp){
	/* 断点(INT 3) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_overflow(int *esp){
	/* 溢出(INTO) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_bounds(int *esp){
	/* 数组越界(BOUND) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_invalid_op(int *esp){
	/* 无效指令 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

// int *inthandler_device_not_available(int *esp){
// 	/* 数学协处理器不存在或不可用(WAIT) */
//  /* 该异常已经在FPU相关代码中定义 */
// }

int *inthandler_double_fault(int *esp){
	/* 双重错误(Double Fault) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_coprocessor_segment_overrun(int *esp){
	/* 向协处理器传送操作数时检测到页错误（Page Fault）或段不存在 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_tss(int *esp){
	/* 无效TSS */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_segment_not_present(int *esp){
	/* 段不存在 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_stack_segment(int *esp){
	/* 栈异常中断 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	if (task->langmode == 1) {
		/* 中文模式 */
		cons_printf(cons, WARMSG_CH,"栈异常", "栈异常通常出现在用户程序错误访问数据栈(如数组成员下标越界等)。");
	} else {
		cons_printf(cons, WARMSG_EN, "Stack Exception", "stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds).");
	}
	cons_printf(cons, "EIP = 0x%08X\n", esp[11]);
	return &(task->tss.esp0);	/* 强制结束程序 */
}

int *inthandler_general_protection(int *esp){
	/* CPU异常中断 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	if (task->langmode == 1) {
		/* 中文模式 */
		cons_printf(cons, WARMSG_CH, "一般保护异常", "一般保护异常通常出现在用户程序企图访问不可访问的地址时。");
	} else {
		/* 非中文模式 */
		cons_printf(cons, WARMSG_EN, "General protection exception", "General protection exception usually occurs when the user program attempts to access an inaccessible address.");
	}
	cons_printf(cons, "EIP = 0x%08X\n", esp[11]);
	return &(task->tss.esp0);	/* 强制结束程序 */
}

int *inthandler_page_fault(int *esp){
	/* 页错误 */
	/* 暂时没有使用到页，暂时不管。 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_coprocessor_error(int *esp){
	/* 协处理器错误 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}
