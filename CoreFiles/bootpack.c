//naskfunc�еĺ���
void io_hlt(void);							//hlt����
//void write_mem8(int addr, int data);		//д���ڴ�(��ָ��ȡ��)
void io_cli(void);							//��ֹ�ж�
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

//���ļ��еĺ���
void KaliMain(void);																				//������
void init_palette(void);																			//��ʼ����ɫ�庯��
void set_palette(int start, int end, unsigned char *rgb);											//���õ�ɫ��
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);		//���Ʒ���
void init_screen(char *vram, int x, int y);															//��ʼ����Ļ

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
	/*������Ϣ*/
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

void KaliMain(void){
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	
	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);		//��ʼ����Ļ
	/*
	* ע������� binfo->vram �ȼ���(*binfo).vram
	* ������ͬ��
	*/
	
	for(;;){
		//ֹͣCPU
		io_hlt();
	}
}

void init_palette(void){
	/*��ɫ�庯����Ԥ��15�л�����ɫ�������������*/
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
	/*��ɫ�幦��*/
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
	/*���Ʒ���*/
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

void HariMain(void){
	/*ϵͳ�������*/
	KaliMain();
}