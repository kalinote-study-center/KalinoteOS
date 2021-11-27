/* CPU�쳣�жϴ��� */
#include "../bootpack.h"
#include <stdio.h>

char WarningMsgCH[300] = "********************%s********************\n   ϵͳ�ڳ�������Ӧ��ʱ�����˴���\n   %s����ô����һ�γ��֣��볢������������Ӧ�ó�������ô��󷴸����֣�����ϵ����Ŀ����ߡ�\n   �����Ǵ˴δ������Ϣ��\n";
char WarningMsgEN[350] = "********************%s********************\n   The system encountered an error while trying to run the application. \n    %s If this error occurs for the first time, please try to restart the application. If this error occurs repeatedly, please contact the software developer. \n the following is the error message:\n";

int *inthandler00(int *esp){
	/* �����쳣 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[20], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg, WarningMsgCH, "�����쳣", "ջ�����쳣ͨ�������ڳ�������0��Ϊ����ʱ��");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x00 :\n �����쳣\n");
	} else {
		sprintf(msg, WarningMsgEN, "Zero Division Exception", "The Zero Division Exception usually occurs when a program attempts to divide by 0.");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x00 :\n Zero Division Exception.\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);
}

int *inthandler01(int *esp){
	/* �����쳣 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler02(int *esp){
	/* NMI�ж� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler03(int *esp){
	/* �ϵ�(INT 3) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler04(int *esp){
	/* ���(INTO) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler05(int *esp){
	/* ����Խ��(BOUND) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler06(int *esp){
	/* ��Чָ�� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

// int *inthandler07(int *esp){
// 	/* ��ѧЭ�����������ڻ򲻿���(WAIT) */
//  /* ���쳣�Ѿ���FPU��ش����ж��� */
// }

int *inthandler08(int *esp){
	/* ˫�ش���(Double Fault) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler09(int *esp){
	/* ��Э���������Ͳ�����ʱ��⵽ҳ����Page Fault����β����� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler0a(int *esp){
	/* ��ЧTSS */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler0b(int *esp){
	/* �β����� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler0c(int *esp){
	/* ջ�쳣�ж� */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[20], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg, WarningMsgCH,"ջ�쳣", "ջ�쳣ͨ���������û���������������ջ(�������Ա�±�Խ���)��");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x0C :\n ջ�쳣\n");
	} else {
		sprintf(msg, WarningMsgEN, "Stack Exception", "stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds).");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0C :\n Stack Exception.\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* ǿ�ƽ������� */
}

int *inthandler0d(int *esp){
	/* CPU�쳣�ж� */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[20], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg, WarningMsgCH, "һ�㱣���쳣", "һ�㱣���쳣ͨ���������û�������ͼ���ʲ��ɷ��ʵĵ�ַʱ��");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x0D :\n һ�㱣���쳣\n");
	} else {
		/* ������ģʽ */
		sprintf(msg, WarningMsgEN, "General protection exception", "General protection exception usually occurs when the user program attempts to access an inaccessible address.");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0D :\n General Protected Exception\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);/* esp11����EIP����ϸesp�б������ϵ�451ҳ */
	cons_putstr0(cons, s);
	
	return &(task->tss.esp0);	/* ǿ�ƽ������� */
}

int *inthandler0e(int *esp){
	/* ҳ���� */
	/* ��ʱû��ʹ�õ�ҳ����ʱ���ܡ� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

