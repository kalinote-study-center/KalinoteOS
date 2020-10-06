/*除画面和中断外的其他处理*/
#include <stdio.h>
#include "bootpack.h"

extern struct FIFO8 keyfifo;

void KaliMain(void){
	/*这里是主程序*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//启动信息(BOOTINFO结构体)
	char s[40], mcursor[256], keybuf[32];
	int mx, my, i;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC初始化结束，解除CPU的中断禁止 */
	
	fifo8_init(&keyfifo, 32, keybuf);							//初始化fifo缓冲区
	io_out8(PIC0_IMR, 0xf9); /* 允许PIC1和键盘(11111001) */
	io_out8(PIC1_IMR, 0xef); /* 允许鼠标(11101111) */
	
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
	//sprintf(s, "scrnx = %d", binfo->scrnx);
	//putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL_WHITE, s);
	
	for(;;){
		//停止CPU
		io_cli();
		if (fifo8_status(&keyfifo) == 0) {
			io_stihlt();
		} else {
			i = fifo8_get(&keyfifo);
			io_sti();
			sprintf(s, "%02X", i);
			boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 0, 16, 15, 31);
			putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL_WHITE, s);
		}
	}
}

void HariMain(void){
	/*系统启动入口*/
	KaliMain();
}
