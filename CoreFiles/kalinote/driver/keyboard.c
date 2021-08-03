/* 该文件负责监听键盘 */

#include "../bootpack.h"

struct FIFO32 *keyfifo;
int keydata0;

void inthandler21(int *esp){
	/* PS/2键盘中断 */
	int data;
	io_out8(PIC0_OCW2, 0x61);	/* 通知PIC：IRQ-01受理完毕(0x60+IRQ编号)，将0x60+IRQ号码输出给OCW2以通知PIC收到IQR的中断通知，然后PIC继续监视IRQ1中断 */
	data = io_in8(PORT_KEYDAT);
	fifo32_put(keyfifo, data + keydata0);
	return;
}

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void){
	/* 等待键盘控制电路准备完毕 - 此处原内容在第140页 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(struct FIFO32 *fifo, int data0){
	/* 将FIFO缓冲区的信息保存到全局变量里 */
	keyfifo = fifo;
	keydata0 = data0;
	/* 键盘控制器的初始化 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

void reset_cpu(void){
	/* 复位CPU */
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, 0xfe);
    for (;;) { io_hlt(); }
}
