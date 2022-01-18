/* shell命令解释器 */
#include <apilib.h>
#include <stdio.h>

void KaliMain(void) {
	char key;
	char s[30], buf[1024];
	int i = 0;
	char langmode = api_getlang();		/* 获取语言模式 */

	/* TODO：从命令行获取一段shell参数，有可能是文件名，也有可能就是一句shell语句 */
	api_cmdline(s, 30);
		
	for(;;) {
start_shell:
		i = 0;
		api_putstr0("/>");
		for(;;) {
			if(i >= 1024) {
				api_putstr0("\ncommand too long!");
				goto start_shell;
			}
			key = api_getkey(1);
			/* TODO：在这里加入特殊键值判断 */
			api_putchar(key);
			buf[i] = key;
			/* 这里的break需要进行确认，如果第一行最后一个字符(可打印)为'\'，则不进行break */
			if(buf[i] == '\n' && buf[i-1] != '\\') {
				buf[i+1] = 0;
				api_putstr0(buf);
				break;
			}
			i++;
		}
		/* 在这里处理一行命令 */
	}
	
}
