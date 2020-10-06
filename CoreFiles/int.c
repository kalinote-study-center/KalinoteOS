/* �жϴ��� */
#include "bootpack.h"
#include <stdio.h>

void init_pic(void){
	/*PIC��ʼ��*/
	io_out8(PIC0_IMR,  0xff  ); /* ��ֹ�����ж� */
	io_out8(PIC1_IMR,  0xff  ); /* ��ֹ�����ж� */

	io_out8(PIC0_ICW1, 0x11  ); /* ���ش���ģʽ(edge trigger mode) */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7��INT20-27���� */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1��IRQ2���� */
	io_out8(PIC0_ICW4, 0x01  ); /* �޻�����ģʽ */

	io_out8(PIC1_ICW1, 0x11  ); /* ���ش���ģʽ(edge trigger mode) */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15��NT28-2f���� */
	io_out8(PIC1_ICW3, 2     ); /* PIC1��IRQ2���� */
	io_out8(PIC1_ICW4, 0x01  ); /* �޻�����ģʽ */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1����ȫ����ֹ */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 ��ֹ�����ж� */

	return;
}

#define PORT_KEYDAT		0x0060

struct KEYBUF keybuf;

void inthandler21(int *esp){
	/* PS/2�����ж� */
	/*
	* ʵ�������ڻ�����ûʲô����PS/2������
	* ����ģ�������������
	*/
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	/* ֪ͨPIC��IRQ-01������� */
	data = io_in8(PORT_KEYDAT);
	if (keybuf.next < 32) {
		keybuf.data[keybuf.next] = data;
		keybuf.next++;
	}
	return;
}

void inthandler2c(int *esp){
	/* PS/2����ж�(ͬ���̣�����ûʲô����PS/2�����) */
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL_BLACK, 0, 0, 32 * 8 - 1, 15);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, "INT 2C (IRQ-12) : PS/2 mouse");
	for (;;) {
		io_hlt();
	}
}

void inthandler27(int *esp){
/* ����PIC0�Ĳ���ȫ�ж϶Բ� */
/* ����оƬ���ԭ����PIC��ʼ��ʱ��Athlon 64X2���Ȼᷢ��һ���ж� */
/*
* Ϊʲôʲô��������
* �����ж�������PIC��ʼ��ʱ�ĵ���������ɵ�
* �������洦��ʲô��
*/
	io_out8(PIC0_OCW2, 0x67); /* ��IRQ-07�������֪ͨPIC������7-1�� */
	return;
}
