/* 输出文件及目录 */
#include "../bootpack.h"
#include <stdio.h>

void cmd_dir(struct CONSOLE *cons, char *cmdline){
	/* 输出文件及目录 */
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[50];
	struct TASK *task = task_now();
	if (task->langmode == 1) {
		cons_putstr0(cons, "文件名			类型  			 大小		  属性\n\n");
	} else {
		cons_putstr0(cons, "FILENAME		TYPE  			 SIZE		PROPERTIES\n\n");
	}
	
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			/* 没有文件信息 */
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			/* 未被删除的文件(finfo[i].name[0]=0xe5时代表文件被删除) */
			if ((finfo[i].type & 0xdf) == 0) {
				if (task->langmode == 1) {
					sprintf(s, "filename      ext 文件     %7d 字节		普通文件\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		NormalFile\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				/* 扩展名 */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xfe) == 0) {
				/* 只读文件 */
				if (task->langmode == 1) {
					sprintf(s, "filename      ext 文件     %7d 字节		只读文件\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		 ReadOnly\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				/* 扩展名 */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xfb) == 0) {
				/* 系统文件 */
				if (task->langmode == 1) {
					sprintf(s, "filename      ext 文件     %7d 字节		系统文件\n", finfo[i].size);
				} else {
					sprintf(s, "filename      ext file     %7d Byte		SystemFile\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				/* 扩展名 */
				s[14] = finfo[i].ext[0];
				s[15] = finfo[i].ext[1];
				s[16] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			} else if ((finfo[i].type & 0xef) == 0) {
				/* 目录 */
				if (task->langmode == 1) {
					sprintf(s, "filename        目录       %7d 字节		  目录\n", finfo[i].size);
				} else {
					sprintf(s, "filename     Directory     %7d Byte		Directory\n", finfo[i].size);
				}
				for (j = 0; j < 8; j++) {
					// 文件名
					s[j] = finfo[i].name[j];
				}
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}
