/* 用于???取命令行参数的程序 */
#include "apilib.h"

void KaliMain(void)
{
	int fh;
	char c, cmdline[30], *p;

	api_cmdline(cmdline, 30);
	for (p = cmdline; *p > ' '; p++) { }	/* 跳?之前的内容，直到遇到空格 */
	for (; *p == ' '; p++) { }	/* 跳?空格 */
	fh = api_fopen(p);
	if (fh != 0) {
		for (;;) {
			if (api_fread(&c, 1, fh) == 0) {
				break;
			}
			api_putchar(c);
		}
	} else {
		api_putstr0("File not found.\n");
	}
	api_end();
}
