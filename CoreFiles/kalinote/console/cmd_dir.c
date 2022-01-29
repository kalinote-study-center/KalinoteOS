/* 输出文件及目录 */
#include <bootpack.h>
#include <stdio.h>
#include <string.h>

void print_files(struct CONSOLE *cons, struct FILEINFO *finfo);		// 打印某个目录

void cmd_dir(struct CONSOLE *cons, char *parameter, int *fat){
	/* 输出文件及目录 */
	struct TASK *task = task_now();
	struct FILEINFO *root_finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);		/* 根目录信息 */
	struct FILEINFO *sub_finfo;			/* 子目录目录信息 */
	char apath[350];

	/* 先处理参数 */
	if(parameter[0]=='/'){
		/* 绝对路径，先读根目录，后面再做改进 */
		cons_printf(cons, "绝对路径参数:%s\n", parameter);
		if(strcmp(parameter, "/") == 0) {
			/* 只有一个根目录，直接读根目录 */
			print_files(cons, root_finfo);
		} else {
			/* 非根目录的绝对路径 */
			sub_finfo = dir_check(parameter,fat);
			if(sub_finfo==0){
				cons_printf(cons, "目录不存在：%s",parameter);
			} else {
				print_files(cons, sub_finfo);
			}
		}
	} else {
		/* 相对路径，先补全为绝对路径 */
		cons_printf(cons, "相对路径参数:%s\n", parameter);
		strcpy(apath, task->dir);
		strcat(apath, parameter);
		sub_finfo = dir_check(apath,fat);
		if(sub_finfo==0){
			cons_printf(cons, "目录不存在：%s",apath);
		} else {
			print_files(cons, sub_finfo);
		}
	}
	
	cons_newline(cons);
	return;
}

void print_files(struct CONSOLE *cons, struct FILEINFO *finfo){
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
	return;
}
