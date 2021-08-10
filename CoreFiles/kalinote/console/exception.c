/* CPU异常中断处理 */
#include "../bootpack.h"
#include <stdio.h>

int *inthandler00(int *esp){
	/* 除零异常 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg,"********************除零异常********************\n   系统在尝试运行应用时遇到了错误。\n   栈除零异常通常出现在程序尝试以0做为除数时。如果该错误第一次出现，请尝试重新启动该应用程序，如果该错误反复出现，请联系软件的开发者。\n   下面是此次错误的信息：\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x00 :\n 除零异常\n");
	} else {
		sprintf(msg,"********************Zero Division Exception********************\n   The system encountered an error while trying to run the application. \n    The Zero Division Exception usually occurs when a program attempts to divide by 0. If this error occurs for the first time, please try to restart the application. If this error occurs repeatedly, please contact the software developer. \n the following is the error message:\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x00 :\n Zero Division Exception.\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);
}

int *inthandler0c(int *esp){
	/* 栈异常中断 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg,"********************栈异常********************\n   系统在尝试运行应用时遇到了错误。\n   栈异常通常出现在用户程序错误访问数据栈(如数组成员下标越界等)。如果该错误第一次出现，请尝试重新启动该应用程序，如果该错误反复出现，请联系软件的开发者。\n   下面是此次错误的信息：\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x0C :\n 栈异常\n");
	} else {
		sprintf(msg,"********************Stack Exception********************\n   The system encountered an error while trying to run the application. \n   stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds). If this error occurs for the first time, please try to restart the application. If this error occurs repeatedly, please contact the software developer. \n the following is the error message:\n");
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
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* 中文模式 */
		sprintf(msg,"********************一般保护异常********************\n   系统在尝试运行应用时遇到了错误。\n   一般保护异常通常出现在用户程序企图访问不可访问的地址时。如果该错误第一次出现，请尝试重新启动该应用程序，如果该错误反复出现，请联系软件的开发者。\n   下面是此次错误的信息：\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n中断号 0x0D :\n 一般保护异常\n");
	} else {
		/* 非中文模式 */
		sprintf(msg,"*****************General protection exception*****************\n   The system encountered an error while trying to run the application.\n   General protection exception usually occurs when the user program attempts to access an inaccessible address. If the error occurs for the first time, please try to restart the application. If the error occurs repeatedly, please contact the software developer. \n   the following is the error message:\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0D :\n General Protected Exception\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);/* esp11号是EIP，详细esp列表在书上第451页 */
	cons_putstr0(cons, s);
	
	return &(task->tss.esp0);	/* 强制结束程序 */
}
