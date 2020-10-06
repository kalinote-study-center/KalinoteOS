/*除画面和中断外的其他处理*/
#include <stdio.h>

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

//结构体
struct BOOTINFO {
	/*启动信息 - 此处原内容在第89页*/
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

struct SEGMENT_DESCRIPTOR {
	/*存放GDT的8字节内容 - 此处原内容在第105页*/
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
	/*存放IDT的8字节内容 - 此处原内容在第105页*/
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

//naskfunc中的函数(汇编编写)
void io_hlt(void);							//hlt功能
//void write_mem8(int addr, int data);		//写入内存(被指针取代)
void io_cli(void);							//禁止中断
void io_out8(int port, int data);			//传输数据用的
int io_load_eflags(void);					//读取最初的eflags值
void io_store_eflags(int eflags);			//将值存入eflags寄存器
void load_gdtr(int limit, int addr);		//加载GDTR寄存器
void load_idtr(int limit, int addr);		//加载IDTR寄存器

//此文件中的函数
void KaliMain(void);																				//主函数
void init_palette(void);																			//初始化调色板函数
void set_palette(int start, int end, unsigned char *rgb);											//设置调色板
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);		//绘制方块
void init_screen(char *vram, int x, int y);															//初始化屏幕
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);								//绘制字体
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);					//绘制字符串
void init_mouse_cursor8(char *mouse, char bc);														//初始化鼠标指针
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);											//鼠标背景色处理
void init_gdtidt(void);																				//初始化GDT和IDT
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

void KaliMain(void){
	/*这里是主程序*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;		//启动信息(BOOTINFO结构体)
	char s[40], mcursor[256];
	int mx, my;
	
	init_gdtidt();
	init_palette();												//初始化调色板
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);		//初始化屏幕
	/*
	* 注：这里的 binfo->vram 等价于(*binfo).vram
	* 其他的同理
	*/
	
	mx = (binfo->scrnx - 16) / 2; 								//鼠标指针位置，默认为屏幕中心
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL_LDBLUE);					//初始化鼠标指针
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16,
		mx, my, mcursor, 16);									//绘制鼠标
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, s);
	/*
	* 注：此时的鼠标背景只能是COL_LDBLUE的纯色，所以移动是会覆盖掉下面的内容
	* 不过后面会解决这个问题
	*/
	
	putfonts8_asc(binfo->vram, binfo->scrnx, 236, 181, COL_DRED, "KalinoteOS");
	putfonts8_asc(binfo->vram, binfo->scrnx, 235, 180, COL_BRED, "KalinoteOS");
	/*
	* 只要先绘制一遍暗色字体，然后再绘制一遍亮色字体，然后两个字符串差1个像素，就可以整出立体感来
	* 后面调整函数文件结构后可以加个函数来专门绘制立体字符串
	*/
	
	//变量相关内容，原文在第98页
	sprintf(s, "scrnx = %d", binfo->scrnx);
	putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL_WHITE, s);
	
	for(;;){
		//停止CPU
		io_hlt();
	}
}

void HariMain(void){
	/*系统启动入口*/
	KaliMain();
}
