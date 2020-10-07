/*除画面和中断外的其他处理*/
#include <stdio.h>
#include "bootpack.h"

#define MEMMAN_FREES		4090	/* 大约32KB */
#define MEMMAN_ADDR			0x003c0000

struct FREEINFO {	/* 可用信息 */
	unsigned int addr, size;
};

struct MEMMAN {		/* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);							//测试CPU类型
void memman_init(struct MEMMAN *man);												//初始化内存管理程序
unsigned int memman_total(struct MEMMAN *man);										//获取剩余内存大小
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);					//分配内存
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);			//释放内存

void KaliMain(void){
	/*这里是主程序*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//启动信息(BOOTINFO结构体)
	char s[40], mcursor[256], keybuf[32], mousebuf[128];
	int mx, my, i;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC初始化结束，解除CPU的中断禁止 */
	fifo8_init(&keyfifo, 32, keybuf);							//初始化键盘fifo缓冲区
	fifo8_init(&mousefifo, 128, mousebuf);						//初始化鼠标fifo缓冲区
	io_out8(PIC0_IMR, 0xf9); /* 允许PIC1和键盘(11111001) */
	io_out8(PIC1_IMR, 0xef); /* 允许鼠标(11101111) */

	init_keyboard();				//初始化键盘
	enable_mouse(&mdec);			//激活鼠标
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

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

	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL_WHITE, s);			//显示内存
	
	for(;;){
		//停止CPU
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
			io_stihlt();
		} else {
			if (fifo8_status(&keyfifo) != 0) {
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE,  0, 16, 15, 31);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL_WHITE, s);
			} else if (fifo8_status(&mousefifo) != 0) {
				i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0) {
					/* 3字符集齐，显示出来 */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {
						s[2] = 'C';
					}
					boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 32, 16, 32 + 15 * 8 - 1, 31);
					putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL_WHITE, s);
					/* 鼠标指针移动 */
					boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, mx, my, mx + 15, my + 15); /* 隐藏鼠标 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 16) {
						mx = binfo->scrnx - 16;
					}
					if (my > binfo->scrny - 16) {
						my = binfo->scrny - 16;
					}
					sprintf(s, "(%3d, %3d)", mx, my);
					boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 0, 0, 79, 15); /* 隐藏坐标 */
					putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, s); /* 显示坐标 */
					putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16); /* 描画鼠标 */
				}
			}
		}
	}
}


#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 确认CPU是386还是486以上的(现在应该不需要了，不过还是写上) */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 如果是386，即使设定AC=1，AC的值还是会自动回到0 */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* 禁止缓存 */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* 允许缓存 */
		store_cr0(cr0);
	}

	return i;
}


void memman_init(struct MEMMAN *man){
	man->frees = 0;			/* 可用信息数目 */
	man->maxfrees = 0;		/* 用于观察可用状况：frees的最大值 */
	man->lostsize = 0;		/* 释放失败时的内存大小总和 */
	man->losts = 0;			/* 释放失败次数 */
	return;
}

unsigned int memman_total(struct MEMMAN *man){
	/* 报告空余内存大小的合计 */
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size){
	/*分配内存*/
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* 找到足够大的内存 */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* free[i]则减掉一条可用信息 */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* 带入结构体 */
				}
			}
			return a;
		}
	}
	return 0; /* 没有可用空间 */
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size){
	/* 释放内存 */
	int i, j;
	/* 为了便于归纳内存，将free[]按照addr的顺序排列 */
	/* 所以，先决定应该放在哪里 */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* 前面有可用内存 */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* 可以与前面的可用内存归纳到一起 */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* 后面也有 */
				if (addr + size == man->free[i].addr) {
					/* 归纳到一起 */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]删除 */
					/* free[i]变成0后归纳到前面去*/
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* 带入结构体 */
					}
				}
			}
			return 0; /* 成功完成 */
		}
	}
	/* 不能与前面的可用空间归纳到一起 */
	if (i < man->frees) {
		/* 后面还有 */
		if (addr + size == man->free[i].addr) {
			/* 与后面归纳到一起 */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* 成功完成 */
		}
	}
	/* 前后都不能 */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]之后的，向后移动，腾出一点可用空间 */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* 更新最大值 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* 成功完成 */
	}
	/* 不能往后移动 */
	man->losts++;
	man->lostsize += size;
	return -1; /* 失败 */
}

void HariMain(void){
	/*系统启动入口*/
	KaliMain();
}
