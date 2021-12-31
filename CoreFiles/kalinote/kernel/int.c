/* 中断处理 */
#include "../bootpack.h"
#include <stdio.h>

void init_pic(void){
	/*PIC初始化*/
	/* 每个常量代表什么意思可以去bootpack.h看 */
	io_out8(PIC0_IMR,  0xff  ); /* 禁止所有中断 */
	io_out8(PIC1_IMR,  0xff  ); /* 禁止所有中断 */
	/* 
	* 关于上面两行代码，0xff=11111111，每一位控制一个IRQ信号，全部为1表示屏蔽所有
	* 初始化前屏蔽所有中断是为了防止初始化时产生电气干扰
	*/

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

void inthandler27(int *esp){
	/* 这个中断没有任何作用 */
	io_out8(PIC0_OCW2, 0x67); /* 将IRQ-07受理完成通知PIC（参照7-1） */
	return;
}
