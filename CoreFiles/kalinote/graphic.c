/*���ļ�����ͼ�λ���*/

#include "bootpack.h"

void init_palette(void){
	/*��ɫ�庯����Ԥ��15�л�����ɫ������������� - �˴�ԭ�����ڵ�75ҳ*/
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:�� */
		0xff, 0x00, 0x00,	/*  1:���� */
		0x00, 0xff, 0x00,	/*  2:���� */
		0xff, 0xff, 0x00,	/*  3:���� */
		0x00, 0x00, 0xff,	/*  4:���� */
		0xff, 0x00, 0xff,	/*  5:���� */
		0x00, 0xff, 0xff,	/*  6:ǳ���� */
		0xff, 0xff, 0xff,	/*  7:�� */
		0xc6, 0xc6, 0xc6,	/*  8:���� */
		0x84, 0x00, 0x00,	/*  9:���� */
		0x00, 0x84, 0x00,	/* 10:���� */
		0x84, 0x84, 0x00,	/* 11:���� */
		0x00, 0x00, 0x84,	/* 12:���� */
		0x84, 0x00, 0x84,	/* 13:���� */
		0x00, 0x84, 0x84,	/* 14:ǳ���� */
		0x84, 0x84, 0x84	/* 15:���� */
	};
	unsigned char table2[216 * 3];
	int r, g, b;
	set_palette(0, 15, table_rgb);
	for (b = 0; b < 6; b++) {
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	set_palette(16, 231, table2);
	return;
	// C�����е�static charֻ���������ݣ��൱�ڻ���е�DB
}

void set_palette(int start, int end, unsigned char *rgb){
	/*��ɫ�幦�� - �˴�ԭ�����ڵ�75ҳ*/
	int i, eflags;
	eflags = io_load_eflags();	/* ��¼�ж���ɱ�־��ֵ */
	io_cli(); 					/* ���ж���ɱ�־��Ϊ0����ֹ�ж� */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* ��ԭ�ж���ɱ�־ */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	/*���Ʒ��� - �˴�ԭ�����ڵ�84ҳ*/
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_screen8(char *vram, int x, int y){
	/*��ʼ����Ļ*/
	boxfill8(vram, x, COL_LDBLUE, 0, 0, x - 1, y - 1);		//����һ����ɫ����������
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	/*�������� - �˴�ԭ�����ڵ�93ҳ*/
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
	/*�����ַ���(ASCLL����) - �˴�ԭ�����ڵ�96ҳ*/
	extern char fonts[4096];
	struct TASK *task = task_now();
	char *nihongo = (char *) *((int *) 0x0fe8), *font;
	int k, t;
	
	if (task->langmode == 0) {
		/* fonts�ֿ� */
		for (; *s != 0x00; s++) {
			putfont8(vram, xsize, x, y, c, fonts + *s * 16);
			x += 8;
		}
	}
	if (task->langmode == 1) {
		/* nihongo.fnt�ֿ� */
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
				putfont8(vram, xsize, x - 8, y, c, font     );	/* ��벿�� */
				putfont8(vram, xsize, x    , y, c, font + 16);	/* �Ұ벿�� */
			}
			x += 8;
		}
	}
	if (task->langmode == 2) {
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
				putfont8(vram, xsize, x - 8, y, c, font     );	/* ��벿�� */
				putfont8(vram, xsize, x    , y, c, font + 16);	/* �Ұ벿�� */
			}
			x += 8;
		}
	}
	return;
}

void init_mouse_cursor8(char *mouse, char bc){
	/* ׼�����ָ��(16x16) - �˴�ԭ�����ڵ�99ҳ*/
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
		/*��걳��ɫ����*/
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}
