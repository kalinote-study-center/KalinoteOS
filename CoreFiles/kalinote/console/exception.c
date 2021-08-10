/* CPU�쳣�жϴ��� */
#include "../bootpack.h"
#include <stdio.h>

int *inthandler00(int *esp){
	/* �����쳣 */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg,"********************�����쳣********************\n   ϵͳ�ڳ�������Ӧ��ʱ�����˴���\n   ջ�����쳣ͨ�������ڳ�������0��Ϊ����ʱ������ô����һ�γ��֣��볢������������Ӧ�ó�������ô��󷴸����֣�����ϵ����Ŀ����ߡ�\n   �����Ǵ˴δ������Ϣ��\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x00 :\n �����쳣\n");
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
	/* ջ�쳣�ж� */
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg,"********************ջ�쳣********************\n   ϵͳ�ڳ�������Ӧ��ʱ�����˴���\n   ջ�쳣ͨ���������û���������������ջ(�������Ա�±�Խ���)������ô����һ�γ��֣��볢������������Ӧ�ó�������ô��󷴸����֣�����ϵ����Ŀ����ߡ�\n   �����Ǵ˴δ������Ϣ��\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x0C :\n ջ�쳣\n");
	} else {
		sprintf(msg,"********************Stack Exception********************\n   The system encountered an error while trying to run the application. \n   stack exception usually occurs when the user program accesses the data stack by mistake (such as the subscript of array member is out of bounds). If this error occurs for the first time, please try to restart the application. If this error occurs repeatedly, please contact the software developer. \n the following is the error message:\n");
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
	char s[30], msg[512];
	if (task->langmode == 1) {
		/* ����ģʽ */
		sprintf(msg,"********************һ�㱣���쳣********************\n   ϵͳ�ڳ�������Ӧ��ʱ�����˴���\n   һ�㱣���쳣ͨ���������û�������ͼ���ʲ��ɷ��ʵĵ�ַʱ������ô����һ�γ��֣��볢������������Ӧ�ó�������ô��󷴸����֣�����ϵ����Ŀ����ߡ�\n   �����Ǵ˴δ������Ϣ��\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\n�жϺ� 0x0D :\n һ�㱣���쳣\n");
	} else {
		/* ������ģʽ */
		sprintf(msg,"*****************General protection exception*****************\n   The system encountered an error while trying to run the application.\n   General protection exception usually occurs when the user program attempts to access an inaccessible address. If the error occurs for the first time, please try to restart the application. If the error occurs repeatedly, please contact the software developer. \n   the following is the error message:\n");
		cons_putstr0(cons, msg);
		cons_putstr0(cons, "\nINT 0x0D :\n General Protected Exception\n");
	}
	sprintf(s, "EIP = 0x%08X\n", esp[11]);/* esp11����EIP����ϸesp�б������ϵ�451ҳ */
	cons_putstr0(cons, s);
	
	return &(task->tss.esp0);	/* ǿ�ƽ������� */
}
