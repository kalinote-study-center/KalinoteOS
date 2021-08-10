/* 
 * KalinoteOS��ͷ�ļ�
 * Ŀǰ�����ļ��ĺ������ڴ�ͷ�ļ��н�������
 * ��ͷ�ļ��е����к������������ṹ��ȣ����Ѿ������ļ����з���
 * �������Ҫ�����Է���ز�ֳɲ�ͬͷ�ļ�
 */

/* ϵͳȫ�ֶ��� */
//sysinfo(bootpack.cϵͳȫ����Ϣ)
struct SYSINFO {
	char sysmode;					/* ϵͳģʽ */
	char sysmmainver;				/* ϵͳ���汾�� */
	int sysver;						/* ϵͳ�汾�� */
	unsigned char ide_hd_num;		/* �����IDEӲ������ */
	unsigned int free_mem;			/* ϵͳʣ���ڴ����� */
	unsigned int memtotal;			/* ϵͳ���ڴ� */
	int year, month, day;			/* ϵͳ���� */
	int hour, min, sec;				/* ϵͳʱ�� */
	unsigned int time_counter;		/* ��¼ϵͳ����������ʱ�� */
};
#define	SYS_MEMORY		0x00400000			// ϵͳռ���ڴ�
#define FIFO_ADDR		0x0fec				// FIFO��ַ
#define SYSINFO_ADDR	0x10000				// ϵͳ��Ϣ��ַ
#define SHTCTL_ADDR		0x0fe4				// ϵͳͼ�����ṹ���ַ
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
void port_read(unsigned short port, void* buf, int n);	// �˿ڶ�����
void port_write(unsigned short port, void* buf, int n);	// �˿�д����
void load_gdtr(int limit, int addr);					// ����GDTR�Ĵ���(GDTR�Ĵ������ڱ���GDT���ڴ��е�λ��)
void load_idtr(int limit, int addr);					// ����IDTR�Ĵ���(IDTR�Ĵ������ڱ���IDT���ڴ��е�λ��)
int load_cr0(void);										// ����CR0�Ĵ���
void store_cr0(int cr0);								// ����CR0�Ĵ���
void load_tr(int tr);									// ����TR�Ĵ���
void asm_inthandler00(void);							// 00���жϣ������쳣
void asm_inthandler07(void);							// 07���жϣ�FPU�쳣�ж�
void asm_inthandler0c(void);							// 0c���жϣ����ڴ���ջ�쳣
void asm_inthandler0d(void);							// 0d���жϣ����ڴ����쳣����
void asm_inthandler20(void);							// 20���жϣ�����timer
void asm_inthandler21(void);							// 21���жϣ�ע����0x21
void asm_inthandler26(void);							// 26���жϣ����ڴ�������
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
void asm_end_app(void);									//����Ӧ�ó���
void clts(void);
void fnsave(int *addr);
void frstor(int *addr);

//dsctbl.c(GDT,IDT)
struct SEGMENT_DESCRIPTOR {
	/* GDT��ȫ�ֶκż�¼�����ݽṹ��ÿ����¼8�ֽ� */
	/* ÿ���α���3����Ϣ���ֱ��Ƕδ�С(limit)���λ�ַ(base)�Ͷι�������(access_right) */
	/* �ڸýṹ����Ϊ��ϵͳ�Բ�ͬCPU�ļ����ԣ�base��Ϊ��low(2�ֽ�)��mid(1�ֽ�)��high(1�ֽ�)����������32λ */
	/* ����(limit)�ĳ�����20λ���ڶ���������һ����־λ(Gbit)�����ڱ�ʾlimit�ĵ�λ����GbitΪ1ʱ������ҳ(4KB)Ϊ��λ������limit��߿ɱ�ʾ4GB(4KB x 1M) */
	/* ������(access_right)Ϊ12λ���������ݼ��·����� */
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
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
#define ADR_IDT			0x0026f800						/* ȫ�ֶκż�¼���ַ */
#define LIMIT_IDT		0x000007ff						/* IDT��С2KB */
#define ADR_GDT			0x00270000						/* GDT��ַ */
#define LIMIT_GDT		0x0000ffff						/* GDT��С64KB */
#define ADR_BOTPAK		0x00280000						/* bootpack.kal��ַ */
#define LIMIT_BOTPAK	0x0007ffff						/* bootpack.kal��С512KB */
#define AR_DATA32_RW	0x4092							/* ϵͳר�ã��ɶ�д����ִ�� */
#define AR_CODE32_ER	0x409a							/* ϵͳר�ã��ɶ���ִ�в���д */
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
void reset_cpu(void);																				// ��λCPU
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

//memory.c(�ڴ����)
#define MEMMAN_FREES		4090	/* ��Լ32KB */
#define MEMMAN_ADDR			0x003c0000		/* �ڴ������ַ */
struct FREEINFO {	/* ������Ϣ */
	unsigned int addr, size;
};
struct MEMMAN {		/* �ڴ���� */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);											// ����CPU���ͣ��Լ��ڴ�����
void memman_init(struct MEMMAN *man);																// ��ʼ���ڴ�������
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
	unsigned int *win;										// ����(���б��)�ṹ��ĵ�ַָ��
};
struct SHTCTL {
	/* ����ͼ�����Ϣ����ṹ */
	unsigned int *vram, *map;								// vram��VRAM��ַ
	int xsize, ysize, top;									// xsize��ysizeͼ���С��top�����ͼ��(���ָ���)�߶�
	struct SHEET *fsheet;									// ��ͼ��
	struct SHEET *sheets[MAX_SHEETS];						// ����ͼ��ĵ�ַ(���ո߶���������)
	struct SHEET sheets0[MAX_SHEETS];						// �������ͼ���ͼ����Ϣ
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned int *vram, int xsize, int ysize);		//��ʼ����
struct SHEET *sheet_alloc(struct SHTCTL *ctl);														//���δʹ�õ���ͼ��
void sheet_setbuf(struct SHEET *sht, unsigned int *buf, int xsize, int ysize, int col_inv);			//���û�������С
void sheet_updown(struct SHEET *sht, int height);													//����sheet�߶�
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);							//ˢ��ͼ��(����)
void sheet_slide(struct SHEET *sht, int vx0, int vy0);												//�ƶ�ͼ��
void sheet_free(struct SHEET *sht);																	//�ͷ���ʹ�õ�ͼ���ڴ�

//graphic.c(������ʾ)
//void init_palette(void);																			//��ʼ����ɫ�庯��
//void set_palette(int start, int end, unsigned char *rgb);											//���õ�ɫ��
void boxfill8(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1);		//���Ʒ���
void init_screen8(int *vram, int x, int y, int bc);													//��ʼ����Ļ
void putfont8(int *vram, int xsize, int x, int y, int c, char *font);								//��������
void putfonts8_asc(int *vram, int xsize, int x, int y, int c, unsigned char *s);					//�����ַ���
void init_mouse_cursor8(int *mouse, int bc);														//��ʼ�����ָ��
void putblock8_8(int *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, int *buf, int bxsize);											//��걳��ɫ����
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
void inthandler20(int *esp);																		// 20���ж�
int timer_cancel(struct TIMER *timer);																// ȡ����ʱ��
void timer_cancelall(struct FIFO32 *fifo);															// ȡ�����ж�ʱ��

/* mtask.c(������) */
#define MAX_TASKS		2500																		// �����������
#define TASK_GDT0		3																			// �����GDT�ļ��ſ�ʼ�����TSS
#define MAX_TASKS_LV	250																			// ÿ�����������
#define MAX_TASKLEVELS	10																			// ����������(321ҳ)
/*������Щ������״̬�룬��ʱ��û���õ�(2021.6.3) */
#define	TESK_UNUSED					0					/* δʹ�� */
#define TASK_UNINTERRUPTIBLE		1					/* ���ж�˯��״̬ */
#define TASK_RUNNING				2					/* �������� */
#define	TASK_INTERRUPTIBLE			3					/* �����ж�˯��״̬ */
#define	TASK_ZOMBIE					4					/* ����״̬ */
#define	TASK_STOPPED				5					/* ��ֹͣ */
/*************************************************/
struct TSS32 {
	/* ����״̬��(104�ֽ�)�������л�����ʱ����Ĵ������ݣ������32λ�棬����Ժ�ϵͳ��������64λ�����64λ�� */
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;						// 32λ�Ĵ���
	int es, cs, ss, ds, fs, gs;														// 16λ�Ĵ���
	int ldtr, iomap;
};
struct TASK {
	int sel, flags; 										// sel�������GDT��ţ�flags������״̬��������Զ��岻ͬ������״̬
	int level, priority;									// priority�ǽ������ȼ�
	struct FIFO32 fifo;										// ����FIFO���������������Ҫ�Ժ�Ҳ���ԼӸ�list(˫����)
	struct TSS32 tss;										// ����״̬��
	int fpu[108 / 4];										// TASKʹ��FPU�Ĵ���ʱ�Ĵ洢λ�úͶ�ȡԴ
	struct SEGMENT_DESCRIPTOR ldt[2];						
	struct CONSOLE *cons;									// �����Ӧ��console
	int ds_base, cons_stack;
	struct FILEHANDLE *fhandle;
	int *fat;
	char *cmdline;
	unsigned char langmode, langbyte1;
	// char *task_name;										// ��������
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
void task_add(struct TASK *task);																	// ��struct TASKLEVEL�����һ������
void task_remove(struct TASK *task);																// ��struct TASKLEVEL��ɾ��һ������
struct TASK *task_init(struct MEMMAN *memman);														// ��ʼ������
struct TASK *task_alloc(void);																		// �������
struct TASK *task_now(void);																		// �����������ڻ�е�TASK�ṹ���ַ
void task_run(struct TASK *task, int level, int priority);											// ���г���
void task_switch(void);																				// �л�����
void task_sleep(struct TASK *task);																	// ����˯��
int *inthandler07(int *esp);																		// FPU�쳣�ж�

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
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);						// ���ɱ༭��
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
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);						// ���������

/* taskbar.c(�׶�������) */
#define TASKBAR_ADDR	0x30100			/* ������ͼ���ַ */
#define TASKBARCTL_ADDR	0x30200			/* ����������ṹ���ַ */
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
	/* ����������ṹ�� */
	int num;										/* �������� */
	int now_row,total_row;							/* ��ǰ��ʾ������������-1 */
	struct MENU *menu;								/* ��ʼ�˵� */
	struct BUTTON *button;							/* Function��ť */
	struct TSKWINBTN tskwinbtns[MAX_TSKWINBTN];		/* ��������ťѡ��ṹ�� */
};
void init_taskbar(struct MEMMAN *memman, struct SHEET *sht);					// ��ʼ��������
int taskbar_addwin(struct WINDOW *window);										// ������������һ�����ڰ�ť
void taskbar_removewin(int index);												// ��������ɾ��һ����ť

/* console.c(����̨) */
#define DEBUG_ADDR		0x30000				// DEBUG consoleλ��
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
void cons_putchar(struct CONSOLE *cons, int chr, char move);										// �����������ʾ����
void cons_putstr0(struct CONSOLE *cons, char *s);													// ��ʾ�ַ���(ͨ���ַ�����0��β)
void cons_putstr1(struct CONSOLE *cons, char *s, int l);											// ��ʾ�ַ���(ͨ��ָ������)
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat,
	unsigned int memtotal, char sysmode);															// ִ������
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);											// CMD����ѯ�ڴ�ʹ��״̬
void cmd_cls(struct CONSOLE *cons);																	// CMD������
void cmd_dir(struct CONSOLE *cons);																	// CMD����ѯĿ¼�ļ�
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);										// CMD����ʾ�ļ�����
void cmd_exit(struct CONSOLE *cons, int *fat);														// CMD���ر������
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD�����µ�������������һ������
void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD���ڵ�ǰ�����ִ������Ҳ�ռ�������
void cmd_langmode(struct CONSOLE *cons, char *cmdline);												// CMD���л�����ģʽ
void cmd_shutdown(void);																			// CMD���ػ�
void cmd_sysmode(struct CONSOLE *cons, char *cmdline);												// CMD���л�ϵͳģʽ
void cmd_echo(struct CONSOLE *cons, char *cmdline);													// CMD��ϵͳ���
void cmd_hdinfo(struct CONSOLE *cons, char *cmdline);												// CMD����ѯIDEӲ����Ϣ
void cmd_getruntime(struct CONSOLE *cons);															// CMD����ѯϵͳ��������ʱ��
void cmd_pciinfo(struct CONSOLE *cons);																// CMD����ѯPCI�豸��Ϣ
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);											// �ⲿӦ�ó���
int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);				// ͨ��edx����API
void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);					// ����һ��ֱ��
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal, int debug);				// ����һ�������
struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal);								// ����һ������
void keywin_off(struct SHEET *key_win);																// ���ƴ��ڱ�������ɫ�͹�꼤��״̬
void keywin_on(struct SHEET *key_win);																// ���ƴ��ڱ�������ɫ�͹�꼤��״̬
void close_console(struct SHEET *sht);																// �ر������
void close_constask(struct TASK *task);																// ��������
void cons_printf(struct CONSOLE *cons, char *format, ...);											// ��ʽ�������ָ��cons����
void debug_print(char *format, ...);																// �����DEBUG����

/* exception.c(�쳣�жϴ���) */
int *inthandler00(int *esp);																		// 00���жϣ����ڴ�������쳣
int *inthandler0c(int *esp);																		// 0c���жϣ����ڴ���ջ�쳣
int *inthandler0d(int *esp);																		// 0d���жϣ����ڴ���һ���쳣

/* file.c(�ļ�����) */
struct FILEINFO {
	//�ļ��ṹ(�����367ҳ)
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;																// ����ӳ���еĵ�ַ = clustno * 512 +0x003e00
	unsigned int size;
};
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
void file_readfat(int *fat, unsigned char *img);													// ����FAT
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);							// �����ļ�
char *file_loadfile2(int clustno, int *psize, int *fat);											// ����kcaѹ���ļ�

/* jpeg.c(����jpgͼƬ) */
struct DLL_STRPICENV{
	int work[64 * 1024 / 4];
};
struct RGB{
	unsigned char b,g,r,t;
};
int info_JPEG(struct DLL_STRPICENV*env,int *info,int size,unsigned char *fp);												// ��ȡJPGͼƬ��Ϣ
int decode0_JPEG(struct DLL_STRPICENV*env,int size,unsigned char *fp,int b_type,unsigned char *buf,int skip);				// JPGͼƬ����

/* bootpack.c */
#define KEYCMD_LED		0xed

/* kca.c(��ѹKCAѹ���ļ�) */
int kca_getsize(unsigned char *p);
int kca_decomp(unsigned char *p, char *q, int size);

/* cmos.c(cmos�˿ڲ���) */
#define cmos_index 0x70
#define cmos_data 0x71
/* cmos�������Ϣƫ�� */
#define CMOS_CUR_SEC	0x0
#define CMOS_ALA_SEC	0x1
#define CMOS_CUR_MIN	0x2
#define CMOS_ALA_MIN	0x3
#define CMOS_CUR_HOUR	0x4
#define CMOS_ALA_HOUR	0x5
#define CMOS_WEEK_DAY	0x6
#define CMOS_MON_DAY	0x7
#define CMOS_CUR_MON	0x8
#define CMOS_CUR_YEAR	0x9
#define CMOS_DEV_TYPE	0x12
#define CMOS_CUR_CEN	0x32
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

/* list.c(˫����ṹ) */
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
void list_insert_before(struct list_elem* before, struct list_elem* elem);		// ������Ԫ��elem���뵽before֮ǰ
void list_push(struct list* plist, struct list_elem* elem);						// ���Ԫ�ص��б����,����ջpush����
void list_append(struct list* plist, struct list_elem* elem);					// ׷��Ԫ�ص������β,���ƶ��е��Ƚ��ȳ�����
void list_remove(struct list_elem* pelem);										// ʹԪ��pelem��������
struct list_elem* list_pop(struct list* plist);									// �������һ��Ԫ�ص���������,����ջ��pop����
int elem_find(struct list* plist, struct list_elem* obj_elem);					// �������в���Ԫ��obj_elem,�ɹ�ʱ��������λ��,ʧ��ʱ����-1
int list_empty(struct list* plist);												// �ж������Ƿ�Ϊ��,��ʱ����1,���򷵻�0

/* util.c(ͨ�ù��߰�) */
int read_rtc(unsigned char tt[5]);

/* acpi.c(��Դ����) */
struct ACPI_RSDP {
	char Signature[8];
	unsigned char Checksum;
	char OEMID[6];
	unsigned char Revision;
	unsigned int RsdtAddress;
	unsigned int Length;
	unsigned int XsdtAddress[2];
	unsigned char ExtendedChecksum;
	unsigned char Reserved[3];
};

struct ACPISDTHeader {
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
void init_acpi(void);
unsigned int acpi_find_table(char *Signature);
unsigned int *acpi_find_rsdp(void);
char checksum(unsigned char *addr, unsigned int length);
int acpi_shutdown(void);

