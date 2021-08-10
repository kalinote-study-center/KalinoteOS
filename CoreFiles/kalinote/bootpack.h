/* 
 * KalinoteOS主头文件
 * 目前所有文件的函数都在此头文件中进行声明
 * 本头文件中的所有函数、变量、结构体等，都已经按照文件进行分类
 * 如果有需要，可以方便地拆分成不同头文件
 */

/* 系统全局定义 */
//sysinfo(bootpack.c系统全局信息)
struct SYSINFO {
	char sysmode;					/* 系统模式 */
	char sysmmainver;				/* 系统主版本号 */
	int sysver;						/* 系统版本号 */
	unsigned char ide_hd_num;		/* 计算机IDE硬盘数量 */
	unsigned int free_mem;			/* 系统剩余内存容量 */
	unsigned int memtotal;			/* 系统总内存 */
	int year, month, day;			/* 系统日期 */
	int hour, min, sec;				/* 系统时间 */
	unsigned int time_counter;		/* 记录系统启动以来的时间 */
};
#define	SYS_MEMORY		0x00400000			// 系统占用内存
#define FIFO_ADDR		0x0fec				// FIFO地址
#define SYSINFO_ADDR	0x10000				// 系统信息地址
#define SHTCTL_ADDR		0x0fe4				// 系统图层管理结构体地址
#undef	NULL			
#define NULL ((void*)0)						// 重新定义NULL
#ifndef FALSE
#define FALSE  (0)							// 定义FALSE
#define TRUE   (1)							// 定义TRUE
#endif

/* asmhead.nas(bootpack.c的前面部分) 以及 naskfunc.nas */
struct BOOTINFO {		/* 0x0ff0-0x0fff */
	/*启动信息 - 此处原内容在第89页*/
	char cyls;			/*启动区读取硬盘位置*/
	char leds;			/*启动时键盘的LED状态*/
	char vmode;			/*显卡模式*/
    char reserve;
	short scrnx, scrny;	/*画面分辨率*/
	int *vram;
};
#define ADR_BOOTINFO	0x00000ff0						// bootinfo位置
#define ADR_DISKIMG		0x00100000						// 文件位置(软盘，内存中)
void io_hlt(void);										// 暂停处理器
//void write_mem8(int addr, int data);					// 写入内存(被指针取代)
void io_cli(void);										// 禁止中断
void io_sti(void);										// 允许中断
void io_stihlt(void);									// 允许中断并暂停处理器
int io_in8(int port);									// 传输数据用的，汇编IN指令，用于端口操作(8位)
void io_out8(int port, int data);						// 传输数据用的，汇编OUT指令，用于端口操作(8位)
int io_in16(int port);									// 传输数据用的，汇编IN指令，用于端口操作(16位)
void io_out16(int port, int data);						// 传输数据用的，汇编OUT指令，用于端口操作(16位)
int io_in32(int port);									// 传输数据用的，汇编IN指令，用于端口操作(32位)
void io_out32(int port, int data);						// 传输数据用的，汇编OUT指令，用于端口操作(32位)
int io_load_eflags(void);								// 读取最初的eflags值
void io_store_eflags(int eflags);						// 将值存入eflags寄存器
void port_read(unsigned short port, void* buf, int n);	// 端口读数据
void port_write(unsigned short port, void* buf, int n);	// 端口写数据
void load_gdtr(int limit, int addr);					// 加载GDTR寄存器(GDTR寄存器用于保存GDT在内存中的位置)
void load_idtr(int limit, int addr);					// 加载IDTR寄存器(IDTR寄存器用于保存IDT在内存中的位置)
int load_cr0(void);										// 加载CR0寄存器
void store_cr0(int cr0);								// 存入CR0寄存器
void load_tr(int tr);									// 加载TR寄存器
void asm_inthandler00(void);							// 00号中断，除零异常
void asm_inthandler07(void);							// 07号中断，FPU异常中断
void asm_inthandler0c(void);							// 0c号中断，用于处理栈异常
void asm_inthandler0d(void);							// 0d号中断，用于处理异常程序
void asm_inthandler20(void);							// 20号中断，用于timer
void asm_inthandler21(void);							// 21号中断，注册在0x21
void asm_inthandler26(void);							// 26号中断，用于处理软盘
void asm_inthandler27(void);							// 27号中断，注册在0x27
void asm_inthandler2c(void);							// 2c号中断，注册在0x2c
void asm_inthandler2e(void);							// IDE硬盘中断
unsigned int memtest_sub(			
	unsigned int start,			
	unsigned int end);									//读取内存
void farjmp(int eip, int cs);							//指令跳转(用于任务切换)
void farcall(int eip, int cs);							//转移到调用的子程序(指定偏移)
void asm_kal_api(void);									//KalinoteOS 系统API
void start_app(int eip, int cs,			
	int esp, int ds, int *tss_esp0);					//启动应用程序
void asm_end_app(void);									//结束应用程序
void clts(void);
void fnsave(int *addr);
void frstor(int *addr);

//dsctbl.c(GDT,IDT)
struct SEGMENT_DESCRIPTOR {
	/* GDT，全局段号记录表数据结构，每条记录8字节 */
	/* 每个段表储存3个信息，分别是段大小(limit)，段基址(base)和段管理属性(access_right) */
	/* 在该结构体中为了系统对不同CPU的兼容性，base分为了low(2字节)、mid(1字节)、high(1字节)，加起来是32位 */
	/* 上限(limit)的长度是20位，在段属性中有一个标志位(Gbit)，用于表示limit的单位，当Gbit为1时，段以页(4KB)为单位，所以limit最高可表示4GB(4KB x 1M) */
	/* 段属性(access_right)为12位，具体数据见下方常量 */
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	/* IDT，中断记录表数据结构，每条记录8字节 */
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void);																				// 初始化GDT和IDT
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);				// 设置段描述符
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);					// 设置门描述符
#define ADR_IDT			0x0026f800						/* 全局段号记录表地址 */
#define LIMIT_IDT		0x000007ff						/* IDT大小2KB */
#define ADR_GDT			0x00270000						/* GDT地址 */
#define LIMIT_GDT		0x0000ffff						/* GDT大小64KB */
#define ADR_BOTPAK		0x00280000						/* bootpack.kal地址 */
#define LIMIT_BOTPAK	0x0007ffff						/* bootpack.kal大小512KB */
#define AR_DATA32_RW	0x4092							/* 系统专用，可读写不可执行 */
#define AR_CODE32_ER	0x409a							/* 系统专用，可读可执行不可写 */
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
	int *buf;									// 缓冲区地址
	int p, q, size, free, flags;				// p是下一次数据写入地址，q是下一次数据读入地址，size是缓冲区大小，free是缓冲区空闲字节数，flags是溢出标志(为1时发生溢出)
	struct TASK *task;							// 有数据写入时需要唤醒的任务
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);						// 初始化FIFO缓冲区
int fifo32_put(struct FIFO32 *fifo, int data);														// 向FIFO传数据并保存
int fifo32_get(struct FIFO32 *fifo);																// 从FIFO获得一个数据
int fifo32_status(struct FIFO32 *fifo);																// 查询缓冲区状态

//mouse.c(鼠标控制)
struct MOUSE_DEC {
	unsigned char buf[3], phase;				// buf[3]是数据缓冲区，P/S2鼠标一次传递3字节数据，phase是接受第几字节的数据
	int x, y, btn;								// x,y，btn是解析鼠标数据后获得的坐标和按钮数据
};
void inthandler2c(int *esp);																		// 鼠标监听中断
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);							// 激活鼠标
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);										// 获取鼠标的三个字节信息，并解码

//keyboard.c(键盘控制)
void inthandler21(int *esp);																		// 键盘监听中断
void wait_KBC_sendready(void);																		// 等待键盘控制电路准备完毕
void init_keyboard(struct FIFO32 *fifo, int data0);													// 初始化键盘控制电路
void reset_cpu(void);																				// 复位CPU
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

//memory.c(内存管理)
#define MEMMAN_FREES		4090	/* 大约32KB */
#define MEMMAN_ADDR			0x003c0000		/* 内存管理表地址 */
struct FREEINFO {	/* 可用信息 */
	unsigned int addr, size;
};
struct MEMMAN {		/* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);											// 测试CPU类型，以及内存容量
void memman_init(struct MEMMAN *man);																// 初始化内存管理程序
unsigned int memman_total(struct MEMMAN *man);														// 获取剩余内存大小
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);									// 分配内存
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);							// 释放内存
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);								// 4K大空间分配内存
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);						// 4K大空间释放内存

//sheet.c(画面图层处理)
#define MAX_SHEETS			256									// 最大图层数(包括鼠标、桌面、任务栏等)
#define SHEET_NO_USE		0									// 使用状态标志(未使用)
#define SHEET_USE			1									// 使用状态标志(正在使用)
#define SHEET_APIWIN		2									// 使用状态标志(外部API窗口)
#define SHEET_CONS			3									// 使用状态标志(命令行窗口)
#define SHEET_NO_TITLE		4									// 使用状态标志(其他无标题栏窗口)
#define SHEET_DEBUG_CONS	5									// 使用状态标志(DEBUG命令行窗口)
#define	SHEET_BACK			101									// 使用状态标志(背景层)
#define	SHEET_TASKBAR		102									// 使用状态标志(任务栏层)
#define	SHEET_MOUSE			103									// 使用状态标志(鼠标指针层)
#define	SHEET_MENU			104									// 使用状态标志(菜单栏层)
#define	SHEET_BUTTON		105									// 使用状态标志(按钮层)
struct SHEET {
	/* 图层结构 */
	unsigned int *buf;										// 图像内容
	int bxsize, bysize, vx0, vy0;							// bxsize和bysize是图像整体大小，vx0和vy0是图层在画面上的坐标位置(v是vram的省略)
	int col_inv, height, flags;								// col_inv是透明色色号，height表示图层高度，flags表示图层设定信息
	struct SHTCTL *ctl;										// 父图层管理器，SHTCTL结构体
	struct SHTCTL *subctl;									// 子图层管理器，SHTCTL结构体，属于SHEET的内部图层
	struct TASK *task;										// 将图层和任务进行绑定，在强制结束任务时可以寻找残留图层并关闭
	unsigned int *win;										// 窗口(或列表等)结构体的地址指针
};
struct SHTCTL {
	/* 多重图层的信息管理结构 */
	unsigned int *vram, *map;								// vram是VRAM地址
	int xsize, ysize, top;									// xsize和ysize图层大小，top是最顶层图层(鼠标指针层)高度
	struct SHEET *fsheet;									// 父图层
	struct SHEET *sheets[MAX_SHEETS];						// 所有图层的地址(按照高度升序排列)
	struct SHEET sheets0[MAX_SHEETS];						// 存放所有图层的图层信息
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned int *vram, int xsize, int ysize);		//初始化层
struct SHEET *sheet_alloc(struct SHTCTL *ctl);														//获得未使用的新图层
void sheet_setbuf(struct SHEET *sht, unsigned int *buf, int xsize, int ysize, int col_inv);			//设置缓冲区大小
void sheet_updown(struct SHEET *sht, int height);													//调整sheet高度
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);							//刷新图层(区域)
void sheet_slide(struct SHEET *sht, int vx0, int vy0);												//移动图层
void sheet_free(struct SHEET *sht);																	//释放已使用的图层内存

//graphic.c(画面显示)
//void init_palette(void);																			//初始化调色板函数
//void set_palette(int start, int end, unsigned char *rgb);											//设置调色板
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

//timer.c(定时器)
#define MAX_TIMER		500
struct TIMER {
	struct TIMER *next;									// 下一个超时的时钟
	unsigned int timeout;								// 超时时间
	char flags, flags2;
	struct FIFO32 *fifo;
	int data;
};
struct TIMERCTL {
	unsigned int count, next, using;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
#define TIMER_FLAGS_ALLOC		1																	// 已配置状态
#define TIMER_FLAGS_USING		2																	// 定时器运行中
extern struct TIMERCTL timerctl;
struct TIMER *timer_alloc(void);
void init_pit(void);																				// 初始化可编程间隔化定时器(PIT)
void timer_free(struct TIMER *timer);																// 释放定时器
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);								// 初始化定时器
void timer_settime(struct TIMER *timer, unsigned int timeout);										// 设置定时器
void inthandler20(int *esp);																		// 20号中断
int timer_cancel(struct TIMER *timer);																// 取消定时器
void timer_cancelall(struct FIFO32 *fifo);															// 取消所有定时器

/* mtask.c(多任务) */
#define MAX_TASKS		2500																		// 最大任务数量
#define TASK_GDT0		3																			// 定义从GDT的几号开始分配给TSS
#define MAX_TASKS_LV	250																			// 每层最多任务数
#define MAX_TASKLEVELS	10																			// 最高任务层数(321页)
/*下面这些是任务状态码，暂时还没有用到(2021.6.3) */
#define	TESK_UNUSED					0					/* 未使用 */
#define TASK_UNINTERRUPTIBLE		1					/* 可中断睡眠状态 */
#define TASK_RUNNING				2					/* 正在运行 */
#define	TASK_INTERRUPTIBLE			3					/* 不可中断睡眠状态 */
#define	TASK_ZOMBIE					4					/* 僵死状态 */
#define	TASK_STOPPED				5					/* 已停止 */
/*************************************************/
struct TSS32 {
	/* 任务状态段(104字节)，用于切换任务时保存寄存器数据，这个是32位版，如果以后系统会升级到64位则更新64位版 */
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;						// 32位寄存器
	int es, cs, ss, ds, fs, gs;														// 16位寄存器
	int ldtr, iomap;
};
struct TASK {
	int sel, flags; 										// sel用来存放GDT编号，flags是任务状态，后面可以定义不同的任务状态
	int level, priority;									// priority是进程优先级
	struct FIFO32 fifo;										// 任务FIFO缓冲区，如果有需要以后也可以加个list(双链表)
	struct TSS32 tss;										// 任务状态段
	int fpu[108 / 4];										// TASK使用FPU寄存器时的存储位置和读取源
	struct SEGMENT_DESCRIPTOR ldt[2];						
	struct CONSOLE *cons;									// 任务对应的console
	int ds_base, cons_stack;
	struct FILEHANDLE *fhandle;
	int *fat;
	char *cmdline;
	unsigned char langmode, langbyte1;
	// char *task_name;										// 任务名称
};
struct TASKLEVEL {
	int running; /* 正在运行的任务数量 */
	int now; /* 当前运行的任务 */
	struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; 									/* 现在活动中的任务等级 */
	struct TASK *task_fpu;							/* 记录当前FPU寄存器属于哪个任务 */
	char lv_change; 								/* 下次任务切换时是否需要改变level */
	struct TASKLEVEL level[MAX_TASKLEVELS];			/* 任务level结构体数组 */
	struct TASK tasks0[MAX_TASKS];					/* 任务结构体数组 */
};
extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;
void task_add(struct TASK *task);																	// 向struct TASKLEVEL中添加一个进程
void task_remove(struct TASK *task);																// 向struct TASKLEVEL中删除一个进程
struct TASK *task_init(struct MEMMAN *memman);														// 初始化进程
struct TASK *task_alloc(void);																		// 分配进程
struct TASK *task_now(void);																		// 返回现在正在活动中的TASK结构体地址
void task_run(struct TASK *task, int level, int priority);											// 运行程序
void task_switch(void);																				// 切换程序
void task_sleep(struct TASK *task);																	// 程序睡眠
int *inthandler07(int *esp);																		// FPU异常中断

/* window.c(窗口绘制) */
#define TIT_ACT_DEFAULT		0x00ffc1c1				// 默认窗口标题栏选中颜色	
#define TIT_DEACT_DEFAULT	0x00cd9b9b				// 默认窗口标题栏未选中颜色
struct WINCOLORS {
	/* 窗口(主题)颜色 */
	int act_color,dis_act_color,back_color;			// 窗口颜色，act_color是key_on态颜色，dis_act_color是key_off态颜色
};
struct WINDOW {
	/* 这个结构体储存窗口信息 */
	unsigned int *buf;								// 窗口图形缓冲区
	char *wtitle;									// 窗口标题
	int xsize,ysize;								// 窗口大小
	struct WINCOLORS wcolor;						// 窗口颜色
	int whandle;									// 窗口句柄(可以在这里存放SHEET结构体)
	int tskwinbtn;									// 任务栏按钮index
};
// struct WINDOW *make_window8(unsigned int *buf, int xsize, int ysize, char *title, char act);		// 生成一个窗口(旧)
struct WINDOW *make_window8(struct SHEET *sht, int xsize, int ysize,
	int act_color, int deact_color, char *title, char act);											// 生成一个窗口
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);				// 先涂背景色，在写字符串
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);						// 生成编辑框
// void make_wtitle8(unsigned int *buf, int xsize, char *title, char act);							// 生成一个标题栏(旧版)
void make_wtitle8(struct WINDOW *window, char act);													// 生成一个标题栏
void change_wtitle8(struct WINDOW *window, char act);												// 改变窗口标题栏颜色
// void make_icon(unsigned int *buf, int xsize, char type);											// 显示一个logo(旧版)
void make_icon(struct WINDOW *window, char type);													// 显示一个logo

/* button.c(按钮) */
struct BUTTON {
	/* 按钮结构体 */
	char *title;									// 按钮标题
	int buttonx, buttony;							// 按钮位置
	int height,width;								// 按钮大小(高宽)
	int flags;										// 按钮状态
	int back_color;									// 按钮背景色
	struct SHEET *sht;								// 按钮图像sheet
	char click_old;									// 上一次是否按下(1为按下，0为未按)
	char show;										// 是否正可视
	void(*onButtonClick)();							// 点击时执行
};
struct BUTTON *make_button(struct MEMMAN *memman, int width, int height,
	int buttonx, int buttony, char *title, int back_color, void(*onButtonClick)());					// 创建一个按钮
void show_button(struct SHEET *sht, struct MEMMAN *memman, struct BUTTON *button);					// 绘制按钮
void change_button(struct BUTTON *button, struct SHEET *fsht, char click);							// 更改按钮凸起和按下(或禁用)
void click_button(struct BUTTON *button);															// 点击按钮
void hide_button(struct BUTTON *button);															// 隐藏按钮
void release_button(struct BUTTON *button);															// 释放按钮

/* menu.c(菜单栏) */
#define	MAX_OPTIONS			255						// 最大选项数量
struct OPTIONS {
	/* 菜单栏选项结构体 */
	int flags;										// 选项标志
	char *title;									// 选项标题
	unsigned char index;							// 选项index
	void(*onOptionClick)();							// 点击时执行
};
struct MENU {
	/* 菜单栏结构体 */
	int menux,menuy;								// 菜单栏坐标
	int mheight,mwidth;								// 菜单栏大小
	unsigned int *buf;								// 菜单图像缓冲地址
	struct SHEET *sht;								// 菜单图像sheet
	int flags;										// flags是菜单当前状态
	unsigned char now, old;							// now是当前选中项,old是上一个选中项
	int option_num;									// 当前选项数量
	struct OPTIONS options[MAX_OPTIONS];			// 存放选项列表(最多256个选项，应该不会有哪个程序用到200多个选项)
};
struct MENU *make_menu(struct MEMMAN *memman, int menux, int menuy);								// 创建菜单栏
void release_menu(struct MEMMAN *man, struct MENU *menu);											// 释放菜单栏
// void add_options(struct MENU *menus, char *option_title);										// 增加选项(旧版)
void add_options(struct MENU *menu, char *option_title, void(*onOptionClick)());					// 增加选项
void remove_options(struct MENU *menu, unsigned char index);										// 移除选项
void show_menu(struct SHTCTL *shtctl, struct MEMMAN *memman, struct MENU *menu);					// 显示菜单
void hide_menu(struct MEMMAN *memman, struct MENU *menu);											// 隐藏菜单
void option_change(struct MENU *menu, int mouse_y);													// 鼠标移动时选项变色处理
void menu_click(struct MENU *menu, int mouse_y);													// 菜单栏被单击

/* textbox.c(输入框) */
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);						// 生成输入框

/* taskbar.c(底端任务栏) */
#define TASKBAR_ADDR	0x30100			/* 任务栏图层地址 */
#define TASKBARCTL_ADDR	0x30200			/* 任务栏管理结构体地址 */
#define MAX_TSKWINBTN	256				/* 最多同时显示256个窗口在taskbar上 */
struct TSKWINBTN {
	/* 每个任务栏按钮选项 */
	int index;					/* 窗口在任务栏的按钮的index */
	int flags;					/* 当前结构体的使用状态(未使用、最小化、打开分别为0，1，2) */
	int row;					/* 窗口在任务连第几排(从第0排开始，每排最多7个窗口) */
	struct WINDOW *win;			/* 对应的窗口 */
	struct BUTTON *button;		/* 对应的任务栏按钮 */
};
struct TASKBARCTL {
	/* 任务栏管理结构体 */
	int num;										/* 窗口数量 */
	int now_row,total_row;							/* 当前显示排数和总排数-1 */
	struct MENU *menu;								/* 开始菜单 */
	struct BUTTON *button;							/* Function按钮 */
	struct TSKWINBTN tskwinbtns[MAX_TSKWINBTN];		/* 任务栏按钮选项结构体 */
};
void init_taskbar(struct MEMMAN *memman, struct SHEET *sht);					// 初始化任务栏
int taskbar_addwin(struct WINDOW *window);										// 向任务栏增加一个窗口按钮
void taskbar_removewin(int index);												// 从任务栏删除一个按钮

/* console.c(命令台) */
#define DEBUG_ADDR		0x30000				// DEBUG console位置
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
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat,
	unsigned int memtotal, char sysmode);															// 执行命令
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);											// CMD：查询内存使用状态
void cmd_cls(struct CONSOLE *cons);																	// CMD：清屏
void cmd_dir(struct CONSOLE *cons);																	// CMD：查询目录文件
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);										// CMD：显示文件内容
void cmd_exit(struct CONSOLE *cons, int *fat);														// CMD：关闭命令窗口
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD：在新的命令行中启动一个程序
void cmd_run(struct CONSOLE *cons, char *cmdline, int memtotal);									// CMD：在当前命令窗口执行命令，且不占用命令窗口
void cmd_langmode(struct CONSOLE *cons, char *cmdline);												// CMD：切换语言模式
void cmd_shutdown(void);																			// CMD：关机
void cmd_sysmode(struct CONSOLE *cons, char *cmdline);												// CMD：切换系统模式
void cmd_echo(struct CONSOLE *cons, char *cmdline);													// CMD：系统输出
void cmd_hdinfo(struct CONSOLE *cons, char *cmdline);												// CMD：查询IDE硬盘信息
void cmd_getruntime(struct CONSOLE *cons);															// CMD：查询系统启动运行时间
void cmd_pciinfo(struct CONSOLE *cons);																// CMD：查询PCI设备信息
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);											// 外部应用程序
int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);				// 通过edx查找API
void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);					// 绘制一条直线
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal, int debug);				// 开启一个命令窗口
struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal);								// 开启一个任务
void keywin_off(struct SHEET *key_win);																// 控制窗口标题栏颜色和光标激活状态
void keywin_on(struct SHEET *key_win);																// 控制窗口标题栏颜色和光标激活状态
void close_console(struct SHEET *sht);																// 关闭命令窗口
void close_constask(struct TASK *task);																// 结束任务
void cons_printf(struct CONSOLE *cons, char *format, ...);											// 格式化输出到指定cons窗口
void debug_print(char *format, ...);																// 输出到DEBUG窗口

/* exception.c(异常中断处理) */
int *inthandler00(int *esp);																		// 00号中断，用于处理除零异常
int *inthandler0c(int *esp);																		// 0c号中断，用于处理栈异常
int *inthandler0d(int *esp);																		// 0d号中断，用于处理一般异常

/* file.c(文件处理) */
struct FILEINFO {
	//文件结构(详见第367页)
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;																// 磁盘映像中的地址 = clustno * 512 +0x003e00
	unsigned int size;
};
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
void file_readfat(int *fat, unsigned char *img);													// 解码FAT
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);							// 加载文件
char *file_loadfile2(int clustno, int *psize, int *fat);											// 加载kca压缩文件

/* jpeg.c(处理jpg图片) */
struct DLL_STRPICENV{
	int work[64 * 1024 / 4];
};
struct RGB{
	unsigned char b,g,r,t;
};
int info_JPEG(struct DLL_STRPICENV*env,int *info,int size,unsigned char *fp);												// 获取JPG图片信息
int decode0_JPEG(struct DLL_STRPICENV*env,int size,unsigned char *fp,int b_type,unsigned char *buf,int skip);				// JPG图片解码

/* bootpack.c */
#define KEYCMD_LED		0xed

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

/* list.c(双链表结构) */
struct list_elem {
	/* list节点，用于保存前一个节点和后一个节点的地址 */
	struct list_elem* prev; // 前躯结点
	struct list_elem* next; // 后继结点
};
struct list {
	/* head是队首,是固定不变的，不是第1个元素,第1个元素为head.next */
	struct list_elem head;
	/* tail是队尾,同样是固定不变的 */
	struct list_elem tail;
};
void list_init(struct list* list);												// 初始化链表
void list_insert_before(struct list_elem* before, struct list_elem* elem);		// 将链表元素elem插入到before之前
void list_push(struct list* plist, struct list_elem* elem);						// 添加元素到列表队首,类似栈push操作
void list_append(struct list* plist, struct list_elem* elem);					// 追加元素到链表队尾,类似队列的先进先出操作
void list_remove(struct list_elem* pelem);										// 使元素pelem脱离链表
struct list_elem* list_pop(struct list* plist);									// 将链表第一个元素弹出并返回,类似栈的pop操作
int elem_find(struct list* plist, struct list_elem* obj_elem);					// 从链表中查找元素obj_elem,成功时返回所在位置,失败时返回-1
int list_empty(struct list* plist);												// 判断链表是否为空,空时返回1,否则返回0

/* util.c(通用工具包) */
int read_rtc(unsigned char tt[5]);

/* acpi.c(电源管理) */
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

