/* ����ļ���Ŀ¼ */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void print_file(struct CONSOLE *cons, struct FILEINFO *finfo);		// ��ӡĳ��Ŀ¼

void cmd_dir(struct CONSOLE *cons, char *parameter, int *fat){
	/* ����ļ���Ŀ¼ */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *root_finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);		/* ��Ŀ¼��Ϣ */
	struct FILEINFO *subdir_finfo;		/* ��Ŀ¼�ڸ�Ŀ¼�е���Ϣ */
	struct FILEINFO *sub_finfo;			/* ��Ŀ¼Ŀ¼��Ϣ */
	char *subdirinfo_memory;
	
	/* �ȴ������ */
	if(parameter[0]=='/'){
		/* ����·�����ȶ���Ŀ¼�����������Ľ� */
		cons_printf(cons, "����·������:%s\n", parameter);
		print_file(cons, root_finfo);
	} else {
		cons_printf(cons, "���·������:%s\n", parameter);
		// cons_printf(cons, "len:%d\n",strlen(parameter));
		if(parameter[0]=='.'){
			/* ��ǰĿ¼���ȶ���Ŀ¼�����������Ľ� */
			print_file(cons, root_finfo);
		} else {
			/* ��ʱ�Ȳ�����·��������취����Ŀ¼�ļ������� */
			subdir_finfo = dir_search(parameter, root_finfo, 224);
			if(subdir_finfo != 0) {
				/* Ŀ¼���� */
				if((subdir_finfo->type & 0xef) == 0){
					/* ��Ŀ¼ */
					subdirinfo_memory = (char *) memman_alloc_4k(memman, 512);
					file_loadfile(subdir_finfo->clustno, 512, subdirinfo_memory, fat, (char *) (ADR_DISKIMG + 0x003e00));
					sub_finfo = (struct FILEINFO *)subdirinfo_memory;
					print_file(cons, sub_finfo);
					memman_free_4k(memman, (int)subdirinfo_memory,512);
				} else {
					/* �ļ����ڣ�������Ŀ¼ */
					cons_printf(cons, "%s Not a directory.", parameter);
				}
			} else {
				/* Ŀ¼������ */
				cons_printf(cons, "%s Not exist.", parameter);
			}
		}
	}
	
	cons_newline(cons);
	return;
}

void print_file(struct CONSOLE *cons, struct FILEINFO *finfo){
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
	return;
}
