//asmhead.nas
struct BOOTINFO {	/* 0x0ff0-0x0fff */
	/*启动信息 - 此处原内容在第89页*/
	char cyls;	/*启动区读取硬盘位置*/
	char leds;	/*启动时键盘的LED状态*/
	char vmode;	/*显卡模式*/
    char reserve;
	short scrnx, scrny;	/*画面分辨率*/
	char *vram;
};
#define ADR_BOOTINFO	0x00000ff0

//naskfunc.nas中的函数(汇编编写)
void io_hlt(void);							//暂停处理器
//void write_mem8(int addr, int data);		//写入内存(被指针取代)
void io_cli(void);							//禁止中断
void io_sti(void);							//允许中断
void io_stihlt(void);						//允许中断并暂停处理器
int io_in8(int port);						//传输数据用的
void io_out8(int port, int data);			//传输数据用的
int io_load_eflags(void);					//读取最初的eflags值
void io_store_eflags(int eflags);			//将值存入eflags寄存器
void load_gdtr(int limit, int addr);		//加载GDTR寄存器
void load_idtr(int limit, int addr);		//加载IDTR寄存器
int load_cr0(void);							//加载CR0寄存器
void store_cr0(int cr0);					//存入CR0寄存器
void asm_inthandler21(void);				//21中断，注册在0x21
void asm_inthandler27(void);				//27中断，注册在0x27
void asm_inthandler2c(void);				//2c中断，注册在0x2c
unsigned int memtest_sub(
	unsigned int start,
	unsigned int end);						//读取内存

//graphic.c
void init_palette(void);																			//初始化调色板函数
void set_palette(int start, int end, unsigned char *rgb);											//设置调色板
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);		//绘制方块
void init_screen(char *vram, int x, int y);															//初始化屏幕
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);								//绘制字体
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);					//绘制字符串
void init_mouse_cursor8(char *mouse, char bc);														//初始化鼠标指针
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);											//鼠标背景色处理

// 15种颜色常数定义
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
void init_gdtidt(void);																				//初始化GDT和IDT
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
void inthandler27(int *esp);			//27中断
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
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);									//初始化FIFO缓冲区
int fifo8_put(struct FIFO8 *fifo, unsigned char data);												//向FIFO传数据并保存
int fifo8_get(struct FIFO8 *fifo);																	//从FIFO获得一个数据
int fifo8_status(struct FIFO8 *fifo);																//查询缓冲区状态

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
extern struct FIFO8 mousefifo;
void inthandler2c(int *esp);																		//鼠标监听中断
void enable_mouse(struct MOUSE_DEC *mdec);															//激活鼠标
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

/* keyboard.c */
void inthandler21(int *esp);																		//键盘监听中断
void wait_KBC_sendready(void);																		//等待键盘控制电路准备完毕
void init_keyboard(void);																			//初始化键盘控制电路
extern struct FIFO8 keyfifo;
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

/* memory.c */
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
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);		//初始化层
struct SHEET *sheet_alloc(struct SHTCTL *ctl);														//获得未使用的新图层
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);		//设置缓冲区大小
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);								//调整sheet高度
void sheet_refresh(struct SHTCTL *ctl);																//刷新图层
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0);							//移动图层
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht);												//释放已使用的图层内存
