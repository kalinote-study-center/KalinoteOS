/*��������ж������������*/
#include <stdio.h>
#include "bootpack.h"

extern struct FIFO8 keyfifo, mousefifo;
void enable_mouse(void);			//�������
void init_keyboard(void);			//��ʼ�����̿��Ƶ�·

void KaliMain(void){
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//������Ϣ(BOOTINFO�ṹ��)
	char s[40], mcursor[256], keybuf[32], mousebuf[128];
	int mx, my, i;
	unsigned char mouse_dbuf[3], mouse_phase;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC��ʼ�����������CPU���жϽ�ֹ */
	fifo8_init(&keyfifo, 32, keybuf);							//��ʼ������fifo������
	fifo8_init(&mousefifo, 128, mousebuf);						//��ʼ�����fifo������
	io_out8(PIC0_IMR, 0xf9); /* ����PIC1�ͼ���(11111001) */
	io_out8(PIC1_IMR, 0xef); /* �������(11101111) */
	
	init_keyboard();		//��ʼ������
	
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
	
	enable_mouse();			//�������
	mouse_phase = 0;		//���뵽�ȴ�����0xfa״̬
	
	for(;;){
		//ֹͣCPU
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
			io_stihlt();
		} else {
			if (fifo8_status(&keyfifo) != 0) {
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE,  0, 16, 15, 31);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL_WHITE, s);
			} else if (fifo8_status(&mousefifo) != 0) {
				i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_phase == 0) {
					/* �ȴ�����0xfa״̬ */
					if (i == 0xfa) {
						mouse_phase = 1;
					}
				} else if (mouse_phase == 1) {
					/* �ȴ����ĵ�һ�ֽ� */
					mouse_dbuf[0] = i;
					mouse_phase = 2;
				} else if (mouse_phase == 2) {
					/* �ȴ����ĵڶ��ֽ� */
					mouse_dbuf[1] = i;
					mouse_phase = 3;
				} else if (mouse_phase == 3) {
					/* �ȴ����ĵ����ֽ� */
					mouse_dbuf[2] = i;
					mouse_phase = 1;
					/* �����ֽ��ռ���ϣ���ʾ���� */
					sprintf(s, "%02X %02X %02X", mouse_dbuf[0], mouse_dbuf[1], mouse_dbuf[2]);
					boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 32, 16, 32 + 8 * 8 - 1, 31);
					putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL_WHITE, s);
				}
			}
		}
	}
}


#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* �ȴ����̿��Ƶ�·׼����� - �˴�ԭ�����ڵ�140ҳ */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(void)
{
	/* ��ʼ�����̿��Ƶ�· */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(void)
{
	/* ������� */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return; /* ���ޤ�������ACK(0xfa)�����Ť���Ƥ��� */
}


void HariMain(void){
	/*ϵͳ�������*/
	KaliMain();
}
