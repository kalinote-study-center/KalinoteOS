/*��������ж������������*/
#include <stdio.h>

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

//�ṹ��
struct BOOTINFO {
	/*������Ϣ - �˴�ԭ�����ڵ�89ҳ*/
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

struct SEGMENT_DESCRIPTOR {
	/*���GDT��8�ֽ����� - �˴�ԭ�����ڵ�105ҳ*/
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
	/*���IDT��8�ֽ����� - �˴�ԭ�����ڵ�105ҳ*/
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

//naskfunc�еĺ���(����д)
void io_hlt(void);							//hlt����
//void write_mem8(int addr, int data);		//д���ڴ�(��ָ��ȡ��)
void io_cli(void);							//��ֹ�ж�
void io_out8(int port, int data);			//���������õ�
int io_load_eflags(void);					//��ȡ�����eflagsֵ
void io_store_eflags(int eflags);			//��ֵ����eflags�Ĵ���
void load_gdtr(int limit, int addr);		//����GDTR�Ĵ���
void load_idtr(int limit, int addr);		//����IDTR�Ĵ���

//���ļ��еĺ���
void KaliMain(void);																				//������
void init_palette(void);																			//��ʼ����ɫ�庯��
void set_palette(int start, int end, unsigned char *rgb);											//���õ�ɫ��
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);		//���Ʒ���
void init_screen(char *vram, int x, int y);															//��ʼ����Ļ
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);								//��������
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);					//�����ַ���
void init_mouse_cursor8(char *mouse, char bc);														//��ʼ�����ָ��
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);											//��걳��ɫ����
void init_gdtidt(void);																				//��ʼ��GDT��IDT
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

void KaliMain(void){
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;		//������Ϣ(BOOTINFO�ṹ��)
	char s[40], mcursor[256];
	int mx, my;
	
	init_gdtidt();
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
	sprintf(s, "scrnx = %d", binfo->scrnx);
	putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL_WHITE, s);
	
	for(;;){
		//ֹͣCPU
		io_hlt();
	}
}

void HariMain(void){
	/*ϵͳ�������*/
	KaliMain();
}
