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

void init_palette(void){
	/*调色板函数，预置15中基本颜色，可以自行添加 - 此处原内容在第75页*/
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:亮红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	set_palette(0, 15, table_rgb);
	return;
	// C语言中的static char只能用于数据，相当于汇编中的DB
}

void set_palette(int start, int end, unsigned char *rgb){
	/*调色板功能 - 此处原内容在第75页*/
	int i, eflags;
	eflags = io_load_eflags();	/* 记录中断许可标志的值 */
	io_cli(); 					/* 将中断许可标志置为0，禁止中断 */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 复原中断许可标志 */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	/*绘制方块 - 此处原内容在第84页*/
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_screen(char *vram, int x, int y){
	/*初始化屏幕*/
	boxfill8( vram, x, COL_LDBLUE, 0, 0, x - 1, y - 1);		//绘制一个纯色背景当桌面
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	/*绘制字体 - 此处原内容在第93页*/
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	/*绘制字符串(ASCLL编码) - 此处原内容在第96页*/
	extern char fonts[4096];
	for (; *s != 0x00; s++) {
		putfont8(vram, xsize, x, y, c, fonts + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor8(char *mouse, char bc){
	/* 准备鼠标指针(16x16) - 此处原内容在第99页*/
	static char cursor[16][16] = {
		"*...............",
		"**..............",
		"*O*.............",
		"*OO*............",
		"*OOO*...........",
		"*OOOO*..........",
		"*OOOOO*.........",
		"*OOOOOO*........",
		"*OOOOOOO*.......",
		"*OOOO*****......",
		"*OO*O*..........",
		"*O*.*O*.........",
		"**..*O*.........",
		"*....*O*........",
		".....*O*........",
		"......*........."
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL_BLACK;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL_WHITE;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize){
		/*鼠标背景色处理*/
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}


void init_gdtidt(void)
{
	/*初始化GDT和IDT - 此处原内容在第页105*/
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;		//GDT的地址位于0x270000~0x27ffff
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;		//IDT的地址位于0x26f800~0x26ffff
	int i;

	/* GDT初始化 */
	for (i = 0; i < 8192; i++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
	load_gdtr(0xffff, 0x00270000);

	/* IDT初始化 */
	for (i = 0; i < 256; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(0x7ff, 0x0026f800);

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}


void HariMain(void){
	/*系统启动入口*/
	KaliMain();
}
