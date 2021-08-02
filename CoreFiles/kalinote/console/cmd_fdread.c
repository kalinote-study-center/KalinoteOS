/* 该指令用于测试软盘读取 */
#include "../bootpack.h"

void cmd_fdread(struct CONSOLE *cons) {
	cons_putstr0(cons, "The instruction has a serious error and is therefore out of use!\n");
	// cons_putstr0(cons, "fdc_read\n");
    // fdc_read(0, 0, 1);
	return;
}

void cmd_fdwrite(struct CONSOLE *cons) {
	cons_putstr0(cons, "The instruction has a serious error and is therefore out of use!\n");
    // const int size = 1024;
    // struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    // unsigned char* buf = (unsigned char*)memman_alloc_4k(memman, size);
	// int i;
	
    // for (i = 0; i < size; ++i)
      // buf[i] = i;
    // fdc_write(buf, 0, 0, 1); // buf, head, track, sector

    // cons_putstr0(cons, "\nTry dump\n");
    // fdc_read(0, 0, 1);
	return;
}
