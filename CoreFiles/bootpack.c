//naskfunc�еĺ���
void io_hlt(void);							//hlt����
//void write_mem8(int addr, int data);		//д���ڴ�(��ָ��ȡ��)
void io_cli(void);							//��ֹ�ж�
void io_out8(int port, int data);			//���������õ�
int io_load_eflags(void);					//��ȡ�����eflagsֵ
void io_store_eflags(int eflags);			//��ֵ����eflags�Ĵ���

//���ļ��еĺ���
void KaliMain(void);																				//������
void init_palette(void);																			//��ʼ����ɫ�庯��
void set_palette(int start, int end, unsigned char *rgb);											//���õ�ɫ��
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);		//���Ʒ���
void init_screen(char *vram, int x, int y);															//��ʼ����Ļ
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);								//��������

// 15����ɫ��������
#define COL_BLACK		0
#define COL_BRED		1
#define COL_BGREEN		2
#define COL_BYELLOW		3
#define COL_BBLUE		4
#define COL_BPURPLE		5
#define COL_LBBLUE		6
#define COL_WHITE		7
#define COL_BGREY		8
#define COL_DRED		9
#define COL_DGREEN		10
#define COL_DYELLOW		11
#define COL_DBLUE		12
#define COL_DPURPLE		13
#define COL_LDBLUE		14
#define COL_DGREY		15

struct BOOTINFO {
	/*������Ϣ - �˴�ԭ�����ڵ�89ҳ*/
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

void KaliMain(void){
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	extern char fonts[4096];
	
	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);		//��ʼ����Ļ
	/*
	* ע������� binfo->vram �ȼ���(*binfo).vram
	* ������ͬ��
	*/
	putfont8(binfo->vram, binfo->scrnx,  8, 8, COL_WHITE, fonts + 'A' * 16);
	putfont8(binfo->vram, binfo->scrnx, 16, 8, COL_WHITE, fonts + 'B' * 16);
	putfont8(binfo->vram, binfo->scrnx, 24, 8, COL_WHITE, fonts + 'C' * 16);
	putfont8(binfo->vram, binfo->scrnx, 40, 8, COL_WHITE, fonts + '1' * 16);
	putfont8(binfo->vram, binfo->scrnx, 48, 8, COL_WHITE, fonts + '2' * 16);
	putfont8(binfo->vram, binfo->scrnx, 56, 8, COL_WHITE, fonts + '3' * 16);
	
	for(;;){
		//ֹͣCPU
		io_hlt();
	}
}

void init_palette(void){
	/*��ɫ�庯����Ԥ��15�л�����ɫ������������� - �˴�ԭ�����ڵ�75ҳ*/
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:�� */
		0xff, 0x00, 0x00,	/*  1:���� */
		0x00, 0xff, 0x00,	/*  2:���� */
		0xff, 0xff, 0x00,	/*  3:���� */
		0x00, 0x00, 0xff,	/*  4:���� */
		0xff, 0x00, 0xff,	/*  5:���� */
		0x00, 0xff, 0xff,	/*  6:ǳ���� */
		0xff, 0xff, 0xff,	/*  7:�� */
		0xc6, 0xc6, 0xc6,	/*  8:���� */
		0x84, 0x00, 0x00,	/*  9:���� */
		0x00, 0x84, 0x00,	/* 10:���� */
		0x84, 0x84, 0x00,	/* 11:���� */
		0x00, 0x00, 0x84,	/* 12:���� */
		0x84, 0x00, 0x84,	/* 13:���� */
		0x00, 0x84, 0x84,	/* 14:ǳ���� */
		0x84, 0x84, 0x84	/* 15:���� */
	};
	set_palette(0, 15, table_rgb);
	return;
	// C�����е�static charֻ���������ݣ��൱�ڻ���е�DB
}

void set_palette(int start, int end, unsigned char *rgb){
	/*��ɫ�幦�� - �˴�ԭ�����ڵ�75ҳ*/
	int i, eflags;
	eflags = io_load_eflags();	/* ��¼�ж���ɱ�־��ֵ */
	io_cli(); 					/* ���ж���ɱ�־��Ϊ0����ֹ�ж� */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* ��ԭ�ж���ɱ�־ */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	/*���Ʒ��� - �˴�ԭ�����ڵ�84ҳ*/
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_screen(char *vram, int x, int y){
	/*��ʼ����Ļ*/
	boxfill8( vram, x, COL_LDBLUE, 0, 0, x - 1, y - 1);		//����һ����ɫ����������
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	/*�������� - �˴�ԭ�����ڵ�93ҳ*/
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}


void HariMain(void){
	/*ϵͳ�������*/
	KaliMain();
}