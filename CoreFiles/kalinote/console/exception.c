/* CPU异常中断处理 */
#include "../bootpack.h"
#include <stdio.h>

int *inthandler00(int *esp){
	/* 除零异常 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[20], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg, WARMSG_CH, "除零异常", "栈除零异常通常出现在程序尝试以0做为除数时。");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x00 :\n 除零异常\n");
	} else {
		sprintf(msg, WARMSG_EN, "Zero Division Exception", "The Zero Division Exception usually occurs when a program attempts to divide by 0.");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x00 :\n Zero Division Exception.\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);
}

int *inthandler01(int *esp){
	/* 调试异常 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler02(int *esp){
	/* NMI中断 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler03(int *esp){
	/* 断点(INT 3) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler04(int *esp){
	/* 溢出(INTO) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler05(int *esp){
	/* 数组越界(BOUND) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler06(int *esp){
	/* 无效指令 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

// int *inthandler07(int *esp){
// 	/* 数学协处理器不存在或不可用(WAIT) */
//  /* 该异常已经在FPU相关代码中定义 */
// }

int *inthandler08(int *esp){
	/* 双重错误(Double Fault) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler09(int *esp){
	/* 向协处理器传送操作数时检测到页错误（Page Fault）或段不存在 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler0a(int *esp){
	/* 无效TSS */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler0b(int *esp){
	/* 段不存在 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler0c(int *esp){
	/* 栈异常中断 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[20], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg, WARMSG_CH,"栈异常", "栈异常通常出现在用户程序错误访问数据栈(如数组成员下标越界等)。");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x0C :\n 栈异常\n");
	} else {
		sprintf(msg, WARMSG_EN, "Stack Exception", "stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds).");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0C :\n Stack Exception.\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* 强制结束程序 */
}

int *inthandler0d(int *esp){
	/* CPU异常中断 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[20], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg, WARMSG_CH, "一般保护异常", "一般保护异常通常出现在用户程序企图访问不可访问的地址时。");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x0D :\n 一般保护异常\n");
	} else {
		/* 非中文模式 */
		sprintf(msg, WARMSG_EN, "General protection exception", "General protection exception usually occurs when the user program attempts to access an inaccessible address.");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0D :\n General Protected Exception\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);/* esp11号是EIP，详细esp列表在书上第451页 */
	cons_putstr0(cons, s);
	
	return &(task->tss.esp0);	/* 强制结束程序 */
}

int *inthandler0e(int *esp){
	/* 页错误 */
	/* 暂时没有使用到页，暂时不管。 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

