/*��������ж������������*/
#include <stdio.h>
#include "bootpack.h"

struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};

extern struct FIFO8 keyfifo, mousefifo;
void enable_mouse(struct MOUSE_DEC *mdec);							//�������
void init_keyboard(void);											//��ʼ�����̿��Ƶ�·
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
extern struct FIFO8 keyfifo, mousefifo;			

void KaliMain(void){
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//������Ϣ(BOOTINFO�ṹ��)
	char s[40], mcursor[256], keybuf[32], mousebuf[128];
	int mx, my, i;
	struct MOUSE_DEC mdec;
	
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
	
	enable_mouse(&mdec);			//�������
	
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
				if (mouse_decode(&mdec, i) != 0) {
					/* 3�ֽڼ��룬��ʾ���� */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {
						s[2] = 'C';
					}
					boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 32, 16, 32 + 15 * 8 - 1, 31);
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

void wait_KBC_sendready(void){
	/* �ȴ����̿��Ƶ�·׼����� - �˴�ԭ�����ڵ�140ҳ */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(void){
	/* ��ʼ�����̿��Ƶ�· */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(struct MOUSE_DEC *mdec){
	/* ������� */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	/* û������Ļ���ACK(0xfa)�ᱻ������ */
	mdec->phase = 0; /* �ȴ�0xfa�Ľ׶� */
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
	if (mdec->phase == 0) {
		/* �ȴ�0xfa�Ľ׶� */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* �ȴ�����һ�ֽڵĽ׶� */
		if ((dat & 0xc8) == 0x08) {
			/* �����һ�ֽ���ȷ */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* �ȴ����ڶ��ֽڵĽ׶� */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* �ȴ��������ֽڵĽ׶� */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* ����y�����뻭������෴ */
		return 1;
	}
	return -1; /* ���� */
}


void HariMain(void){
	/*ϵͳ�������*/
	KaliMain();
}
