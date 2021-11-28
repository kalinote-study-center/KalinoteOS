/* 输出文件及目录 */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

void print_file(struct CONSOLE *cons, struct FILEINFO *finfo);		// 打印某个目录

void cmd_dir(struct CONSOLE *cons, char *parameter, int *fat){
	/* 输出文件及目录 */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *root_finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);		/* 根目录信息 */
	struct FILEINFO *subdir_finfo;		/* 子目录在根目录中的信息 */
	struct FILEINFO *sub_finfo;			/* 子目录目录信息 */
	char *subdirinfo_memory;
	
	/* 先处理参数 */
	if(parameter[0]=='/'){
		/* 绝对路径，先读根目录，后面再做改进 */
		cons_printf(cons, "绝对路径参数:%s\n", parameter);
		print_file(cons, root_finfo);
	} else {
		cons_printf(cons, "相对路径参数:%s\n", parameter);
		// cons_printf(cons, "len:%d\n",strlen(parameter));
		if(parameter[0]=='.'){
			/* 当前目录，先读根目录，后面再做改进 */
			print_file(cons, root_finfo);
		} else {
			/* 暂时先不处理路径，先想办法把子目录文件读出来 */
			subdir_finfo = dir_search(parameter, root_finfo, 224);
			if(subdir_finfo != 0) {
				/* 目录存在 */
				if((subdir_finfo->type & 0xef) == 0){
					/* 是目录 */
					subdirinfo_memory = (char *) memman_alloc_4k(memman, 512);
					file_loadfile(subdir_finfo->clustno, 512, subdirinfo_memory, fat, (char *) (ADR_DISKIMG + 0x003e00));
					sub_finfo = (struct FILEINFO *)subdirinfo_memory;
					print_file(cons, sub_finfo);
					memman_free_4k(memman, (int)subdirinfo_memory,512);
				} else {
					/* 文件存在，但不是目录 */
					cons_printf(cons, "%s Not a directory.", parameter);
				}
			} else {
				/* 目录不存在 */
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
