//asmhead.nas(bootpack.c��ǰ�沿��)
struct BOOTINFO {	/* 0x0ff0-0x0fff */
	/*������Ϣ - �˴�ԭ�����ڵ�89ҳ*/
	char cyls;	/*��������ȡӲ��λ��*/
	char leds;	/*����ʱ���̵�LED״̬*/
	char vmode;	/*�Կ�ģʽ*/
    char reserve;
	short scrnx, scrny;	/*����ֱ���*/
	int *vram;
};
#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG		0x00100000			//�ļ�λ��(���̣��ڴ���)

//naskfunc.nas�еĺ���(����д)
void io_hlt(void);							//��ͣ������
//void write_mem8(int addr, int data);		//д���ڴ�(��ָ��ȡ��)
void io_cli(void);							//��ֹ�ж�
void io_sti(void);							//�����ж�
void io_stihlt(void);						//�����жϲ���ͣ������
int io_in8(int port);						//���������õģ����INָ����ڶ˿ڲ���
void io_out8(int port, int data);			//���������õģ����OUTָ����ڶ˿ڲ���
int io_load_eflags(void);					//��ȡ�����eflagsֵ
void io_store_eflags(int eflags);			//��ֵ����eflags�Ĵ���
void load_gdtr(int limit, int addr);		//����GDTR�Ĵ���
void load_idtr(int limit, int addr);		//����IDTR�Ĵ���
int load_cr0(void);							//����CR0�Ĵ���
void store_cr0(int cr0);					//����CR0�Ĵ���
void load_tr(int tr);						//����TR�Ĵ���
void asm_inthandler0c(void);				//0c���жϣ����ڴ���ջ�쳣
void asm_inthandler0d(void);				//0d���жϣ����ڴ����쳣����
void asm_inthandler20(void);				//20���жϣ�����timer
void asm_inthandler21(void);				//21���жϣ�ע����0x21
void asm_inthandler27(void);				//27���жϣ�ע����0x27
void asm_inthandler2c(void);				//2c���жϣ�ע����0x2c
unsigned int memtest_sub(
	unsigned int start,
	unsigned int end);						//��ȡ�ڴ�
void farjmp(int eip, int cs);				//ָ����ת
void farcall(int eip, int cs);				//ת�Ƶ����õ��ӳ���(ָ��ƫ��)
void asm_kal_api(void);						//KalinoteOS ϵͳAPI
void start_app(int eip, int cs,
	int esp, int ds, int *tss_esp0);		//����Ӧ�ó���
void asm_end_app(void);						//����Ӧ�ó���
void asm_shutdown(void);					//�ػ�����

//graphic.c(������ʾ)
void init_palette(void);																			//��ʼ����ɫ�庯��
void set_palette(int start, int end, unsigned char *rgb);											//���õ�ɫ��
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

//dsctbl.c(GDT,IDT)
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
void init_gdtidt(void);																				// ��ʼ��GDT��IDT
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);				// ���ö�������
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);					// ������������
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
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
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);						//��ʼ��FIFO������
int fifo32_put(struct FIFO32 *fifo, int data);														//��FIFO�����ݲ�����
int fifo32_get(struct FIFO32 *fifo);																//��FIFO���һ������
int fifo32_status(struct FIFO32 *fifo);																//��ѯ������״̬

//mouse.c(������)
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
void inthandler2c(int *esp);																		//�������ж�
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);							//�������
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);										//��ȡ���������ֽ���Ϣ��������

//keyboard.c(���̿���)
void inthandler21(int *esp);																		//���̼����ж�
void wait_KBC_sendready(void);																		//�ȴ����̿��Ƶ�·׼�����
void init_keyboard(struct FIFO32 *fifo, int data0);													//��ʼ�����̿��Ƶ�·
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

//memory.c(�ڴ����)
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

//sheet.c(����ͼ�㴦��)
#define MAX_SHEETS		256
struct SHEET {
	unsigned int *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
	struct TASK *task;
};
struct SHTCTL {
	unsigned int *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned int *vram, int xsize, int ysize);		//��ʼ����
struct SHEET *sheet_alloc(struct SHTCTL *ctl);														//���δʹ�õ���ͼ��
void sheet_setbuf(struct SHEET *sht, unsigned int *buf, int xsize, int ysize, int col_inv);			//���û�������С
void sheet_updown(struct SHEET *sht, int height);													//����sheet�߶�
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);							//ˢ��ͼ��(����)
void sheet_slide(struct SHEET *sht, int vx0, int vy0);												//�ƶ�ͼ��
void sheet_free(struct SHEET *sht);																	//�ͷ���ʹ�õ�ͼ���ڴ�

//timer.c(��ʱ��)
#define MAX_TIMER		500
struct TIMER {
	struct TIMER *next;
	unsigned int timeout;
	char flags, flags2;
	struct FIFO32 *fifo;
	int data;
};
struct TIMERCTL {
	unsigned int count, next, using;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
extern struct TIMERCTL timerctl;
struct TIMER *timer_alloc(void);
void init_pit(void);																				//��ʼ���ɱ�̼������ʱ��(PIT)
void timer_free(struct TIMER *timer);																//�ͷŶ�ʱ��
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);								//��ʼ����ʱ��
void timer_settime(struct TIMER *timer, unsigned int timeout);										//���ö�ʱ��
void inthandler20(int *esp);																		//20���ж�
int timer_cancel(struct TIMER *timer);																//ȡ����ʱ��
void timer_cancelall(struct FIFO32 *fifo);															//ȡ�����ж�ʱ��

/* mtask.c(������) */
#define MAX_TASKS		2500																		//�����������
#define TASK_GDT0		3																			//�����GDT�ļ��ſ�ʼ�����TSS
#define MAX_TASKS_LV	100																			//�������ȼ�
#define MAX_TASKLEVELS	10																			//��������(321ҳ)
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};
struct TASK {
	int sel, flags; /* sel�������GDT��� */
	int level, priority;
	struct FIFO32 fifo;
	struct TSS32 tss;
	struct SEGMENT_DESCRIPTOR ldt[2];
	struct CONSOLE *cons;
	int ds_base, cons_stack;
	struct FILEHANDLE *fhandle;
	int *fat;
	char *cmdline;
	unsigned char langmode, langbyte1;
};
struct TASKLEVEL {
	int running; /* �������е��������� */
	int now; /* ��ǰ���е����� */
	struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; /* ���ڻ�е�����ȼ� */
	char lv_change; /* �´������л�ʱ�Ƿ���Ҫ�ı�level */
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};
extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
struct TASK *task_now(void);
void task_run(struct TASK *task, int level, int priority);											//���г���
void task_switch(void);																				//�л�����
void task_sleep(struct TASK *task);																	//����˯��

/* window.c(���ڻ���) */
void make_window8(unsigned int *buf, int xsize, int ysize, char *title, char act);					// ����һ������
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);				// ��Ϳ����ɫ����д�ַ���
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);						// ���ɱ༭��
void make_wtitle8(unsigned int *buf, int xsize, char *title, char act);								// ����һ��������
void change_wtitle8(struct SHEET *sht, char act);													// �ı䴰�ڱ�������ɫ
void make_icon(unsigned int *buf, int xsize, char icon_pic[16][16], char type);						// ��ʾһ��logo

/* console.c(����̨) */
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
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);				// ִ������
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);											// CMD����ѯ�ڴ�ʹ��״̬
void cmd_cls(struct CONSOLE *cons);																	// CMD������
void cmd_dir(struct CONSOLE *cons);																	// CMD����ѯĿ¼�ļ�
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);										// CMD����ʾ�ļ�����
void cmd_exit(struct CONSOLE *cons, int *fat);														// CMD���ر������
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD�����µ�������������һ������
void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD���ڵ�ǰ�����ִ������Ҳ�ռ�������
void cmd_langmode(struct CONSOLE *cons, char *cmdline);												// CMD���л�����ģʽ
void cmd_shutdown(void);																			// CMD���ػ�
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);											// �ⲿӦ�ó���
int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);				// ͨ��edx����API
int *inthandler0d(int *esp);																		// 0d���жϣ����ڴ����쳣����
int *inthandler0c(int *esp);																		// 0c���жϣ����ڴ���ջ�쳣
void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);					// ����һ��ֱ��

/* file.c(�ļ�����) */
struct FILEINFO {
	//�ļ��ṹ(�����367ҳ)
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
void file_readfat(int *fat, unsigned char *img);													// ����FAT
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);							// �����ļ�
char *file_loadfile2(int clustno, int *psize, int *fat);											// ����kcaѹ���ļ�

/* jpeg.c(��ȡjpgͼƬ) */
struct DLL_STRPICENV{
	int work[64 * 1024 / 4];
};
struct RGB{
	unsigned char b,g,r,t;
};
int info_JPEG(struct DLL_STRPICENV*env,int *info,int size,unsigned char *fp);												// ��ȡJPGͼƬ��Ϣ
int decode0_JPEG(struct DLL_STRPICENV*env,int size,unsigned char *fp,int b_type,unsigned char *buf,int skip);				// JPGͼƬ����

/* bootpack.c */
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal);							// ����һ�������
struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal);								// ����һ������

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
