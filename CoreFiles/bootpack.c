//naskfunc中的函数
void io_hlt(void);							//hlt功能
//void write_mem8(int addr, int data);		//写入内存(被指针取代)
void io_cli(void);							//禁止中断
void io_out8(int port, int data);			//传输数据用的
int io_load_eflags(void);					//读取最初的eflags值
void io_store_eflags(int eflags);			//将值存入eflags寄存器

//此文件中的函数
void KaliMain(void);																				//主函数
void init_palette(void);																			//初始化调色板函数
void set_palette(int start, int end, unsigned char *rgb);											//设置调色板
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);		//绘制方块
void init_screen(char *vram, int x, int y);															//初始化屏幕
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);								//绘制字体
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);					//绘制字符串

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

struct BOOTINFO {
	/*启动信息 - 此处原内容在第89页*/
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

void KaliMain(void){
	/*这里是主程序*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;		//启动信息(BOOTINFO结构体)
	
	init_palette();												//初始化调色板
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);		//初始化屏幕
	/*
	* 注：这里的 binfo->vram 等价于(*binfo).vram
	* 其他的同理
	*/
	
	putfonts8_asc(binfo->vram, binfo->scrnx, 236, 181, COL_DRED, "KalinoteOS");
	putfonts8_asc(binfo->vram, binfo->scrnx, 235, 180, COL_BRED, "KalinoteOS");
	/*
	* 只要先绘制一遍暗色字体，然后再绘制一遍亮色字体，然后两个字符串差1个像素，就可以整出立体感来
	* 后面调整函数文件结构后可以加个函数来专门绘制立体字符串
	*/
	
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

void HariMain(void){
	/*系统启动入口*/
	KaliMain();
}