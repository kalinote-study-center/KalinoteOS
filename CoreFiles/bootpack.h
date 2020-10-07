//asmhead.nas
struct BOOTINFO {	/* 0x0ff0-0x0fff */
	/*������Ϣ - �˴�ԭ�����ڵ�89ҳ*/
	char cyls;	/*��������ȡӲ��λ��*/
	char leds;	/*����ʱ���̵�LED״̬*/
	char vmode;	/*�Կ�ģʽ*/
    char reserve;
	short scrnx, scrny;	/*����ֱ���*/
	char *vram;
};
#define ADR_BOOTINFO	0x00000ff0

//naskfunc.nas�еĺ���(����д)
void io_hlt(void);							//��ͣ������
//void write_mem8(int addr, int data);		//д���ڴ�(��ָ��ȡ��)
void io_cli(void);							//��ֹ�ж�
void io_sti(void);							//�����ж�
void io_stihlt(void);						//�����жϲ���ͣ������
int io_in8(int port);						//���������õ�
void io_out8(int port, int data);			//���������õ�
int io_load_eflags(void);					//��ȡ�����eflagsֵ
void io_store_eflags(int eflags);			//��ֵ����eflags�Ĵ���
void load_gdtr(int limit, int addr);		//����GDTR�Ĵ���
void load_idtr(int limit, int addr);		//����IDTR�Ĵ���
int load_cr0(void);							//����CR0�Ĵ���
void store_cr0(int cr0);					//����CR0�Ĵ���
void asm_inthandler21(void);				//21�жϣ�ע����0x21
void asm_inthandler27(void);				//27�жϣ�ע����0x27
void asm_inthandler2c(void);				//2c�жϣ�ע����0x2c
unsigned int memtest_sub(
	unsigned int start,
	unsigned int end);						//��ȡ�ڴ�

//graphic.c
void init_palette(void);																			//��ʼ����ɫ�庯��
void set_palette(int start, int end, unsigned char *rgb);											//���õ�ɫ��
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);		//���Ʒ���
void init_screen(char *vram, int x, int y);															//��ʼ����Ļ
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);								//��������
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);					//�����ַ���
void init_mouse_cursor8(char *mouse, char bc);														//��ʼ�����ָ��
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);											//��걳��ɫ����

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

//dsctlb.c
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void);																				//��ʼ��GDT��IDT
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

/* int.c */
void init_pic(void);
void inthandler27(int *esp);			//27�ж�
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

/* fifo.c */
struct FIFO8 {
	unsigned char *buf;
	int p, q, size, free, flags;
};
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);									//��ʼ��FIFO������
int fifo8_put(struct FIFO8 *fifo, unsigned char data);												//��FIFO�����ݲ�����
int fifo8_get(struct FIFO8 *fifo);																	//��FIFO���һ������
int fifo8_status(struct FIFO8 *fifo);																//��ѯ������״̬

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
extern struct FIFO8 mousefifo;
void inthandler2c(int *esp);																		//�������ж�
void enable_mouse(struct MOUSE_DEC *mdec);															//�������
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

/* keyboard.c */
void inthandler21(int *esp);																		//���̼����ж�
void wait_KBC_sendready(void);																		//�ȴ����̿��Ƶ�·׼�����
void init_keyboard(void);																			//��ʼ�����̿��Ƶ�·
extern struct FIFO8 keyfifo;
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

/* memory.c */
#define MEMMAN_FREES		4090	/* ��Լ32KB */
#define MEMMAN_ADDR			0x003c0000
struct FREEINFO {	/* ������Ϣ */
	unsigned int addr, size;
};
struct MEMMAN {		/* �ڴ���� */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);											//����CPU����
void memman_init(struct MEMMAN *man);																//��ʼ���ڴ�������
unsigned int memman_total(struct MEMMAN *man);														//��ȡʣ���ڴ��С
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);									//�����ڴ�
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);							//�ͷ��ڴ�
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);								//4K��ռ�����ڴ�
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);						//4K��ռ��ͷ��ڴ�

/* sheet.c */
#define MAX_SHEETS		256
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
};
struct SHTCTL {
	unsigned char *vram;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);		//��ʼ����
struct SHEET *sheet_alloc(struct SHTCTL *ctl);														//���δʹ�õ���ͼ��
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);		//���û�������С
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);								//����sheet�߶�
void sheet_refresh(struct SHTCTL *ctl);																//ˢ��ͼ��
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0);							//�ƶ�ͼ��
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht);												//�ͷ���ʹ�õ�ͼ���ڴ�
