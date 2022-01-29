/* 切换命令行目录 */
#include <bootpack.h>
#include <string.h>

void cmd_cd(struct CONSOLE *cons, char *parameter, int *fat) {
	struct TASK *task = task_now();
	char apath[350];
	int i;
	if(strcmp(parameter,".")==0){
		// 目录不变
		cons_printf(cons,"当前console路径已切换为：%s\n", task->dir);
		return;
	}	
	if(strcmp(parameter,"/")==0){
		// 切换到根目录
		strcpy(task->dir,"/");
		cons_printf(cons,"当前console路径已切换为：%s\n", task->dir);
		return;
	}
	
	
	/* 如果不是以'/'开头，则是相对路径 */
	if(parameter[0]!='/'){
		/* 相对路径，处理时加上当前路径 */
		strcpy(apath, task->dir);
		strcat(apath, parameter);
	} else {
		/* 绝对路径 */
		strcpy(apath, parameter);
	}
	
	/* 先查找路径是否存在 */
	if(dir_check(apath, fat)!=0) {
		cons_putstr0(cons,"路径存在。\n");
		if(strlen(apath) < 255) {
			i = strlen(apath);
			if(apath[i]!='/'){
				/* 如果路径不是以'/'结尾，则在结尾处添加一个 */
				apath[i] = '/';
				apath[i+1] = 0;
			}
			strcpy(task->dir,apath);
			cons_printf(cons,"当前console路径已切换为：%s\n", task->dir);
		} else {
			cons_putstr0(cons,"路径过长。\n");
		}
	} else {
		cons_putstr0(cons,"路径不存在。\n");
	}
	
	return;
}
