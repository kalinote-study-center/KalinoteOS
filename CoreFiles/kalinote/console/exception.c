/* CPU�쳣�жϴ��� */
/*
* ����ļ������¼��(�󲿷�)�ײ�Ӳ���쳣ʱ��(����)����ʽ
* ��Щ����һ��ͨ��nasmfunc.asm�еĻ����������е���
*/
#include "../bootpack.h"
#include <stdio.h>

int *inthandler_divzero(int *esp){
	/* �����쳣 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	if (task->langmode == 1) {
		/* ����ģʽ */
		cons_printf(cons, WARMSG_CH, "�����쳣", "ջ�����쳣ͨ�������ڳ�������0��Ϊ����ʱ��");
	} else {
		cons_printf(cons, WARMSG_EN, "Zero Division Exception", "The Zero Division Exception usually occurs when a program attempts to divide by 0.");
	}
	cons_printf(cons, "EIP = 0x%08X\n", esp[11]);
	return &(task->tss.esp0);
}

int *inthandler_debug(int *esp){
	/* �����쳣 */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_nmi(int *esp){
	/* NMI�ж� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_int3(int *esp){
	/* �ϵ�(INT 3) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_overflow(int *esp){
	/* ���(INTO) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_bounds(int *esp){
	/* ����Խ��(BOUND) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_invalid_op(int *esp){
	/* ��Чָ�� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

// int *inthandler_device_not_available(int *esp){
// 	/* ��ѧЭ�����������ڻ򲻿���(WAIT) */
//  /* ���쳣�Ѿ���FPU��ش����ж��� */
// }

int *inthandler_double_fault(int *esp){
	/* ˫�ش���(Double Fault) */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_coprocessor_segment_overrun(int *esp){
	/* ��Э���������Ͳ�����ʱ��⵽ҳ����Page Fault����β����� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_tss(int *esp){
	/* ��ЧTSS */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_segment_not_present(int *esp){
	/* �β����� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_stack_segment(int *esp){
	/* ջ�쳣�ж� */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	if (task->langmode == 1) {
		/* ����ģʽ */
		cons_printf(cons, WARMSG_CH,"ջ�쳣", "ջ�쳣ͨ���������û���������������ջ(�������Ա�±�Խ���)��");
	} else {
		cons_printf(cons, WARMSG_EN, "Stack Exception", "stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds).");
	}
	cons_printf(cons, "EIP = 0x%08X\n", esp[11]);
	return &(task->tss.esp0);	/* ǿ�ƽ������� */
}

int *inthandler_general_protection(int *esp){
	/* CPU�쳣�ж� */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	if (task->langmode == 1) {
		/* ����ģʽ */
		cons_printf(cons, WARMSG_CH, "һ�㱣���쳣", "һ�㱣���쳣ͨ���������û�������ͼ���ʲ��ɷ��ʵĵ�ַʱ��");
	} else {
		/* ������ģʽ */
		cons_printf(cons, WARMSG_EN, "General protection exception", "General protection exception usually occurs when the user program attempts to access an inaccessible address.");
	}
	cons_printf(cons, "EIP = 0x%08X\n", esp[11]);
	return &(task->tss.esp0);	/* ǿ�ƽ������� */
}

int *inthandler_page_fault(int *esp){
	/* ҳ���� */
	/* ��ʱû��ʹ�õ�ҳ����ʱ���ܡ� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}

int *inthandler_coprocessor_error(int *esp){
	/* Э���������� */
	struct TASK *task = task_now();
	return &(task->tss.esp0);
}
