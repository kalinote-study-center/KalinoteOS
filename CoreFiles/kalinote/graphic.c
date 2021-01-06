/*此文件负责图形绘制*/
#include "bootpack.h"
#include <stdio.h>

int read_wallpaper_32 (unsigned char *filename, int x, int y, int *fat, unsigned int *vram);

// void init_palette(void){
	// /* 调色板函数，预置15中基本颜色，可以自行添加 - 此处原内容在第75页 */
	// /* 使用32位色彩模式后不再需要初始化调色板 */
	// static unsigned char table_rgb[16 * 3] = {
		// 0x00, 0x00, 0x00,	/*  0:黑 */
		// 0xff, 0x00, 0x00,	/*  1:亮红 */
		// 0x00, 0xff, 0x00,	/*  2:亮绿 */
		// 0xff, 0xff, 0x00,	/*  3:亮黄 */
		// 0x00, 0x00, 0xff,	/*  4:亮蓝 */
		// 0xff, 0x00, 0xff,	/*  5:亮紫 */
		// 0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		// 0xff, 0xff, 0xff,	/*  7:白 */
		// 0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		// 0x84, 0x00, 0x00,	/*  9:暗红 */
		// 0x00, 0x84, 0x00,	/* 10:暗绿 */
		// 0x84, 0x84, 0x00,	/* 11:暗黄 */
		// 0x00, 0x00, 0x84,	/* 12:暗青 */
		// 0x84, 0x00, 0x84,	/* 13:暗紫 */
		// 0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		// 0x84, 0x84, 0x84	/* 15:暗灰 */
	// };
	// unsigned char table2[216 * 3];
	// int r, g, b;
	// set_palette(0, 15, table_rgb);
	// for (b = 0; b < 6; b++) {
		// for (g = 0; g < 6; g++) {
			// for (r = 0; r < 6; r++) {
				// table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				// table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				// table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			// }
		// }
	// }
	// set_palette(16, 231, table2);
	// return;
	// /* C语言中的static char只能用于数据，相当于汇编中的DB */
// }

// void set_palette(int start, int end, unsigned char *rgb){
	// /*调色板功能 - 此处原内容在第75页*/
	// /* 使用32位色彩模式后不再需要初始化调色板 */
	// int i, eflags;
	// eflags = io_load_eflags();	/* 记录中断许可标志的值 */
	// io_cli(); 					/* 将中断许可标志置为0，禁止中断 */
	// io_out8(0x03c8, start);
	// for (i = start; i <= end; i++) {
		// io_out8(0x03c9, rgb[0] / 4);
		// io_out8(0x03c9, rgb[1] / 4);
		// io_out8(0x03c9, rgb[2] / 4);
		// rgb += 3;
	// }
	// io_store_eflags(eflags);	/* 复原中断许可标志 */
	// return;
// }

void boxfill8(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1){
	/*绘制方块 - 此处原内容在第84页*/
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_screen8(int *vram, int x, int y, int bc){
	/*初始化屏幕*/
	int *fat;
	boxfill8(vram, x, COL_LDBLUE, 0, 0, x - 1, y - 1);		//绘制一个纯色背景当底板
	char s[100];
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	fat = (int *) memman_alloc_4k(memman, 4 * 1024 * 768);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	read_wallpaper_32("back.jpg", x, y, fat, vram);
	
	sprintf(s,"DrawMode = 0x%05x", bc);
	putfonts8_asc(vram, x, 8, 16, COL_BRED, s);
	return;
}

void init_taskbar(int *vram, int x, int y){
	/* 初始化任务栏 */
	boxfill8(vram, x, COL_BGREY,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL_WHITE,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL_BGREY,  0,     y - 26, x -  1, y -  1);
	
	boxfill8(vram, x, COL_WHITE,  3,    y - 24, 59,     y - 24);
	boxfill8(vram, x, COL_WHITE,  2,    y - 24,  2,     y -  4);
	boxfill8(vram, x, COL_DGREY,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL_DGREY, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL_BLACK,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL_BLACK, 60,     y - 24, 60,     y -  3);
	
	boxfill8(vram, x, COL_DGREY, x - 75, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL_DGREY, x - 75, y - 23, x - 75, y -  4);
	boxfill8(vram, x, COL_WHITE, x - 75, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL_WHITE, x -  3, y - 24, x -  3, y -  3);
}

void putfont8(int *vram, int xsize, int x, int y, int c, char *font){
	/*绘制字体 - 此处原内容在第93页*/
	int i;
	int *p;
	char d;	/* data */;
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

void putfont32(int *vram, int xsize, int x, int y, int c, char *font1, char *font2){
	/* 中文字库的渲染方式和日文不同，中文为上下组合，日文为左右组合 */
	int i,k,j,f;
	int *p;
	j=0;
	p=vram+(y+j)*xsize+x;
	j++;
	//上半部分
	for(i=0;i<16;i++){
		for(k=0;k<8;k++){
			if(font1[i]&(0x80>>k)){
				p[k+(i%2)*8]=c;
			}
		}
		for(k=0;k<8/2;k++){
			f=p[k+(i%2)*8];
			p[k+(i%2)*8]=p[8-1-k+(i%2)*8];
			p[8-1-k+(i%2)*8]=f;
		}
		if(i%2){
			p=vram+(y+j)*xsize+x;
			j++;
		}
	}
	//下半部分
	for(i=0;i<16;i++){
		for(k=0;k<8;k++){
			if(font2[i]&(0x80>>k)){
				p[k+(i%2)*8]=c;
			}
		}
		for(k=0;k<8/2;k++){
			f=p[k+(i%2)*8];
			p[k+(i%2)*8]=p[8-1-k+(i%2)*8];
			p[8-1-k+(i%2)*8]=f;
		}
		if(i%2){
			p=vram+(y+j)*xsize+x;
			j++;
		}
	}
	return;
}

void putfonts8_asc(int *vram, int xsize, int x, int y, int c, unsigned char *s)
{
	/*绘制字符串(ASCLL编码) - 此处原内容在第96页*/
	extern char fonts[4096];
	struct TASK *task = task_now();
	char *hzk = (char *) *((int *) 0x10fe8), *font;
	char *nihongo = (char *) *((int *) 0xef9);
	int k, t;
	
	if (task->langmode == 0) {
		/* fonts字库 */
		for (; *s != 0x00; s++) {
			putfont8(vram, xsize, x, y, c, fonts + *s * 16);
			x += 8;
		}
	}
	if (task->langmode == 1) {
		/* 中文 */
		for (; *s != 0x00; s++) {
			if (task->langbyte1 == 0) {
				if (0xa1 <= *s && *s <= 0xfe) {
					task->langbyte1 = *s;
				} else {
					putfont8(vram, xsize, x, y, c, fonts + *s * 16);//只要是半角就使用fonts里面的字符
				}
			} else {
				k = task->langbyte1 - 0xa1;
				t = *s - 0xa1;
				task->langbyte1 = 0;
				font = hzk + (k * 94 + t) * 32;
				putfont32(vram,xsize,x-8,y,c,font,font+16);
			}
			x += 8;
		}
	}
	if (task->langmode == 2) {
		/* 日文Shift-JIS */
		for (; *s != 0x00; s++) {
			if (task->langbyte1 == 0) {
				if ((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc)) {
					task->langbyte1 = *s;
				} else {
					putfont8(vram, xsize, x, y, c, nihongo + *s * 16);
				}
			} else {
				if (0x81 <= task->langbyte1 && task->langbyte1 <= 0x9f) {
					k = (task->langbyte1 - 0x81) * 2;
				} else {
					k = (task->langbyte1 - 0xe0) * 2 + 62;
				}
				if (0x40 <= *s && *s <= 0x7e) {
					t = *s - 0x40;
				} else if (0x80 <= *s && *s <= 0x9e) {
					t = *s - 0x80 + 63;
				} else {
					t = *s - 0x9f;
					k++;
				}
				task->langbyte1 = 0;
				font = nihongo + 256 * 16 + (k * 94 + t) * 32;
				putfont8(vram, xsize, x - 8, y, c, font     );	/* 左半部分 */
				putfont8(vram, xsize, x    , y, c, font + 16);	/* 右半部分 */
			}
			x += 8;
		}
	}
	if (task->langmode == 3) {
		/* 日文EUC */
		for (; *s != 0x00; s++) {
			if (task->langbyte1 == 0) {
				if (0x81 <= *s && *s <= 0xfe) {
					task->langbyte1 = *s;
				} else {
					putfont8(vram, xsize, x, y, c, nihongo + *s * 16);
				}
			} else {
				k = task->langbyte1 - 0xa1;
				t = *s - 0xa1;
				task->langbyte1 = 0;
				font = nihongo + 256 * 16 + (k * 94 + t) * 32;
				putfont8(vram, xsize, x - 8, y, c, font     );	/* 左半部分 */
				putfont8(vram, xsize, x    , y, c, font + 16);	/* 右半部分 */
			}
			x += 8;
		}
	}
	return;
}

void init_mouse_cursor8(int *mouse, int bc){
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

void putblock8_8(int *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, int *buf, int bxsize){
		/*鼠标背景色处理*/
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

int read_wallpaper_32 (unsigned char *filename, int x, int y, int *fat, unsigned int *vram) {
	/* 32位色彩模式下读取壁纸 */
	int i, j, x0, y0, fsize, info[4];
	unsigned char *filebuf, r, g, b;
	struct RGB *picbuf;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct DLL_STRPICENV *env;
	finfo = file_search("back.jpg", (struct FILEINFO *)(ADR_DISKIMG + 0x002600), 224);  /* 查找壁纸文件 */
	if (finfo == 0) {
		/* 读取文件失败 */
		return -1;
	}
	fsize = finfo->size;
	filebuf = (unsigned char *) memman_alloc_4k(memman, fsize);
	filebuf = file_loadfile2(finfo->clustno, &fsize, fat);
	env = (struct DLL_STRPICENV *) memman_alloc_4k(memman, sizeof(struct DLL_STRPICENV));
	info_JPEG(env, info, fsize, filebuf);
	picbuf = (struct RGB *) memman_alloc_4k(memman, info[2] * info[3] * sizeof(struct RGB) * 4);
	decode0_JPEG(env, fsize, filebuf, 4, (unsigned char *) picbuf, 0);
	x0 = (int) ((x - info[2]) / 2);
	y0 = (int) ((y - info[3]) / 2);
	for (i = 0; i < info[3]; i++) {
		for (j = 0; j < info[2]; j++) {
			r = picbuf[i * info[2] + j].r;
			g = picbuf[i * info[2] + j].g;
			b = picbuf[i * info[2] + j].b;
			vram[(y0 + i) * x + (x0 + j)] = b | g << 8 | r << 16 | 0x00 << 24;
		}
	}
	memman_free_4k(memman, (int) filebuf, fsize);
	memman_free_4k(memman, (int) picbuf, info[2] * info[3] * sizeof(struct RGB) * 4);
	memman_free_4k(memman, (int) env, sizeof(struct DLL_STRPICENV));
	return 0;
}
