/*��������ж������������*/
#include <stdio.h>
#include "bootpack.h"

extern struct FIFO8 keyfifo;

void KaliMain(void){
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//������Ϣ(BOOTINFO�ṹ��)
	char s[40], mcursor[256], keybuf[32];
	int mx, my, i;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC��ʼ�����������CPU���жϽ�ֹ */
	
	fifo8_init(&keyfifo, 32, keybuf);							//��ʼ��fifo������
	io_out8(PIC0_IMR, 0xf9); /* ����PIC1�ͼ���(11111001) */
	io_out8(PIC1_IMR, 0xef); /* �������(11101111) */
	
	init_palette();												//��ʼ����ɫ��
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);		//��ʼ����Ļ
	/*
	* ע������� binfo->vram �ȼ���(*binfo).vram
	* ������ͬ��
	*/
	mx = (binfo->scrnx - 16) / 2; 								//���ָ��λ�ã�Ĭ��Ϊ��Ļ����
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL_LDBLUE);					//��ʼ�����ָ��
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16,
		mx, my, mcursor, 16);									//�������
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, s);
	/*
	* ע����ʱ����걳��ֻ����COL_LDBLUE�Ĵ�ɫ�������ƶ��ǻḲ�ǵ����������
	* ������������������
	*/
	
	putfonts8_asc(binfo->vram, binfo->scrnx, 236, 181, COL_DRED, "KalinoteOS");
	putfonts8_asc(binfo->vram, binfo->scrnx, 235, 180, COL_BRED, "KalinoteOS");
	/*
	* ֻҪ�Ȼ���һ�鰵ɫ���壬Ȼ���ٻ���һ����ɫ���壬Ȼ�������ַ�����1�����أ��Ϳ��������������
	* ������������ļ��ṹ����ԼӸ�������ר�Ż��������ַ���
	*/
	
	//����������ݣ�ԭ���ڵ�98ҳ
	//sprintf(s, "scrnx = %d", binfo->scrnx);
	//putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL_WHITE, s);
	
	for(;;){
		//ֹͣCPU
		io_cli();
		if (fifo8_status(&keyfifo) == 0) {
			io_stihlt();
		} else {
			i = fifo8_get(&keyfifo);
			io_sti();
			sprintf(s, "%02X", i);
			boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 0, 16, 15, 31);
			putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL_WHITE, s);
		}
	}
}

void HariMain(void){
	/*ϵͳ�������*/
	KaliMain();
}
