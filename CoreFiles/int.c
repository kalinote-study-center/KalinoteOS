/* 中断处理 */
#include "bootpack.h"
#include <stdio.h>

void init_pic(void){
	/*PIC初始化*/
	io_out8(PIC0_IMR,  0xff  ); /* 禁止所有中断 */
	io_out8(PIC1_IMR,  0xff  ); /* 禁止所有中断 */

	io_out8(PIC0_ICW1, 0x11  ); /* 边沿触发模式(edge trigger mode) */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7由INT20-27接收 */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1由IRQ2链接 */
	io_out8(PIC0_ICW4, 0x01  ); /* 无缓冲区模式 */

	io_out8(PIC1_ICW1, 0x11  ); /* 边沿触发模式(edge trigger mode) */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15由NT28-2f接收 */
	io_out8(PIC1_ICW3, 2     ); /* PIC1由IRQ2链接 */
	io_out8(PIC1_ICW4, 0x01  ); /* 无缓冲区模式 */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1以外全部禁止 */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 禁止所有中断 */

	return;
}

#define PORT_KEYDAT		0x0060

struct KEYBUF keybuf;

void inthandler21(int *esp){
	/* PS/2键盘中断 */
	/*
	* 实际上现在基本上没什么人用PS/2键盘了
	* 不过模拟器里面可以用
	*/
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	/* 通知PIC：IRQ-01受理完毕 */
	data = io_in8(PORT_KEYDAT);
	if (keybuf.next < 32) {
		keybuf.data[keybuf.next] = data;
		keybuf.next++;
	}
	return;
}

void inthandler2c(int *esp){
	/* PS/2鼠标中断(同键盘，现在没什么人用PS/2鼠标了) */
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL_BLACK, 0, 0, 32 * 8 - 1, 15);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, "INT 2C (IRQ-12) : PS/2 mouse");
	for (;;) {
		io_hlt();
	}
}

void inthandler27(int *esp){
/* 来自PIC0的不完全中断对策 */
/* 由于芯片组的原因，在PIC初始化时，Athlon 64X2机等会发生一次中断 */
/*
* 为什么什么都不做？
* →该中断是由于PIC初始化时的电气噪声造成的
* 不必认真处理什么。
*/
	io_out8(PIC0_OCW2, 0x67); /* 将IRQ-07受理完成通知PIC（参照7-1） */
	return;
}
