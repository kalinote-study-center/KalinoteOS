/* �p��???�施�ߍs�Q���I���� */
#include "apilib.h"

void KaliMain(void)
{
	int fh;
	char c, cmdline[30], *p;

	api_cmdline(cmdline, 30);
	for (p = cmdline; *p > ' '; p++) { }	/* ��?�V�O�I���e�C����������i */
	for (; *p == ' '; p++) { }	/* ��?��i */
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
