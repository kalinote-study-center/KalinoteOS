/*除画面和中断外的其他处理*/
#include <stdio.h>
#include "bootpack.h"

void make_window(unsigned char *buf, int xsize, int ysize, char *title);			//生成一个窗口

void KaliMain(void){
	/* 系统入口 */
	
	/*这里是主程序*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//启动信息(BOOTINFO结构体)
	char s[40], keybuf[32], mousebuf[128];
	int mx, my, i;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse, *sht_win;
	unsigned char *buf_back, buf_mouse[256], *buf_win;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC初始化结束，解除CPU的中断禁止 */
	fifo8_init(&keyfifo, 32, keybuf);							//初始化键盘fifo缓冲区
	fifo8_init(&mousefifo, 128, mousebuf);						//初始化鼠标fifo缓冲区
	init_pit();													//初始化定时器
	io_out8(PIC0_IMR, 0xf8); /* 允许PIT、PIC1和键盘(11111000) */
	io_out8(PIC1_IMR, 0xef); /* 允许鼠标(11101111) */

	init_keyboard();				//初始化键盘
	enable_mouse(&mdec);			//激活鼠标
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();												//初始化调色板
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sht_back  = sheet_alloc(shtctl);
	sht_mouse = sheet_alloc(shtctl);							//鼠标
	sht_win = sheet_alloc(shtctl);						//counter窗口
	
	/* 分配内存 */
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	buf_win = (unsigned char *) memman_alloc_4k(memman, 160 * 52);
	
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	// 没有透明色
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);						// 鼠标 透明色号99
	sheet_setbuf(sht_win, buf_win, 160, 52, -1);						// counter窗口
	init_screen(buf_back, binfo->scrnx, binfo->scrny);					// 初始化屏幕
	/*
	* 注：这里的 binfo->vram 等价于(*binfo).vram
	* 其他的同理
	*/
	init_mouse_cursor8(buf_mouse, 99);							//背景色号99.

	/* counter窗口 */
	make_window(buf_win, 160, 52, "counter");
	sheet_slide(sht_back, 0, 0);
	mx = (binfo->scrnx - 16) / 2; 								//鼠标指针位置，默认为屏幕中心
	my = (binfo->scrny - 28 - 16) / 2;
	sheet_slide(sht_mouse, mx, my);
	sheet_slide(sht_win, 75, 70);		//调整counter窗口位置
	
	sheet_updown(sht_back,  0);
	sheet_updown(sht_win,   1);			/* 背景的序号是0(最底层)，其次是窗口1(中间)，最后是鼠标(最顶层) */
	sheet_updown(sht_mouse, 2);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL_WHITE, s);
	
	//变量相关内容，原文在第98页
	//sprintf(s, "scrnx = %d", binfo->scrnx);
	//putfonts8_asc(buf_back, binfo->scrnx, 16, 64, COL_WHITE, s);

	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 32, COL_WHITE, s);			//显示内存
	sheet_refresh(sht_back, 0, 0, binfo->scrnx, 48);
	
	for(;;){
		//计数器
		sprintf(s, "%010d", timerctl.count);
		boxfill8(buf_win, 160, COL_BGREY, 40, 28, 119, 43);
		putfonts8_asc(buf_win, 160, 40, 28, COL_BLACK, s);
		sheet_refresh(sht_win, 40, 28, 120, 44);
		
		//停止CPU
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
			io_sti();
		} else {
			if (fifo8_status(&keyfifo) != 0) {
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				boxfill8(buf_back, binfo->scrnx, COL_LDBLUE,  0, 16, 15, 31);
				putfonts8_asc(buf_back, binfo->scrnx, 0, 16, COL_WHITE, s);
				sheet_refresh(sht_back, 0, 16, 16, 32);
			} else if (fifo8_status(&mousefifo) != 0) {
				i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0) {
					/* 3字符集齐，显示出来 */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {
						//鼠标左键
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {
						//鼠标右键
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {
						//鼠标中键
						s[2] = 'C';
					}
					boxfill8(buf_back, binfo->scrnx, COL_LDBLUE, 32, 16, 32 + 15 * 8 - 1, 31);
					putfonts8_asc(buf_back, binfo->scrnx, 32, 16, COL_WHITE, s);
					sheet_refresh(sht_back, 32, 16, 32 + 15 * 8, 32);
					/* 鼠标指针移动 */
					mx += mdec.x;
					my += mdec.y;
					//限制鼠标指针边界
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 1) {
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1) {
						my = binfo->scrny - 1;
					}
					sprintf(s, "(%3d, %3d)", mx, my);
					boxfill8(buf_back, binfo->scrnx, COL_LDBLUE, 0, 0, 79, 15); /* 隐藏坐标 */
					putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL_WHITE, s); /* 显示坐标 */
					sheet_refresh(sht_back, 0, 0, 80, 16);
					sheet_slide(sht_mouse, mx, my); /* 包含sheet_refresh */
				}
			}
		}
	}
}

void make_window(unsigned char *buf, int xsize, int ysize, char *title){
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL_BGREY, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL_WHITE, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL_DGREY, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL_BGREY, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL_DBLUE, 3,         3,         xsize - 4, 20       );
	boxfill8(buf, xsize, COL_DGREY, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL_BLACK, 0,         ysize - 1, xsize - 1, ysize - 1);
	putfonts8_asc(buf, xsize, 24, 4, COL_WHITE, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL_BLACK;
			} else if (c == '$') {
				c = COL_DGREY;
			} else if (c == 'Q') {
				c = COL_BGREY;
			} else {
				c = COL_WHITE;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}
