/* ����ļ���Ŀ¼ */
#include "../bootpack.h"
#include <stdio.h>

void cmd_dir(struct CONSOLE *cons, char *cmdline){
	/* ����ļ���Ŀ¼ */
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[50];
	struct TASK *task = task_now();
	if (task->langmode == 1) {
		cons_putstr0(cons, "�ļ���			����  			 ��С		  ����\n\n");
	} else {
		cons_putstr0(cons, "FILENAME		TYPE  			 SIZE		PROPERTIES\n\n");
	}
	
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			/* û���ļ���Ϣ */
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			/* δ��ɾ�����ļ�(finfo[i].name[0]=0xe5ʱ�����ļ���ɾ��) */
			if ((finfo[i].type & 0xdf) == 0) {
				if (task->langmode == 1) {
					sprintf(s, "filename      ext �ļ�     %7d �ֽ�		��ͨ�ļ�\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		NormalFile\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// �ļ���
					s[j] = finfo[i].name[j];
				}
				/* ��չ�� */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xfe) == 0) {
				/* ֻ���ļ� */
				if (task->langmode == 1) {
					sprintf(s, "filename      ext �ļ�     %7d �ֽ�		ֻ���ļ�\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		 ReadOnly\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// �ļ���
					s[j] = finfo[i].name[j];
				}
				/* ��չ�� */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xfb) == 0) {
				/* ϵͳ�ļ� */
				if (task->langmode == 1) {
					sprintf(s, "filename      ext �ļ�     %7d �ֽ�		ϵͳ�ļ�\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		SystemFile\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// �ļ���
					s[j] = finfo[i].name[j];
				}
				/* ��չ�� */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xef) == 0) {
				/* Ŀ¼ */
				if (task->langmode == 1) {
					sprintf(s, "filename        Ŀ¼       %7d �ֽ�		  Ŀ¼\n", finfo[i].size);
				} else {
					sprintf(s, "filename     Directory     %7d Byte		Directory\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// �ļ���
					s[j] = finfo[i].name[j];
				}
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}
