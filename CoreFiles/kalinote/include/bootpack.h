/* 
 * KalinoteOS��ͷ�ļ�
 * Ŀǰ�����ļ��ĺ������ڴ�ͷ�ļ��н�������
 * ��ͷ�ļ��е����к������������ṹ��ȣ����Ѿ������ļ����з���
 * �������Ҫ�����Է���ز�ֳɲ�ͬͷ�ļ�
 */

/*
* ��Щʱ����ܻ��õ�Ƕ��ʽ�����룬��������������ʽ����˵����
* asm("������"
*	: ����Ĵ���
*	: ����Ĵ���
*	: �ᱻ�޸ĵļĴ���)
*/

/* ϵͳȫ�ֶ��� */
/* sysinfo(bootpack.cϵͳȫ����Ϣ) */
typedef struct {
	char cpuid;						/* �Ƿ�֧��CPUID */
	char brandString;				/* �Ƿ�֧��Brand String */
	/* ���濼�ǰ�����flag���ɰ�λ����(��ʡ�ռ�) :) */
	unsigned char max_cpuid_num;	/* ���CPUID���� */
	char oem[13];					/* CPU OEM��Ϣ */
	char CPUName[49];				/* CPU�ͺ���Ϣ */
} CPUIDINFO;
typedef struct {
	/* VBE��Ϣ */
	char VbeSignature[4];
	short VbeVersion;
	int *OemStringPtr;
	int Capabilities;
	int *VideoModePtr;
	short TotalMemory;
	short OemSoftwareRev;
	int *OemVerdorNamePtr;
	int *OemProductNamePtr;
	int *OemProductRevPtr;
	char Reserved[222];
} VBEINFOBLOCK;
typedef struct {
	/* ʱ�� */
	int year, month, day;					/* ϵͳ���� */
	int hour, min, sec;						/* ϵͳʱ�� */
} DATETIME;
struct SYSINFO {
	double sysmode;							/* ϵͳģʽ */
	unsigned int sysmmainver;				/* ϵͳ���汾�� */
	int sysver;								/* ϵͳ�汾�� */
	unsigned char ide_hd_num;				/* �����IDEӲ������ */
	unsigned int free_mem;					/* ϵͳʣ���ڴ����� */
	unsigned int memtotal;					/* ϵͳ���ڴ� */
	// int year, month, day;				/* (�ѷ���)ϵͳ���� */
	// int hour, min, sec;					/* (�ѷ���)ϵͳʱ�� */
	CPUIDINFO cpuid_info;					/* CPUIDINFO�ṹ�� */
	VBEINFOBLOCK vbe_info;					/* VBE��Ϣ TODO������VBE��Ϣ��ȡ */
	DATETIME datetime;						/* ʱ������� */
};
#define	SYS_MEMORY		0x00400000			// ϵͳռ���ڴ�
#define FIFO_ADDR		0x0fec				// FIFO��ַ
#define SYSINFO_ADDR	0x10000				// ϵͳ��Ϣ��ַ
#define SHTCTL_ADDR		0x0fe4				// ϵͳͼ������ṹ���ַ
#define HZK_ADDR		0x10fe8				// �������屣���ַ
#undef	NULL			
#define NULL ((void*)0)						// ���¶���NULL
#ifndef FALSE
#define FALSE  (0)							// ����FALSE
#define TRUE   (1)							// ����TRUE
#endif

/* asmhead.nas(bootpack.c��ǰ�沿��) �Լ� naskfunc.nas */
struct BOOTINFO {		/* 0x0ff0-0x0fff */
	/*������Ϣ - �˴�ԭ�����ڵ�89ҳ*/
	char cyls;			/*��������ȡӲ��λ��*/
	char leds;			/*����ʱ���̵�LED״̬*/
	char vmode;			/*�Կ�ģʽ*/
    char reserve;
	short scrnx, scrny;	/*����ֱ���*/
	int *vram;
};
#define ADR_BOOTINFO	0x00000ff0						// bootinfoλ��
#define ADR_VBEINFO		0x00001000						// VBEINFOλ��
#define ADR_DISKIMG		0x00100000						// �ļ�λ��(���̣��ڴ���)
void io_hlt(void);										// ��ͣ������
//void write_mem8(int addr, int data);					// д���ڴ�(��ָ��ȡ��)
void io_cli(void);										// ��ֹ�ж�
void io_sti(void);										// �����ж�
void io_stihlt(void);									// �����жϲ���ͣ������
int io_in8(int port);									// ���������õģ����INָ����ڶ˿ڲ���(8λ)
void io_out8(int port, int data);						// ���������õģ����OUTָ����ڶ˿ڲ���(8λ)
int io_in16(int port);									// ���������õģ����INָ����ڶ˿ڲ���(16λ)
void io_out16(int port, int data);						// ���������õģ����OUTָ����ڶ˿ڲ���(16λ)
int io_in32(int port);									// ���������õģ����INָ����ڶ˿ڲ���(32λ)
void io_out32(int port, int data);						// ���������õģ����OUTָ����ڶ˿ڲ���(32λ)
int io_load_eflags(void);								// ��ȡ�����eflagsֵ
void io_store_eflags(int eflags);						// ��ֵ����eflags�Ĵ���
void port_read(unsigned short port, void *buf, int n);	// �˿ڶ�����
void port_write(unsigned short port, void *buf, int n);	// �˿�д����
void load_gdtr(int limit, int addr);					// ����GDTR�Ĵ���(GDTR�Ĵ������ڱ���GDT���ڴ��е�λ��)
void load_idtr(int limit, int addr);					// ����IDTR�Ĵ���(IDTR�Ĵ������ڱ���IDT���ڴ��е�λ��)
int load_cr0(void);										// ����CR0�Ĵ���
void store_cr0(int cr0);								// ����CR0�Ĵ���
void load_tr(int tr);									// ����TR�Ĵ���
int check_cpuid(void);									// ���CPUID�Ƿ����
int read_cpuid(int code, int *ebx, int *edx, int *ecx);	// ��CPUID��ȡ��Ϣ
void asm_inthandler_divzero(void);						// 00���жϣ������쳣
void asm_inthandler_device_not_available(void);			// 07���жϣ���ѧЭ������������
void asm_inthandler_stack_segment(void);				// 0c���жϣ����ڴ���ջ�쳣
void asm_inthandler_general_protection(void);			// 0d���жϣ����ڴ����쳣����
void asm_inthandler_timer(void);						// 20���жϣ�����timer
void asm_inthandler21(void);							// 21���жϣ�ע����0x21
void asm_inthandler26(void);							// 26���жϣ�����FDC
void asm_inthandler27(void);							// 27���жϣ�ע����0x27
void asm_inthandler2c(void);							// 2c���жϣ�ע����0x2c
void asm_inthandler2e(void);							// IDEӲ���ж�
unsigned int memtest_sub(			
	unsigned int start,			
	unsigned int end);									//��ȡ�ڴ�
void farjmp(int eip, int cs);							//ָ����ת(���������л�)
void farcall(int eip, int cs);							//ת�Ƶ����õ��ӳ���(ָ��ƫ��)
void asm_kal_api(void);									//KalinoteOS ϵͳAPI
void start_app(int eip, int cs,			
	int esp, int ds, int *tss_esp0);					//����Ӧ�ó���
void asm_end_app(int* esp);								//����Ӧ�ó���
void clts(void);
void io_fnsave(int *addr);
void io_frstor(int *addr);
void fwait(void);

//dsctbl.c(GDT,IDT)
struct SEGMENT_DESCRIPTOR {
	/* GDT��ȫ�ֶκż�¼�����ݽṹ��ÿ����¼8�ֽ� */
	/* ÿ���α�����3����Ϣ���ֱ��Ƕδ�С(limit)���λ�ַ(base)�Ͷι�������(access_right) */
	/* �ڸýṹ����Ϊ��ϵͳ�Բ�ͬCPU�ļ����ԣ�base��Ϊ��low(2�ֽ�)��mid(1�ֽ�)��high(1�ֽ�)����������32λ */
	/* ����(limit)�ĳ�����20λ���ڶ���������һ����־λ(Gbit)�����ڱ�ʾlimit�ĵ�λ����GbitΪ1ʱ������ҳ(4KB)Ϊ��λ������limit��߿ɱ�ʾ4GB(4KB x 1M) */
	/* ������(access_right)Ϊ12λ���������ݼ��·����� */
	/*
	* 31-24        23 22   21 20   19-16        15 14-13 12 11-8  7-0          (��32λ)
	* �λ�ַ��8λ  G  D/B  L  AVL  �ν��޸�4λ  P  DPL   S  TYPE  �λ�ַ��8λ
	*                   31-16                          15-0                    (��32λ)
	*                �λ�ַ��16λ                  �ν��޵�16λ
	*/
	/*
	* ��Ȩ��(����)��12λ(8-15,20-23)����˵����
	* 8-11λ��TYPEָ�������ͣ��Ӹߵ���*�ֱ�ΪX C(E) R(W) Aλ�������ΪXCRA�����ݶ�ΪXEWA��
	* ���У�XΪ�Ƿ��ִ�У���λΪ��ִ�У������ʼ��Ϊ��ִ�У����ݶ�ʼ��Ϊ����ִ�С�
	* CΪһ���ԣ���λΪ����һ���ԡ�(��������˵��)
	* RΪ�ɶ���־λ����λΪ�ɶ���
	* AΪ�ѷ���λ����λ��ʾ�ö��ѱ����ʹ���
	* EΪ��չ����λ����λ��ʾ������չ������������չ��
	* WΪ�Ƿ�ɶ�д����λΪ�ɶ�д������Ϊֻ����
	* 12λ(S)ָ�����������ͣ���λΪ����/���ݶΣ�����Ϊϵͳ�Ρ�
	* 13-14λ(DPL)ָ���˶���������Ȩ����0Ϊ��ߣ�һ��Ϊ�ں˳���3Ϊ��ͣ�һ��ΪӦ�ó���
	* 15λ(P)ָ���˶��Ƿ����ڴ��У���λ��ʾ�ö������ڴ��У�����μĴ������Լ��ز����ڴ��еĶ�������NP�쳣
	* 20λ(AVL)Ϊϵͳʹ�õ�λ����������ʹ�á�����ʱ��0���ɡ�
	* 21λ(L)��64λ����α�־��������64λ������ʹ�á�
	* 22λ(D/B)ָ����Ĭ�ϲ�������С����λָʾ 32 λ��ƫ�Ƶ�ַ���߲�����������Ϊ16λ��
	* 23λ(G)ָ���˿����ȴ�С����λʱ������Ϊ4K����߹���4G�ڴ棬����Ϊ1B����߹���1M�ڴ�
	*/
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;		/* limit_high�ĵ�4λ�Ƕν��޸�4λ������λ��Ȩ��(����)��4λ */
};
struct GATE_DESCRIPTOR {
	/* IDT���жϼ�¼�����ݽṹ��ÿ����¼8�ֽ� */
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void);																				// ��ʼ��GDT��IDT
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);				// ���ö�������
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);					// ������������
#define ADR_IDT			0x0026f800						/* ȫ�ֶκż�¼����ַ */
#define LIMIT_IDT		0x000007ff						/* IDT��С2KB */
#define ADR_GDT			0x00270000						/* GDT��ַ */
#define LIMIT_GDT		0x0000ffff						/* GDT��С64KB */
#define ADR_BOTPAK		0x00280000						/* bootpack.kal��ַ */
#define LIMIT_BOTPAK	0x0007ffff						/* bootpack.kal��С512KB */
#define AR_DATA32_RW_R0	0x4092							/* (��8λ1001 0010)�����ڴ��У�r0��������/���ݶΣ��ɶ�д����ִ�У�δ�����ʣ���һ����(�ں�) */	/* 9-12λû��ʹ�ã�������Զ��0����ͬ */
#define AR_DATA32_RW_R1	0x40b2							/* ͬ�ϣ�r1�� */
#define AR_DATA32_RW_R2	0x40d2                          /* ͬ�ϣ�r2�� */
#define AR_DATA32_RW_R3	0x40f2                          /* ͬ�ϣ�r3��(Ӧ�ó���) */
#define AR_CODE32_ER_R0	0x409a							/* (��8λ1001 1010)�����ڴ��У�r0��������/���ݶΣ��ɶ���ִ�в���д��δ�����ʣ���һ����(�ں�) */
#define AR_CODE32_ER_R1	0x40ba							/* ͬ�ϣ�r1�� */
#define AR_CODE32_ER_R2	0x40da							/* ͬ�ϣ�r2�� */
#define AR_CODE32_ER_R3	0x40fa							/* ͬ�ϣ�r3��(Ӧ�ó���) */
#define AR_LDT			0x0082
#define AR_TSS32		0x0089
#define AR_INTGATE32	0x008e

//int.c(�жϿ���)
void init_pic(void);							// ��ʼ���ж�
void inthandler27(int *esp);					// 27���жϣ��������ÿ���ȥ�����￴
#define PIC0_ICW1		0x0020					// ��PIC�ĳ�ʼ���������ݣ�1�ţ��̶�ֵ
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021					// ��PIC���ж����μĴ�����ַ
#define PIC0_ICW2		0x0021					// ��PIC�ĳ�ʼ���������ݣ�2�ţ����ֵ���������趨PIC�жϺţ�����Ϊ�˱�����ң���ʱ����
#define PIC0_ICW3		0x0021					// ��PIC�ĳ�ʼ���������ݣ�3�ţ��̶�ֵ
#define PIC0_ICW4		0x0021					// ��PIC�ĳ�ʼ���������ݣ�4�ţ��̶�ֵ
#define PIC1_ICW1		0x00a0					// ��PIC�ĳ�ʼ���������ݣ�1�ţ��̶�ֵ
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1					// ��PIC���ж����μĴ�����ַ
#define PIC1_ICW2		0x00a1					// ��PIC�ĳ�ʼ���������ݣ�2�ţ����ֵ���������趨PIC�жϺţ�����Ϊ�˱�����ң���ʱ����
#define PIC1_ICW3		0x00a1					// ��PIC�ĳ�ʼ���������ݣ�3�ţ��̶�ֵ
#define PIC1_ICW4		0x00a1					// ��PIC�ĳ�ʼ���������ݣ�4�ţ��̶�ֵ

//fifo.c(FIFO������)
struct FIFO32 {
	int *buf;									// ��������ַ
	int p, q, size, free, flags;				// p����һ������д���ַ��q����һ�����ݶ����ַ��size�ǻ�������С��free�ǻ����������ֽ�����flags�������־(Ϊ1ʱ�������)
	struct TASK *task;							// ������д��ʱ��Ҫ���ѵ�����
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);						// ��ʼ��FIFO������
int fifo32_put(struct FIFO32 *fifo, int data);														// ��FIFO�����ݲ�����
int fifo32_get(struct FIFO32 *fifo);																// ��FIFO���һ������
int fifo32_status(struct FIFO32 *fifo);																// ��ѯ������״̬

//mouse.c(������)
struct MOUSE_DEC {
	unsigned char buf[3], phase;				// buf[3]�����ݻ�������P/S2���һ�δ���3�ֽ����ݣ�phase�ǽ��ܵڼ��ֽڵ�����
	int x, y, btn;								// x,y��btn�ǽ���������ݺ��õ�����Ͱ�ť����
};
void inthandler2c(int *esp);																		// �������ж�
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);							// �������
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);										// ��ȡ���������ֽ���Ϣ��������

//keyboard.c(���̿���)
void inthandler21(int *esp);																		// ���̼����ж�
void wait_KBC_sendready(void);																		// �ȴ����̿��Ƶ�·׼�����
void init_keyboard(struct FIFO32 *fifo, int data0);													// ��ʼ�����̿��Ƶ�·
void reset_cpu(void);																				// ��λCPU(8042����)
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

//memory.c(�ڴ����)
#define MEMMAN_FREES		4090	/* ��Լ32KB */
#define MEMMAN_ADDR			0x003c0000		/* �ڴ��������ַ */
struct FREEINFO {	/* ������Ϣ */
	unsigned int addr, size;
};
struct MEMMAN {		/* �ڴ���� */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);											// ����CPU���ͣ��Լ��ڴ�����
void memman_init(struct MEMMAN *man);																// ��ʼ���ڴ��������
unsigned int memman_total(struct MEMMAN *man);														// ��ȡʣ���ڴ��С
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);									// �����ڴ�
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);							// �ͷ��ڴ�
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);								// 4K��ռ�����ڴ�
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);						// 4K��ռ��ͷ��ڴ�

//sheet.c(����ͼ�㴦��)
#define MAX_SHEETS			256									// ���ͼ����(������ꡢ���桢��������)
#define SHEET_NO_USE		0									// ʹ��״̬��־(δʹ��)
#define SHEET_USE			1									// ʹ��״̬��־(����ʹ��)
#define SHEET_APIWIN		2									// ʹ��״̬��־(�ⲿAPI����)
#define SHEET_CONS			3									// ʹ��״̬��־(�����д���)
#define SHEET_NO_TITLE		4									// ʹ��״̬��־(�����ޱ���������)
#define SHEET_DEBUG_CONS	5									// ʹ��״̬��־(DEBUG�����д���)
#define	SHEET_BACK			101									// ʹ��״̬��־(������)
#define	SHEET_TASKBAR		102									// ʹ��״̬��־(��������)
#define	SHEET_MOUSE			103									// ʹ��״̬��־(���ָ���)
#define	SHEET_MENU			104									// ʹ��״̬��־(�˵�����)
#define	SHEET_BUTTON		105									// ʹ��״̬��־(��ť��)
struct SHEET {
	/* ͼ��ṹ */
	unsigned int *buf;										// ͼ������
	int bxsize, bysize, vx0, vy0;							// bxsize��bysize��ͼ�������С��vx0��vy0��ͼ���ڻ����ϵ�����λ��(v��vram��ʡ��)
	int col_inv, height, flags;								// col_inv��͸��ɫɫ�ţ�height��ʾͼ��߶ȣ�flags��ʾͼ���趨��Ϣ
	struct SHTCTL *ctl;										// ��ͼ���������SHTCTL�ṹ��
	struct SHTCTL *subctl;									// ��ͼ���������SHTCTL�ṹ�壬����SHEET���ڲ�ͼ��
	struct TASK *task;										// ��ͼ���������а󶨣���ǿ�ƽ�������ʱ����Ѱ�Ҳ���ͼ�㲢�ر�
	unsigned int *win;										// ����(���б���)�ṹ��ĵ�ַָ��
};
struct SHTCTL {
	/* ����ͼ�����Ϣ�����ṹ */
	unsigned int *vram, *map;								// vram��VRAM��ַ
	int xsize, ysize, top;									// xsize��ysizeͼ���С��top�����ͼ��(���ָ���)�߶�
	struct SHEET *fsheet;									// ��ͼ��
	struct SHEET *sheets[MAX_SHEETS];						// ����ͼ��ĵ�ַ(���ո߶���������)
	struct SHEET sheets0[MAX_SHEETS];						// �������ͼ���ͼ����Ϣ
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned int *vram, int xsize, int ysize);		// ��ʼ����
struct SHEET *sheet_alloc(struct SHTCTL *ctl);														// ���δʹ�õ���ͼ��
void sheet_setbuf(struct SHEET *sht, unsigned int *buf, int xsize, int ysize, int col_inv);			// ���û�������С
void sheet_updown(struct SHEET *sht, int height);													// ����sheet�߶�
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);							// ˢ��ͼ��(����)
void sheet_refreshall(struct SHEET *sht, int bx0, int by0, int bx1, int by1);						// (δ���)����ͼ�㵽��ͼ��ȫ��ˢ��
void sheet_slide(struct SHEET *sht, int vx0, int vy0);												// �ƶ�ͼ��
void sheet_free(struct SHEET *sht);																	// �ͷ���ʹ�õ�ͼ���ڴ�

//graphic.c(������ʾ)                                                                                  
//void init_palette(void);																			// ��ʼ����ɫ�庯��
//void set_palette(int start, int end, unsigned char *rgb);											// ���õ�ɫ��
void boxfill8(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1);		// ���Ʒ���
void init_screen8(int *vram, int x, int y, int bc);													// ��ʼ����Ļ
void putfont8(int *vram, int xsize, int x, int y, int c, char *font);								// ��������
void putfonts8_asc(int *vram, int xsize, int x, int y, int c, unsigned char *s);					// �����ַ���
void init_mouse_cursor8(int *mouse, int bc);														// ��ʼ�����ָ��
void putblock8_8(int *vram, int vxsize, int pxsize,                                                    
	int pysize, int px0, int py0, int *buf, int bxsize);											// ��걳��ɫ����
int read_image_32(char *filename, int x0, int y0, int width, int *fat, unsigned int *vram);			// 32λɫ��ģʽ�¶�ȡͼƬ
void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);					// ����һ��ֱ��(ϵͳ����)
// 15����ɫ�������壬��ϵͳ֧��RGBȫ��ɫ�����Կ���ʹ��0xRGB(��ͨ��RGB��ʾ����)����ʾ��ɫ
#define COL_BLACK		0x00000000
#define COL_BRED		0x00ff0000
#define COL_BGREEN		0x0000ff00
#define COL_BYELLOW		0x00ffff00
#define COL_BBLUE		0x000000ff
#define COL_BPURPLE		0x00ff00ff
#define COL_LBBLUE		0x0000ffff
#define COL_WHITE		0x00ffffff
#define COL_BGREY		0x00c6c6c6
#define COL_DRED		0x00840000
#define COL_DGREEN		0x00008400
#define COL_DYELLOW		0x00848400
#define COL_DBLUE		0x00000084
#define COL_DPURPLE		0x00840084
#define COL_LDBLUE		0x00008484
#define COL_DGREY		0x00848484

//timer.c(��ʱ��)
#define MAX_TIMER		500
struct TIMER {
	struct TIMER *next;									// ��һ����ʱ��ʱ��
	unsigned int timeout;								// ��ʱʱ��
	char flags, flags2;
	struct FIFO32 *fifo;
	int data;
};
struct TIMERCTL {
	unsigned int count, next, using;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
#define TIMER_FLAGS_ALLOC		1																	// ������״̬
#define TIMER_FLAGS_USING		2																	// ��ʱ��������
extern struct TIMERCTL timerctl;
struct TIMER *timer_alloc(void);
void init_pit(void);																				// ��ʼ���ɱ�̼������ʱ��(PIT)
void timer_free(struct TIMER *timer);																// �ͷŶ�ʱ��
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);								// ��ʼ����ʱ��
void timer_settime(struct TIMER *timer, unsigned int timeout);										// ���ö�ʱ��
void inthandler_timer(int *esp);																	// ��ʱ���ж�
int timer_cancel(struct TIMER *timer);																// ȡ����ʱ��
void timer_cancelall(struct FIFO32 *fifo);															// ȡ�����ж�ʱ��
void timer_sleep(int time);

/* signal.c(�źŴ���) */
typedef int sig_atomic_t;
typedef unsigned int sigset_t;
#define MAX_SIG				32					// �ź���������(���)
#define SIGHUP				 1					// �Ҷ�����
#define SIGINT				 2                  // �����ж�
#define SIGQUIT				 3                  // �����˳�(Shift+F1)
#define SIGILL				 4                  // �Ƿ�ָ��(0x06��exception)
#define SIGTRAP				 5                  // ���ٶϵ�
#define SIGABRT				 6                  // �쳣����
#define SIGIOT				 6                  // �쳣����
#define SIGUNUSED			 7                  // ����
#define SIGFPE				 8                  // Э����������(0x07��exception)
#define SIGKILL				 9                  // ǿ�Ƚ�����ֹ
#define SIGUSR1				10                  // �û��ź�1�����̿�ʹ��
#define SIGSEGV				11                  // ��Ч�ڴ�����
#define SIGUSR2				12                  // �û��ź�2�����̿�ʹ��
#define SIGPIPE				13                  // �ܵ�д������û��reader
#define SIGALRM				14                  // RTC Alarm
#define SIGTERM				15                  // ������ֹ
#define SIGSTKFLT			16                  // ջ����(0x0c��exception)
#define SIGCHLD				17                  // �ӽ���ֹͣ(��ֹ)
#define SIGCONT				18                  // �ָ����̼���ִ��
#define SIGSTOP				19                  // ֹͣ����ִ��
#define SA_NOCLDSTOP		1					// �ӽ��̴���ֹͣ״̬���򲻶�SIGCHLD����
#define SA_NOMASK			0x40000000			// ����ֹ��ָ�����źŴ�������(�źž��)���ٴν��յ����ź�
#define SA_ONESHOT			0x80000000			// �źž��һ�������ù��ͻָ�Ĭ�ϴ������
struct SIGACTION {
	void (*sa_handler)(int);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_restorer)(void);
};
int signal(int signum, int handler, int restorer);													// Ϊָ���ź������źŴ�������
int sigaction(int signum, struct SIGACTION *action, struct SIGACTION *oldaction);					// �ı�����յ��ź�ʱ�Ĳ���
void do_signal(int sig_num, int eax, int ebx,
	int ecx, int edx, int fs, int es, int ds,
	int eip, int cs, int eflags, unsigned int *esp, int ss);										// (δ���)�źŴ�������(TODO)

/* mtask.c(������) */
#define MAX_TASKS		250							// �����������
#define TASK_GDT0		3							// �����GDT�ļ��ſ�ʼ�����TSS
#define MAX_TASKS_LV	25							// ÿ�����������
#define MAX_TASKLEVELS	10							// ����������(321ҳ)
#define MAX_DIRLENGTH	256							// ���������·������
/*������Щ������״̬�� */
#define	TASK_UNUSED					0					/* δʹ�� */
#define TASK_UNINTERRUPTIBLE		1					/* �����ж�˯��״̬ */
#define TASK_RUNNING				2					/* �������� */
#define	TASK_INTERRUPTIBLE			3					/* ���ж�˯��״̬ */
#define	TASK_ZOMBIE					4					/* ����״̬ */
#define	TASK_STOPPED				5					/* ��ֹͣ */
/*************************************************/
struct TASK_SEGMENT_ADDR {
	/* ��¼������ضε�ַ */
	unsigned int start_code;						// ����ε�ַ
	unsigned int end_code;							// ���볤��(�ֽ�)
	unsigned int end_data;							// ���볤��+���ݳ���
	unsigned int brk;								// �ܳ���(�ֽ�)
	unsigned int start_stack;						// ��ջ��ַ
};
struct TASK_ID_NUM {
	/* ������ر�ʶ�� */
	int pid;                                        // ���̺�
	int father;                                     // �����̺�
	int pgrp;                                       // �������
	int session;                                    // �Ự��
	int leader;                                     // �Ự����
};
struct USER_ID {
	/* �û���ر�ʶ��(TODO�����Ժ������û�����������������) */
	unsigned int uid;								// �û�id(��ʶ��)
	unsigned int euid;								// ��Ч�û�id
	unsigned int suid;								// ������û�id
	unsigned int gid;								// ��id(��ʶ��)
	unsigned int egid;								// ��Ч��id
	unsigned int sgid;								// ������id
};
struct TASK_TIME {
	/* �������ʱ���¼ */
	unsigned int utime;								// (��������,��ͬ)�û�̬����ʱ��(tik,��ͬ)
	unsigned int stime;								// ϵͳ̬����ʱ��
	unsigned int cutime;							// �ӽ����û�̬����ʱ��
	unsigned int cstime;							// �ӽ���ϵͳ̬����ʱ��
	unsigned int start_time;						// ���̿�ʼʱ��
};
struct TSS32 {
	/* ����״̬��(104�ֽ�)�������л�����ʱ����Ĵ������� */
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;		// 32λ�Ĵ���
	int es, cs, ss, ds, fs, gs;										// 16λ�Ĵ���
	int ldtr, iomap;
};
struct TASK {
	int sel, flags; 										// sel�������GDT��ţ�flags������״̬��������Զ��岻ͬ������״̬
	int level, priority;									// priority�ǽ������ȼ�
	struct FIFO32 fifo;										// ����FIFO���������������Ҫ�Ժ�Ҳ���ԼӸ�list(˫����)
	struct TSS32 tss;										// ����״̬��
	int fpu[108 / 4];										// TASKʹ��FPU�Ĵ���ʱ�Ĵ洢λ�úͶ�ȡԴ
	struct SEGMENT_DESCRIPTOR ldt[2];						// LDT������
	struct CONSOLE *cons;									// �����Ӧ��console
	int ds_base, cons_stack;								// ���ݶλ�ַ
	struct FILEHANDLE *fhandle;								// �ļ��������������
	int *fat;												// �ļ�fat��������
	char *cmdline;											// �����¼
	unsigned char langmode, langbyte1;						// ����ģʽ����
	/* TODO */
	char dir[MAX_DIRLENGTH];								// ����������·��
	struct SIGACTION sigaction[MAX_SIG];					// �źŴ�����������ͱ�־��Ϣ
	int signal;												// �źţ�ÿһ��bit����һ���ź�
	struct TASK_TIME task_time;								// �����������ʱ��
	char *task_name;										// ��������
	int blocked;											// �����ź�������
	int exit_code;											// ִ��ֹͣ���˳��룬���������̴���
	struct TASK_SEGMENT_ADDR task_segment_addr;				// ��¼������ض�(���ݶΡ�����ε�)��ַ
	struct TASK_ID_NUM task_id_num;							// ������ر�ʶ��
	struct USER_ID user_id;									// �û��������id
};
struct TASKLEVEL {
	int running; /* �������е��������� */
	int now; /* ��ǰ���е����� */
	struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; 									/* ���ڻ�е�����ȼ� */
	struct TASK *task_fpu;							/* ��¼��ǰFPU�Ĵ��������ĸ����� */
	char lv_change; 								/* �´������л�ʱ�Ƿ���Ҫ�ı�level */
	struct TASKLEVEL level[MAX_TASKLEVELS];			/* ����level�ṹ������ */
	struct TASK tasks0[MAX_TASKS];					/* ����ṹ������ */
};
extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;
extern struct TASK *idle;
void task_add(struct TASK *task);																	// ��struct TASKLEVEL������һ������
void task_remove(struct TASK *task);																// ��struct TASKLEVEL��ɾ��һ������
struct TASK *task_init(struct MEMMAN *memman);														// ��ʼ������
struct TASK *task_alloc(void);																		// �������
struct TASK *task_now(void);																		// �����������ڻ�е�TASK�ṹ���ַ
void task_run(struct TASK *task, int level, int priority);											// ���г���
void task_switch(void);																				// �л�����
void task_sleep(struct TASK *task);																	// ����˯��
int *inthandler_device_not_available(int *esp);														// ��ѧЭ������������

/* window.c(���ڻ���) */
#define TIT_ACT_DEFAULT		0x00ffc1c1				// Ĭ�ϴ��ڱ�����ѡ����ɫ	
#define TIT_DEACT_DEFAULT	0x00cd9b9b				// Ĭ�ϴ��ڱ�����δѡ����ɫ
struct WINCOLORS {
	/* ����(����)��ɫ */
	int act_color,dis_act_color,back_color;			// ������ɫ��act_color��key_on̬��ɫ��dis_act_color��key_off̬��ɫ
};
struct WINDOW {
	/* ����ṹ�崢�洰����Ϣ */
	unsigned int *buf;								// ����ͼ�λ�����
	char *wtitle;									// ���ڱ���
	int xsize,ysize;								// ���ڴ�С
	struct WINCOLORS wcolor;						// ������ɫ
	int whandle;									// ���ھ��(������������SHEET�ṹ��)
	int tskwinbtn;									// ��������ťindex
};
// struct WINDOW *make_window8(unsigned int *buf, int xsize, int ysize, char *title, char act);		// ����һ������(��)
struct WINDOW *make_window8(struct SHEET *sht, int xsize, int ysize,
	int act_color, int deact_color, char *title, char act);											// ����һ������
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);				// ��Ϳ����ɫ����д�ַ���
// void make_wtitle8(unsigned int *buf, int xsize, char *title, char act);							// ����һ��������(�ɰ�)
void make_wtitle8(struct WINDOW *window, char act);													// ����һ��������
void change_wtitle8(struct WINDOW *window, char act);												// �ı䴰�ڱ�������ɫ
// void make_icon(unsigned int *buf, int xsize, char type);											// ��ʾһ��logo(�ɰ�)
void make_icon(struct WINDOW *window, char type);													// ��ʾһ��logo

/* button.c(��ť) */
struct BUTTON {
	/* ��ť�ṹ�� */
	char *title;									// ��ť����
	int buttonx, buttony;							// ��ťλ��
	int height,width;								// ��ť��С(�߿�)
	int flags;										// ��ť״̬
	int back_color;									// ��ť����ɫ
	struct SHEET *sht;								// ��ťͼ��sheet
	char click_old;									// ��һ���Ƿ���(1Ϊ���£�0Ϊδ��)
	char show;										// �Ƿ�������
	void(*onButtonClick)();							// ���ʱִ��
};
struct BUTTON *make_button(struct MEMMAN *memman, int width, int height,
	int buttonx, int buttony, char *title, int back_color, void(*onButtonClick)());					// ����һ����ť
void show_button(struct SHEET *sht, struct MEMMAN *memman, struct BUTTON *button);					// ���ư�ť
void change_button(struct BUTTON *button, struct SHEET *fsht, char click);							// ���İ�ť͹��Ͱ���(�����)
void click_button(struct BUTTON *button);															// �����ť
void hide_button(struct BUTTON *button);															// ���ذ�ť
void release_button(struct BUTTON *button);															// �ͷŰ�ť

/* menu.c(�˵���) */
#define	MAX_OPTIONS			255						// ���ѡ������
struct OPTIONS {
	/* �˵���ѡ��ṹ�� */
	int flags;										// ѡ���־
	char *title;									// ѡ�����
	unsigned char index;							// ѡ��index
	void(*onOptionClick)();							// ���ʱִ��
};
struct MENU {
	/* �˵����ṹ�� */
	int menux,menuy;								// �˵�������
	int mheight,mwidth;								// �˵�����С
	unsigned int *buf;								// �˵�ͼ�񻺳��ַ
	struct SHEET *sht;								// �˵�ͼ��sheet
	int flags;										// flags�ǲ˵���ǰ״̬
	unsigned char now, old;							// now�ǵ�ǰѡ����,old����һ��ѡ����
	int option_num;									// ��ǰѡ������
	struct OPTIONS options[MAX_OPTIONS];			// ���ѡ���б�(���256��ѡ�Ӧ�ò������ĸ������õ�200���ѡ��)
};
struct MENU *make_menu(struct MEMMAN *memman, int menux, int menuy);								// �����˵���
void release_menu(struct MEMMAN *man, struct MENU *menu);											// �ͷŲ˵���
// void add_options(struct MENU *menus, char *option_title);										// ����ѡ��(�ɰ�)
void add_options(struct MENU *menu, char *option_title, void(*onOptionClick)());					// ����ѡ��
void remove_options(struct MENU *menu, unsigned char index);										// �Ƴ�ѡ��
void show_menu(struct SHTCTL *shtctl, struct MEMMAN *memman, struct MENU *menu);					// ��ʾ�˵�
void hide_menu(struct MEMMAN *memman, struct MENU *menu);											// ���ز˵�
void option_change(struct MENU *menu, int mouse_y);													// ����ƶ�ʱѡ���ɫ����
void menu_click(struct MENU *menu, int mouse_y);													// �˵���������

/* textbox.c(�����) */
struct TEXTBOX {
	int x,y;					/* ͼ��������� */
	int height,width;			/* �߿� */
	struct SHEET *sht;			/* ������ͼ�� */
	char flags;					/* �����״̬��ʶ(bitmap,��1λ��ʶ�Ƿ񼤻��2λ��ʾ�Ƿ���ӣ���3λ��ʶ�Ƿ����) */
	int cur_x, cur_y, cur_c;	/* ���λ��(���)��״̬(cur_c) */
	// char *buf;				/* (��sheet�е�һ�������Բ��õ�������)�����������ݵĻ����� */
	int char_num;				/* ����ַ����� */
	int back_color;				/* ������ɫ */
};
void make_textbox_old(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);						// (�ɰ�)���������
struct TEXTBOX *make_textbox(struct SHTCTL *shtctl, int x0, int y0, int height,
	int width, int back_color, unsigned int *buf, int char_num);										// ���������
void show_textbox(struct TEXTBOX *textbox);																// ��ʾtextbox

/* taskbar.c(�׶�������) */
#define TASKBAR_ADDR	0x30100			/* ������ͼ���ַ */
#define TASKBARCTL_ADDR	0x30200			/* �����������ṹ���ַ */
#define MAX_TSKWINBTN	256				/* ���ͬʱ��ʾ256��������taskbar�� */
struct TSKWINBTN {
	/* ÿ����������ťѡ�� */
	int index;					/* �������������İ�ť��index */
	int flags;					/* ��ǰ�ṹ���ʹ��״̬(δʹ�á���С�����򿪷ֱ�Ϊ0��1��2) */
	int row;					/* �������������ڼ���(�ӵ�0�ſ�ʼ��ÿ�����7������) */
	struct WINDOW *win;			/* ��Ӧ�Ĵ��� */
	struct BUTTON *button;		/* ��Ӧ����������ť */
};
struct TASKBARCTL {
	/* �����������ṹ�� */
	int num;										/* �������� */
	int now_row,total_row;							/* ��ǰ��ʾ������������-1 */
	struct MENU *menu;								/* ��ʼ�˵� */
	struct BUTTON *button;							/* Function��ť */
	struct TSKWINBTN tskwinbtns[MAX_TSKWINBTN];		/* ��������ťѡ��ṹ�� */
};
void init_taskbar(struct MEMMAN *memman, struct SHEET *sht);					// ��ʼ��������
int taskbar_addwin(struct WINDOW *window);										// ������������һ�����ڰ�ť
void taskbar_removewin(int index);												// ��������ɾ��һ����ť

/* exit.c(�������ʱ�Ĵ���) */
void do_exit(int *esp);										// (δ���)�˳�ʱ��ش���
void tell_father(int pid);									// (δ���)�򸸽��̷���SIGCHLD�ź�
int task_kill(int pid, int sig);							// (δ���)�����(����)���鷢���ź�
void task_kill_session(void);								// ��ֹ�Ự

/* fork.c(���ƽ���) */
int copy_mem(int task_num, struct TASK *task);									// (δ���)���̸���ʱ������ڴ渴��
int copy_process(unsigned int fsize, unsigned int segsiz);						// (δ���)���ƽ���

/* console.c(������) */
#define DEBUG_ADDR		0x30000							// DEBUG consoleλ��
#define DEBUG_WAR_STR	"cannot run this command in debug console\n"
struct CONSOLE {
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
	struct TIMER *timer;
};
struct FILEHANDLE {
	char *buf;
	int size;
	int pos;
};
void console_task(struct SHEET *sheet, unsigned int memtotal);										// ���������
void cons_newline(struct CONSOLE *cons);															// ����ڻ���
void cons_putchar(struct CONSOLE *cons, int chr, char move, int font_color, int back_color);		// �����������ʾ����
void cons_putstr0(struct CONSOLE *cons, char *s);													// ��ʾ�ַ���(ͨ���ַ�����0��β)
void cons_putstr1(struct CONSOLE *cons, char *s, int l);											// ��ʾ�ַ���(ͨ��ָ������)
void cons_col_putstr0(struct CONSOLE *cons, char *s, int font_color, int back_color);				// ��ʾ�Զ�����ɫ�ͱ������ַ���(ͨ���ַ�����0��β)
void cons_col_putstr1(struct CONSOLE *cons, char *s, int l, int font_color, int back_color);		// ��ʾ�Զ�����ɫ�ͱ������ַ���(ͨ��ָ������)
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat,
	unsigned int memtotal, char sysmode);															// ִ������
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);											// CMD����ѯ�ڴ�ʹ��״̬
void cmd_cls(struct CONSOLE *cons);																	// CMD������
void cmd_dir(struct CONSOLE *cons, char *parameter, int *fat);										// CMD����ѯĿ¼�ļ�
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);										// CMD����ʾ�ļ�����
void cmd_exit(struct CONSOLE *cons, int *fat);														// CMD���ر������
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD�����µ�������������һ������
void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD���ڵ�ǰ�����ִ������Ҳ�ռ�������
void cmd_langmode(struct CONSOLE *cons, char *cmdline);												// CMD���л�����ģʽ
void cmd_shutdown(void);																			// CMD���ػ�
void cmd_sysmode(struct CONSOLE *cons, char *cmdline);												// CMD���л�ϵͳģʽ
void cmd_echo(struct CONSOLE *cons, char *cmdline);													// CMD��ϵͳ���
void cmd_getruntime(struct CONSOLE *cons);															// CMD����ѯϵͳ��������ʱ��
void cmd_sysinfo(struct CONSOLE *cons, unsigned int memtotal);										// CMD�����ϵͳ�����Ϣ
void cmd_pwd(struct CONSOLE *cons);																	// CMD���鿴��ǰ������·��
void cmd_cd(struct CONSOLE *cons, char *parameter, int *fat);										// CMD���л�������Ŀ¼
void cmd_ps(struct CONSOLE *cons);																	// CMD���鿴��������״̬
void cmd_testfunc(struct CONSOLE *cons, int *fat);													// ���ܲ���ר��
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);											// �ⲿӦ�ó���
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal, int debug);				// ����һ�������
struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal);								// ����һ������
void keywin_off(struct SHEET *key_win);																// ���ƴ��ڱ�������ɫ�͹�꼤��״̬
void keywin_on(struct SHEET *key_win);																// ���ƴ��ڱ�������ɫ�͹�꼤��״̬
void close_console(struct SHEET *sht);																// �ر������
void close_constask(struct TASK *task);																// ��������
void cons_printf(struct CONSOLE *cons, char *format, ...);											// ��ʽ�������ָ��cons����
void debug_print(char *format, ...);																// �����DEBUG����

/* kal_api.c(KalinoteOS Ӧ�ó���API�ӿ�) */
int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);				// ͨ��edx����API

/* exception.c(�쳣�жϴ���) */
#define WARMSG_CH	"********************%s********************\n   ϵͳ�ڳ�������Ӧ��ʱ�����˴���\n   %s����ô����һ�γ��֣��볢������������Ӧ�ó�������ô��󷴸����֣�����ϵ�����Ŀ����ߡ�\n   �����Ǵ˴δ������Ϣ��\n"
#define WARMSG_EN	"********************%s********************\n   The system encountered an error while trying to run the application. \n    %s If this error occurs for the first time, please try to restart the application. If this error occurs repeatedly, please contact the software developer. \n the following is the error message:\n"
int *inthandler_divzero(int *esp);																	// 00���жϣ����ڴ��������쳣
int *inthandler_stack_segment(int *esp);															// 0c���жϣ����ڴ���ջ�쳣
int *inthandler_general_protection(int *esp);														// 0d���жϣ����ڴ���һ���쳣

/* fat12.c(�ļ��������ļ�ϵͳ) */
#define FILE_NOINFO			0x00						// û���ļ���Ϣ
#define FILE_DELETED		0xe5						// �Ѿ���ɾ����
#define FILE_READONLY		0xfe						// ֻ���ļ�
#define FILE_SYSTEM			0xfb						// ϵͳ�ļ�
#define FILE_DIR			0xef						// Ŀ¼
#define FILE_NORMAL			0xdf						// ��ͨ�ļ�
#define ADR_FAT				(ADR_DISKIMG+0x200)			// FAT��ַ
struct FILEINFO {
	//�ļ��ṹ(�����367ҳ)
	unsigned char name[8], ext[3], type;
	char reserve[10];									// ����λ
	unsigned short time, date, clustno;					// ����ӳ���еĵ�ַ = clustno * 512 +0x003e00
	unsigned int size;
};
struct buf_head {
	/* ������ͷ���ݽṹ */
	char *data;								/* ָ�룬ָ�����ݿ�(1024�ֽ�) */
	unsigned long blocknum;					/* ��� */
	unsigned short dev;						/* �豸�� */
	unsigned char uptodate;					/* �����Ƿ���� */
	unsigned char dirt;						/* 0-clean,1-dirty(�����޸ı�־) */
	unsigned char count;					/* ʹ�õ��û��� */
	unsigned char lock;						/* 0 - ok, 1 -locked(�������Ƿ�����) */
	struct TASK *wait;						/* ָ��ȴ����������������� */
	struct buf_head *prev;                  /* ��һ���� */
	struct buf_head *next;                  /* ��һ���� */
	struct buf_head *prev_free;             /* ��һ�����п� */
	struct buf_head *next_free;             /* ��һ�����п� */
};
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);							// �����ļ�
void file_readfat(int *fat, unsigned char *img);													// ����FAT
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);							// �����ļ�
char *file_loadfile2(int clustno, int *psize, int *fat);											// ����kcaѹ���ļ�
struct FILEINFO *dir_search(char *name, struct FILEINFO *finfo, int max);							// ��finfo��������Ϊname��Ŀ¼
struct FILEINFO *dir_check(char *dir, int *fat);													// ͨ������·����ȡ�ļ���Ϣ
// int file_delete(int *fat, struct FILEINFO *file);																// ɾ���ļ�
// void file_writefat(int *fat, int no, int data);														// дfat��
// int fat_flush(void);																				// ��fat������д��FDC��

/* jpeg.c(����jpgͼƬ) */
struct DLL_STRPICENV{
	int work[64 * 1024 / 4];
};
struct RGB{
	unsigned char b,g,r,t;
};
int info_JPEG(struct DLL_STRPICENV *env,int *info, int size,unsigned char *fp);												// ��ȡJPGͼƬ��Ϣ
int decode0_JPEG(struct DLL_STRPICENV *env,int size,unsigned char *fp,int b_type,unsigned char *buf,int skip);				// JPGͼƬ����

/* bootpack.c */
#define KEYCMD_LED		0xed

/* clock.c(ʱ��) */
void clock_task(struct SHEET *sht);												// ʱ��������������
void clock_taskinit(struct TASK *task, struct SHEET *sht);						// ʱ�������ʼ��
void rtc_syn(void);																// ��cmosͬ��ʱ��

/* kca.c(��ѹKCAѹ���ļ�) */
int kca_getsize(unsigned char *p);
int kca_decomp(unsigned char *p, char *q, int size);

/* cmos.c(cmos�˿ڲ���) */
#define cmos_index 0x70
#define cmos_data 0x71
/* cmos�������Ϣƫ�� */
#define CMOS_CUR_SEC	0x0							// ʵʱ����ֵ
#define CMOS_ALA_SEC	0x1							// ������ֵ
#define CMOS_CUR_MIN	0x2							// ʵʱ�ӷ�ֵ
#define CMOS_ALA_MIN	0x3							// ������ֵ
#define CMOS_CUR_HOUR	0x4							// ʵʱ��Сʱֵ
#define CMOS_ALA_HOUR	0x5							// ����Сʱֵ
#define CMOS_WEEK_DAY	0x6							// ʵʱ�ӣ�һ���е���
#define CMOS_MON_DAY	0x7							// ʵʱ�ӣ�һ���е���
#define CMOS_CUR_MON	0x8							// ʵʱ���·�
#define CMOS_CUR_YEAR	0x9							// ʵʱ�����
#define CMOS_DEV_TYPE	0x12						// Ӳ������������
#define CMOS_CUR_CEN	0x32						// ��ǰ����ֵ
#define BCD_HEX(n)	((n >> 4) * 10) + (n & 0xf)
#define BCD_ASCII_first(n)	(((n<<4)>>4)+0x30)
#define BCD_ASCII_S(n)	((n<<4)+0x30)
unsigned int get_hour_hex();						// ȡ��ǰʱ��(Сʱ)
unsigned int get_min_hex();							// ȡ��ǰʱ��(����)
unsigned int get_sec_hex();							// ȡ��ǰʱ��(��)
unsigned int get_day_of_month();					// ȡ��ǰ����
unsigned int get_day_of_week();						// ȡ��ǰ����
unsigned int get_mon_hex();							// ȡ��ǰ�·�
unsigned int get_year();							// ȡ��ǰ���
unsigned char cmos_hd_info();						// ��ȡӲ����Ϣ

/* list.c(˫�����ṹ) */
struct list_elem {
	/* list�ڵ㣬���ڱ���ǰһ���ڵ�ͺ�һ���ڵ�ĵ�ַ */
	struct list_elem* prev; // ǰ�����
	struct list_elem* next; // ��̽��
};
struct list {
	/* head�Ƕ���,�ǹ̶�����ģ����ǵ�1��Ԫ��,��1��Ԫ��Ϊhead.next */
	struct list_elem head;
	/* tail�Ƕ�β,ͬ���ǹ̶������ */
	struct list_elem tail;
};
void list_init(struct list* list);												// ��ʼ������
void list_insert_before(struct list_elem* before, struct list_elem* elem);		// (δ���)������Ԫ��elem���뵽before֮ǰ
void list_push(struct list* plist, struct list_elem* elem);						// (δ���)����Ԫ�ص��б�����,����ջpush����
void list_append(struct list* plist, struct list_elem* elem);					// (δ���)׷��Ԫ�ص�������β,���ƶ��е��Ƚ��ȳ�����
void list_remove(struct list_elem* pelem);										// (δ���)ʹԪ��pelem��������
struct list_elem* list_pop(struct list* plist);									// (δ���)��������һ��Ԫ�ص���������,����ջ��pop����
int elem_find(struct list* plist, struct list_elem* obj_elem);					// (δ���)�������в���Ԫ��obj_elem,�ɹ�ʱ��������λ��,ʧ��ʱ����-1
int list_empty(struct list* plist);												// (δ���)�ж������Ƿ�Ϊ��,��ʱ����1,���򷵻�0

/* util.c(ͨ�ù��߰�) */
int read_rtc(unsigned char tt[5]);

/* acpi.c(��Դ����) */
struct ACPI_RSDP {
	/* RSDP�ṹ */
	char Signature[8];
	unsigned char Checksum;
	char OEMID[6];
	unsigned char Revision;
	unsigned int RsdtAddress;
	/* ������1.0�Ľṹ��������2.0����չ�ṹ	*/
	unsigned int Length;
	unsigned int XsdtAddress[2];
	unsigned char ExtendedChecksum;
	unsigned char Reserved[3];
};
struct ACPISDTHeader {
	/* RSDP�ṹ */
	char Signature[4];
	unsigned int Length;
	unsigned char Revision;
	unsigned char Checksum;
	char OEMID[6];
	char OEMTableID[8];
	unsigned int OEMRevision;
	unsigned int CreatorID;
	unsigned int CreatorRevision;
};
struct ACPI_RSDT {
	struct ACPISDTHeader header;
	unsigned int Entry;
};
typedef struct {
	unsigned char AddressSpace;
	unsigned char BitWidth;
	unsigned char BitOffset;
	unsigned char AccessSize;
	unsigned int Address[2];
} GenericAddressStructure;
struct ACPI_FADT {
	/* Fixed ACPI Description Table���ݽṹ */
    struct   ACPISDTHeader h;
    unsigned int FirmwareCtrl;
    unsigned int Dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    unsigned char  Reserved;
 
    unsigned char  PreferredPowerManagementProfile;
    unsigned short SCI_Interrupt;
    unsigned int SMI_CommandPort;
    unsigned char  AcpiEnable;
    unsigned char  AcpiDisable;
    unsigned char  S4BIOS_REQ;
    unsigned char  PSTATE_Control;
    unsigned int PM1aEventBlock;
    unsigned int PM1bEventBlock;
    unsigned int PM1aControlBlock;
    unsigned int PM1bControlBlock;
    unsigned int PM2ControlBlock;
    unsigned int PMTimerBlock;
    unsigned int GPE0Block;
    unsigned int GPE1Block;
    unsigned char  PM1EventLength;
    unsigned char  PM1ControlLength;
    unsigned char  PM2ControlLength;
    unsigned char  PMTimerLength;
    unsigned char  GPE0Length;
    unsigned char  GPE1Length;
    unsigned char  GPE1Base;
    unsigned char  CStateControl;
    unsigned short WorstC2Latency;
    unsigned short WorstC3Latency;
    unsigned short FlushSize;
    unsigned short FlushStride;
    unsigned char  DutyOffset;
    unsigned char  DutyWidth;
    unsigned char  DayAlarm;
    unsigned char  MonthAlarm;
    unsigned char  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    unsigned short BootArchitectureFlags;
 
    unsigned char  Reserved2;
    unsigned int Flags;
	/*
	* Flags;	
	* ״̬��־���������ݼ��·�����(ACPI_FADT_*),
	* ʹ��(acpi_gbl_FADT.flags & ACPI_FADT_*)��֤
	* ��Щ��������linuxԴ����
	*/

    // 12 byte structure; see below for details
    GenericAddressStructure ResetReg;
 
    unsigned char  ResetValue;
    unsigned char  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    unsigned int                X_FirmwareControl[2];
    unsigned int                X_Dsdt[2];
 
    GenericAddressStructure X_PM1aEventBlock;
    GenericAddressStructure X_PM1bEventBlock;
    GenericAddressStructure X_PM1aControlBlock;
    GenericAddressStructure X_PM1bControlBlock;
    GenericAddressStructure X_PM2ControlBlock;
    GenericAddressStructure X_PMTimerBlock;
    GenericAddressStructure X_GPE0Block;
    GenericAddressStructure X_GPE1Block;
};
#define ACPI_FADT_WBINVD            (1)		/* FADT Flags 00: [V1] WBINVDָ������� */
#define ACPI_FADT_WBINVD_FLUSH      (1<<1)	/* FADT Flags 01: [V1] WBINVDˢ�µ���ʹ������Ч */
#define ACPI_FADT_C1_SUPPORTED      (1<<2)	/* FADT Flags 02: [V1] ���д�������֧��C1״̬ */
#define ACPI_FADT_C2_MP_SUPPORTED   (1<<3)	/* FADT Flags 03: [V1] MPϵͳ֧��C2״̬ */
#define ACPI_FADT_POWER_BUTTON      (1<<4)	/* FADT Flags 04: [V1] ��Դ��ť������Ϊһ�ֿ��Ʒ������� */
#define ACPI_FADT_SLEEP_BUTTON      (1<<5)	/* FADT Flags 05: [V1] ˯�߰�ť������Ϊһ�ֿ��Ʒ������� */
#define ACPI_FADT_FIXED_RTC         (1<<6)	/* FADT Flags 06: [V1] RTC����״̬���ڹ̶��Ĵ����ռ��� */
#define ACPI_FADT_S4_RTC_WAKE       (1<<7)	/* FADT Flags 07: [V1] RTC�����ɽ�ϵͳ��S4���� */
#define ACPI_FADT_32BIT_TIMER       (1<<8)	/* FADT Flags 08: [V1] ACPI��ʱ������Ϊ32λ (0=24-bit) */
#define ACPI_FADT_DOCKING_SUPPORTED (1<<9)	/* FADT Flags 09: [V1] ֧�� Docking(?) */
#define ACPI_FADT_RESET_REGISTER    (1<<10)	/* FADT Flags 10: [V2] ֧��ͨ��FADT reset\ U REG����ϵͳ���� */
#define ACPI_FADT_SEALED_CASE       (1<<11)	/* FADT Flags 11: [V3] ���ڲ���չ���ܣ����δ֪ */
#define ACPI_FADT_HEADLESS          (1<<12)	/* FADT Flags 12: [V3] û�б�����Ƶ���ܻ򱾵������豸 */
#define ACPI_FADT_SLEEP_TYPE        (1<<13)	/* FADT Flags 13: [V3] д��SLP_TYPx�Ĵ��������ִ�б���ָ�� */
#define ACPI_FADT_PCI_EXPRESS_WAKE  (1<<14)	/* FADT Flags 14: [V4] ϵͳ֧��PCIEXP_���� (STS/EN) bits (ACPI 3.0) */
#define ACPI_FADT_PLATFORM_CLOCK    (1<<15)	/* FADT Flags 15: [V4] OSPMӦʹ��ƽ̨�ṩ�Ķ�ʱ�� (ACPI 3.0) */
#define ACPI_FADT_S4_RTC_VALID      (1<<16)	/* FADT Flags 16: [V4] S4���Ѻ���Ч��RTC_STS���� (ACPI 3.0) */
#define ACPI_FADT_REMOTE_POWER_ON   (1<<17)	/* FADT Flags 17: [V4] ϵͳ��Զ�̿������� (ACPI 3.0) */
#define ACPI_FADT_APIC_CLUSTER      (1<<18)	/* FADT Flags 18: [V4] ���б���APIC����ʹ��Ⱥ��ģ�� (ACPI 3.0) */
#define ACPI_FADT_APIC_PHYSICAL     (1<<19)	/* FADT Flags 19: [V4] ���б���XAPIC����ʹ������Ŀ�ĵ�ģʽ (ACPI 3.0) */
#define ACPI_FADT_HW_REDUCED        (1<<20)	/* FADT Flags 20: [V5] δʵ��ACPIӲ�� (ACPI 5.0) */
#define ACPI_FADT_LOW_POWER_S0      (1<<21)	/* FADT Flags 21: [V5] S0����Ч�����ڻ�����S3 (ACPI 5.0) */
#define ACPI_ADR_SPACE_SYSTEM_MEMORY    (unsigned char) 0
#define ACPI_ADR_SPACE_SYSTEM_IO        (unsigned char) 1
#define ACPI_ADR_SPACE_PCI_CONFIG       (unsigned char) 2
#define ACPI_ADR_SPACE_EC               (unsigned char) 3
#define ACPI_ADR_SPACE_SMBUS            (unsigned char) 4
#define ACPI_ADR_SPACE_CMOS             (unsigned char) 5
#define ACPI_ADR_SPACE_PCI_BAR_TARGET   (unsigned char) 6
#define ACPI_ADR_SPACE_IPMI             (unsigned char) 7
#define ACPI_ADR_SPACE_GPIO             (unsigned char) 8
#define ACPI_ADR_SPACE_GSBUS            (unsigned char) 9
#define ACPI_ADR_SPACE_PLATFORM_COMM    (unsigned char) 10
#define ACPI_ADR_SPACE_PLATFORM_RT      (unsigned char) 11
void init_acpi(void);
unsigned int acpi_find_table(char *Signature);
unsigned int *acpi_find_rsdp(void);
char checksum(unsigned char *addr, unsigned int length);
int acpi_shutdown(void);												/* ͨ��ACPIʵ�ֹػ� */
int acpi_reboot(void);													/* ͨ��ACPIʵ������ */
int acpi_reset(void);													/* ͨ��ACPI��I/O����ʵ������ */

/* cpuid.h(CPUID��غ�) */
/* ��Ӧ������ */
// #define CPUID_VENDOR_OLDAMD      		"AMDisbetter!"					/* ����AMD�������Ĺ�����Ʒ */
// #define CPUID_VENDOR_AMD         		"AuthenticAMD"
#define CPUID_VENDOR_INTEL       		"GenuineIntel"
// #define CPUID_VENDOR_OLDTRANSMETA		"TransmetaCPU"
// #define CPUID_VENDOR_TRANSMETA   		"GenuineTMx86"
// #define CPUID_VENDOR_CYRIX       		"CyrixInstead"
// #define CPUID_VENDOR_CENTAUR     		"CentaurHauls"
// #define CPUID_VENDOR_NEXGEN      		"NexGenDriven"
// #define CPUID_VENDOR_UMC         		"UMC UMC UMC "
// #define CPUID_VENDOR_SIS         		"SiS SiS SiS "
// #define CPUID_VENDOR_NSC         		"Geode by NSC"
// #define CPUID_VENDOR_RISE        		"RiseRiseRise"
// #define CPUID_VENDOR_VORTEX      		"Vortex86 SoC"
// #define CPUID_VENDOR_VIA         		"VIA VIA VIA "
/* ������Ĺ�Ӧ������ */
#define CPUID_VENDOR_VMWARE      		"VMwareVMware"
#define CPUID_VENDOR_XENHVM      		"XenVMMXenVMM"
#define CPUID_VENDOR_MICROSOFT_HV		"Microsoft Hv"
#define CPUID_VENDOR_PARALLELS   		" lrpepyh vr"
void cpu_init(void);													/* ��ʼ��CPU�����Ϣ */
int cpu_64_check(void);													/* (δ���)���CPU�Ƿ�֧��64λ */

/* io.c(�����������) */
#define IO_READ		1	/* ���� */
#define IO_WRITE	2	/* д�� */
#define IO_EXIT		3	/* ������� */
void io_init(void);															// ��ʼ��IO
void io_runreq(char mode, int cyl, int head, int *req, int *mem);
int io_runreqsub(char mode, int cyl, int head, int sect, int sects);
void io_req(char mode, int s);
int io_stat(char mode, int s);

/* fdc.c(FDC���̿�����) */
struct FDC {
	char mode, mot;
	char cyl, head, sect;
	char sects;
	char st0;
};
#define DMAC0_CBAR		0x0004
#define DMAC0_CBCR		0x0005
#define DMAC0_SMR		0x000a
#define DMAC0_MODE		0x000b
#define DMAP_ADDRUP		0x0081
#define DMAC1_CBAR		0x00c0
#define DMAC1_MODE		0x00d6
#define FDC_MOTOR		0x03f2
#define FDC_STAT		0x03f4
#define FDC_DATA		0x03f5
void fdc_tinit(struct TASK *task);					// ��ʼ��FDC����
void fdc_dmainit(void);
void fdc_req(char mode, char cyl, char head, char sect, char send);
int fdc_reqstat(void);
void fdc_init(void);								// ��ʼ��FDC
void fdc_initwait(int wait);						// ��ʼ�������еĵȴ�����
void fdc_sendcmd(int data);							// ��FDC����ָ��
void fdc_setdma(void);								// ����DMA
void fdc_sethead(void);								// ����head
void fdc_setcmd(void);								// ��������ģʽ
char fdc_getrstatsub(void);							// ��ȡ�Ĵ���״̬�ӳ���
void fdc_getint(void);								// ��ȡ�ж�״̬
void fdc_getrstat(void);							// ��ȡ�Ĵ���״̬
int fdc_rdwri(void);								// ���ƶ�д
void fdc_task(void);								// FDC�����������
void inthandler26(int *esp);						// ���̿������жϳ���

/********************************************************************
*              ������ͼ����ʾ����(��X11,TinyGL,etc.��ֲ)            *
********************************************************************/
 
/* x11(sample X Windows lib) */
/* X.h */
typedef unsigned long XID;
typedef XID Window;
typedef XID Drawable;
typedef XID Font;
typedef XID Pixmap;
typedef XID Cursor;
typedef XID Colormap;
typedef XID GContext;
typedef XID KeySym;
typedef unsigned long Mask;
typedef unsigned long Atom;
typedef unsigned long VisualID;
typedef unsigned long Time;
typedef unsigned char KeyCode;

/* Xlib.h */
typedef struct XExtData {
	/* ��չ��Ҫһ�ַ�������˽������(private_data)����һЩ�ṹ(list)�� */
	int number;							/* XRegisterExtension���ص�number */
	struct XExtData *next;				/* �б��ṹ����һ�� */
	int (*free_private)();				/* ���øú������ͷ�private�ռ� */
	char *private_data;					/* ����չ��˽������ */
} XExtData;
typedef struct {
	/* �Ӿ��ṹ�������йؿ��ܵ���ɫӳ�����Ϣ��  */
	XExtData *ext_data;										/* ������չ�������ݵĹ��� */
	VisualID visualid;										/* ���Ӿ�������Ӿ� ID */
	int class;												/* ��Ļ��𣨵�ɫ�ȣ�  */
	unsigned long red_mask, green_mask, blue_mask;			/* ����ֵ */
	int bits_per_rgb;										/* ��ͬ��ɫֵ�Ķ������� 2  */
	int map_entries;										/* ��ɫӳ����Ŀ */
} Visual;

/* Xutil.h */
typedef struct {
  Visual *visual;
  VisualID visualid;
  int screen;
  int depth;
  int class;
  unsigned long red_mask;
  unsigned long green_mask;
  unsigned long blue_mask;
  int colormap_size;
  int bits_per_rgb;
} XVisualInfo;

/* ���´������ڲ���(����)����ϵͳ����ʹ��ʱ(�������)�����õ� */
