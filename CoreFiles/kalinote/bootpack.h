//asmhead.nas(bootpack.c的前面部分)
struct BOOTINFO {	/* 0x0ff0-0x0fff */
	/*启动信息 - 此处原内容在第89页*/
	char cyls;	/*启动区读取硬盘位置*/
	char leds;	/*启动时键盘的LED状态*/
	char vmode;	/*显卡模式*/
    char reserve;
	short scrnx, scrny;	/*画面分辨率*/
	int *vram;
};
#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG		0x00100000			//文件位置(软盘，内存中)

//naskfunc.nas中的函数(汇编编写)
void io_hlt(void);							//暂停处理器
//void write_mem8(int addr, int data);		//写入内存(被指针取代)
void io_cli(void);							//禁止中断
void io_sti(void);							//允许中断
void io_stihlt(void);						//允许中断并暂停处理器
int io_in8(int port);						//传输数据用的，汇编IN指令，用于端口操作
void io_out8(int port, int data);			//传输数据用的，汇编OUT指令，用于端口操作
int io_load_eflags(void);					//读取最初的eflags值
void io_store_eflags(int eflags);			//将值存入eflags寄存器
void load_gdtr(int limit, int addr);		//加载GDTR寄存器
void load_idtr(int limit, int addr);		//加载IDTR寄存器
int load_cr0(void);							//加载CR0寄存器
void store_cr0(int cr0);					//存入CR0寄存器
void load_tr(int tr);						//加载TR寄存器
void asm_inthandler0c(void);				//0c号中断，用于处理栈异常
void asm_inthandler0d(void);				//0d号中断，用于处理异常程序
void asm_inthandler20(void);				//20号中断，用于timer
void asm_inthandler21(void);				//21号中断，注册在0x21
void asm_inthandler27(void);				//27号中断，注册在0x27
void asm_inthandler2c(void);				//2c号中断，注册在0x2c
unsigned int memtest_sub(
	unsigned int start,
	unsigned int end);						//读取内存
void farjmp(int eip, int cs);				//指令跳转
void farcall(int eip, int cs);				//转移到调用的子程序(指定偏移)
void asm_kal_api(void);						//KalinoteOS 系统API
void start_app(int eip, int cs,
	int esp, int ds, int *tss_esp0);		//启动应用程序
void asm_end_app(void);						//结束应用程序
void asm_shutdown(void);					//关机功能

//graphic.c(画面显示)
void init_palette(void);																			//初始化调色板函数
void set_palette(int start, int end, unsigned char *rgb);											//设置调色板
void boxfill8(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1);		//绘制方块
void init_screen8(int *vram, int x, int y, int bc);													//初始化屏幕
void putfont8(int *vram, int xsize, int x, int y, int c, char *font);								//绘制字体
void putfonts8_asc(int *vram, int xsize, int x, int y, int c, unsigned char *s);					//绘制字符串
void init_mouse_cursor8(int *mouse, int bc);														//初始化鼠标指针
void putblock8_8(int *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, int *buf, int bxsize);											//鼠标背景色处理
// 15种颜色常数定义，此系统支持RGB全彩色，所以可以使用0xRGB(普通的RGB表示方法)来表示颜色
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
void init_gdtidt(void);																				// 初始化GDT和IDT
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);				// 设置段描述符
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);					// 设置门描述符
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

//int.c(中断控制)
void init_pic(void);							// 初始化中断
void inthandler27(int *esp);					// 27号中断，具体作用可以去代码里看
#define PIC0_ICW1		0x0020					// 主PIC的初始化控制数据，1号，固定值
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021					// 主PIC的中断屏蔽寄存器地址
#define PIC0_ICW2		0x0021					// 主PIC的初始化控制数据，2号，这个值可以用来设定PIC中断号，不过为了避免混乱，暂时不改
#define PIC0_ICW3		0x0021					// 主PIC的初始化控制数据，3号，固定值
#define PIC0_ICW4		0x0021					// 主PIC的初始化控制数据，4号，固定值
#define PIC1_ICW1		0x00a0					// 从PIC的初始化控制数据，1号，固定值
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1					// 从PIC的中断屏蔽寄存器地址
#define PIC1_ICW2		0x00a1					// 从PIC的初始化控制数据，2号，这个值可以用来设定PIC中断号，不过为了避免混乱，暂时不改
#define PIC1_ICW3		0x00a1					// 从PIC的初始化控制数据，3号，固定值
#define PIC1_ICW4		0x00a1					// 从PIC的初始化控制数据，4号，固定值

//fifo.c(FIFO缓冲区)
struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);						//初始化FIFO缓冲区
int fifo32_put(struct FIFO32 *fifo, int data);														//向FIFO传数据并保存
int fifo32_get(struct FIFO32 *fifo);																//从FIFO获得一个数据
int fifo32_status(struct FIFO32 *fifo);																//查询缓冲区状态

//mouse.c(鼠标控制)
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
void inthandler2c(int *esp);																		//鼠标监听中断
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);							//激活鼠标
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);										//获取鼠标的三个字节信息，并解码

//keyboard.c(键盘控制)
void inthandler21(int *esp);																		//键盘监听中断
void wait_KBC_sendready(void);																		//等待键盘控制电路准备完毕
void init_keyboard(struct FIFO32 *fifo, int data0);													//初始化键盘控制电路
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

//memory.c(内存管理)
#define MEMMAN_FREES		4090	/* 大约32KB */
#define MEMMAN_ADDR			0x003c0000
struct FREEINFO {	/* 可用信息 */
	unsigned int addr, size;
};
struct MEMMAN {		/* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);											//测试CPU类型
void memman_init(struct MEMMAN *man);																//初始化内存管理程序
unsigned int memman_total(struct MEMMAN *man);														//获取剩余内存大小
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);									//分配内存
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);							//释放内存
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);								//4K大空间分配内存
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);						//4K大空间释放内存

//sheet.c(画面图层处理)
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
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned int *vram, int xsize, int ysize);		//初始化层
struct SHEET *sheet_alloc(struct SHTCTL *ctl);														//获得未使用的新图层
void sheet_setbuf(struct SHEET *sht, unsigned int *buf, int xsize, int ysize, int col_inv);			//设置缓冲区大小
void sheet_updown(struct SHEET *sht, int height);													//调整sheet高度
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);							//刷新图层(区域)
void sheet_slide(struct SHEET *sht, int vx0, int vy0);												//移动图层
void sheet_free(struct SHEET *sht);																	//释放已使用的图层内存

//timer.c(定时器)
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
void init_pit(void);																				//初始化可编程间隔化定时器(PIT)
void timer_free(struct TIMER *timer);																//释放定时器
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);								//初始化定时器
void timer_settime(struct TIMER *timer, unsigned int timeout);										//设置定时器
void inthandler20(int *esp);																		//20号中断
int timer_cancel(struct TIMER *timer);																//取消定时器
void timer_cancelall(struct FIFO32 *fifo);															//取消所有定时器

/* mtask.c(多任务) */
#define MAX_TASKS		2500																		//最大任务数量
#define TASK_GDT0		3																			//定义从GDT的几号开始分配给TSS
#define MAX_TASKS_LV	100																			//最高任务等级
#define MAX_TASKLEVELS	10																			//最高任务层(321页)
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};
struct TASK {
	int sel, flags; /* sel用来存放GDT编号 */
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
	int running; /* 正在运行的任务数量 */
	int now; /* 当前运行的任务 */
	struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; /* 现在活动中的任务等级 */
	char lv_change; /* 下次任务切换时是否需要改变level */
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};
extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
struct TASK *task_now(void);
void task_run(struct TASK *task, int level, int priority);											//运行程序
void task_switch(void);																				//切换程序
void task_sleep(struct TASK *task);																	//程序睡眠

/* window.c(窗口绘制) */
void make_window8(unsigned int *buf, int xsize, int ysize, char *title, char act);					// 生成一个窗口
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);				// 先涂背景色，在写字符串
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);						// 生成编辑框
void make_wtitle8(unsigned int *buf, int xsize, char *title, char act);								// 生成一个标题栏
void change_wtitle8(struct SHEET *sht, char act);													// 改变窗口标题栏颜色
void make_icon(unsigned int *buf, int xsize, char icon_pic[16][16], char type);						// 显示一个logo

/* console.c(命令台) */
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
void console_task(struct SHEET *sheet, unsigned int memtotal);										// 命令窗口任务
void cons_newline(struct CONSOLE *cons);															// 命令窗口换行
void cons_putchar(struct CONSOLE *cons, int chr, char move);										// 在命令窗口上显示文字
void cons_putstr0(struct CONSOLE *cons, char *s);													// 显示字符串(通过字符编码0结尾)
void cons_putstr1(struct CONSOLE *cons, char *s, int l);											// 显示字符串(通过指定长度)
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);				// 执行命令
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);											// CMD：查询内存使用状态
void cmd_cls(struct CONSOLE *cons);																	// CMD：清屏
void cmd_dir(struct CONSOLE *cons);																	// CMD：查询目录文件
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);										// CMD：显示文件内容
void cmd_exit(struct CONSOLE *cons, int *fat);														// CMD：关闭命令窗口
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD：在新的命令行中启动一个程序
void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD：在当前命令窗口执行命令，且不占用命令窗口
void cmd_langmode(struct CONSOLE *cons, char *cmdline);												// CMD：切换语言模式
void cmd_shutdown(void);																			// CMD：关机
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);											// 外部应用程序
int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);				// 通过edx查找API
int *inthandler0d(int *esp);																		// 0d号中断，用于处理异常程序
int *inthandler0c(int *esp);																		// 0c号中断，用于处理栈异常
void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);					// 绘制一条直线

/* file.c(文件处理) */
struct FILEINFO {
	//文件结构(详见第367页)
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
void file_readfat(int *fat, unsigned char *img);													// 解码FAT
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);							// 加载文件
char *file_loadfile2(int clustno, int *psize, int *fat);											// 加载kca压缩文件

/* jpeg.c(读取jpg图片) */
struct DLL_STRPICENV{
	int work[64 * 1024 / 4];
};
struct RGB{
	unsigned char b,g,r,t;
};
int info_JPEG(struct DLL_STRPICENV*env,int *info,int size,unsigned char *fp);												// 获取JPG图片信息
int decode0_JPEG(struct DLL_STRPICENV*env,int size,unsigned char *fp,int b_type,unsigned char *buf,int skip);				// JPG图片解码

/* bootpack.c */
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal);							// 开启一个命令窗口
struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal);								// 开启一个任务

/* kca.c(解压KCA压缩文件) */
int kca_getsize(unsigned char *p);
int kca_decomp(unsigned char *p, char *q, int size);

/* cmos.c(cmos端口操作) */
#define cmos_index 0x70
#define cmos_data 0x71
/* cmos中相关信息偏移 */
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
unsigned int get_hour_hex();						// 取当前时间(小时)
unsigned int get_min_hex();							// 取当前时间(分钟)
unsigned int get_sec_hex();							// 取当前时间(秒)
unsigned int get_day_of_month();					// 取当前日期
unsigned int get_day_of_week();						// 取当前星期
unsigned int get_mon_hex();							// 取当前月份
unsigned int get_year();							// 取当前年份
