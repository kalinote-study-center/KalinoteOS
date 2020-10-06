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
void io_hlt(void);							//hlt功能
//void write_mem8(int addr, int data);		//写入内存(被指针取代)
void io_cli(void);							//禁止中断
void io_out8(int port, int data);			//传输数据用的
int io_load_eflags(void);					//读取最初的eflags值
void io_store_eflags(int eflags);			//将值存入eflags寄存器
void load_gdtr(int limit, int addr);		//加载GDTR寄存器
void load_idtr(int limit, int addr);		//加载IDTR寄存器

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