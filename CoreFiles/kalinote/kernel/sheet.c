/* 图层管理 */
#include "../bootpack.h"

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned int *vram, int xsize, int ysize){
	/* 初始化图层系统 */
	struct SHTCTL *ctl;
	int i;
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));		/* 分配用于图层控制结构体变量的空间，前面的(struct SHTCTL *)是将分配到的内存位置作为“图层控制结构体变量”来使用 */
	if (ctl == 0) {
		/* 空间分配失败 */
		goto err;
	}
	ctl->map = (unsigned int *) memman_alloc_4k(memman, xsize * ysize * 4);
	if (ctl->map == 0) {
		memman_free_4k(memman, (int) ctl, sizeof (struct SHTCTL));
		goto err;
	}
	ctl->vram = vram;						/* 系统VRAM地址 */
	ctl->xsize = xsize;						/* 可显示的x大小 */
	ctl->ysize = ysize;						/* 可显示的y大小 */
	ctl->top = -1; 							/* 一个SHEET都没有(给SHTCTL结构体的"top(顶层层数)"赋值-1，表示没有图册) */
	ctl->fsheet = 0;						/* 父图层 */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = SHEET_NO_USE;			/* 标记为未使用 */
		ctl->sheets0[i].ctl = ctl;						/* 记录所属 */
	}
err:
	return ctl;
}

struct SHEET *sheet_alloc(struct SHTCTL *ctl){
	/* 分配一个新的图层 */
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		/* 在sheets0[]中循环查找一个没有使用的图层 */
		if (ctl->sheets0[i].flags == SHEET_NO_USE) {
			sht = &ctl->sheets0[i];			/* sht的地址(ctl->sheets0[i]的地址) */
			sht->flags = SHEET_USE;			/* 标记为正在使用(作为窗口) */
			sht->height = -1;				/* 高度设置为-1，表示高度还没有设置，不显示 */
			sht->task = 0;					/* 不使用自动关闭 */
			sht->win = 0;					/* 默认给0，这个参数用于保存窗口(或菜单栏等)的地址 */
			// sht->subctl = 0;				/* 子图层管理器，管理子图层 */
			/* 创建一个子图层管理器 */
			// if(subctl != 0) {
				// /* 需要时再分配，避免浪费内存 */
				// sht->subctl = shtctl_init(memman, sht->buf, sht->bxsize, sht->bysize);
			// } else {
				// sht->subctl = 0;
			// }
			/* 如果需要使用子图层管理器，需要在sheet_setbuf以后通过sht->subctl = shtctl_init(memman, sht_buf, sht->bxsize, sht->bysize);设置 */
			/**************************************************************************************************
			* 在使用子图层管理器的时候一定要注意sht->subctl是否已经定义，还有是否没有图层(图层管理器top = -1) *
			**************************************************************************************************/
			sht->subctl = 0;
			return sht;
		}
	}
	return 0;								/* 所有的SHEET都处于正在使用的状态 */
}

void sheet_setbuf(struct SHEET *sht, unsigned int *buf, int xsize, int ysize, int col_inv){
	/* 设定图层缓冲区和透明色 */
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0){
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned int *buf, sid, *map = ctl->map;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0; /* 将进行了减法计算的地址作为图层号码使用 */
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		if (sht->col_inv == -1) {
			/* 无透明色图层专用高速版 */
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		} else {
			/* 有透明图层普通版 */
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (buf[by * sht->bxsize + bx] != sht->col_inv) {
						map[vy * ctl->xsize + vx] = sid;
					}
				}
			}
		}
	}
	return;
}

void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1){
	/* 区域刷新 */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned int *buf, *vram = ctl->vram, *map = ctl->map, sid;
	struct SHEET *sht;
	/* 如果refresh的范围超出了画面则修正，防止vram溢出 */
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= h1; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				if (map[vy * ctl->xsize + vx] == sid) {
					vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
			}
		}
	}
	return;
}

void sheet_updown(struct SHEET *sht, int height){
	/* 调整图层高度 */
	struct SHTCTL *ctl = sht->ctl;
	int h, old = sht->height; /* 储存设置前的高度信息 */

	/*
	*   这个函数大概在做的就是，重新指定图层高度后，判断图层的新高度与原有图层的关系(更高、更低或者直接隐藏图层)然后做出相应的调整
	*/

	/* 如果指定的高度过高或过低，则进行修正 */
	if (height > ctl->top + 1) {
		/* 如果高于top层两层及以上，则调整到top层 */
		height = ctl->top + 1;
	}
	if (height < -1) {
		/* 如果小于-1层，则调整到-1层(不显示) */
		height = -1;
	}
	sht->height = height; /* 设定修正后图层高度 */

	/* 下面主要是进行sheets[]的重新排列 */
	if (old > height) {	/* 比以前低 */
		if (height >= 2) {
			/* 把中间的往上提 */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		} else {	/* 隐藏 */
			if (ctl->top > old) {
				/* 把上面的降下来 */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* 由于显示中的图层减少了一个，所以最上面的图层高度下降 */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1); /* 按新图层的信息重新绘制画面 */
		}
	} else if (old < height) {	/* 比以前高 */
		if (old >= 2) {
			/* 把中间的拉下去 */
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* 由隐藏状态转为显示状态 */
			/* 将已在上面的提上来 */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; /* 由于已显示的图层增加了1个，所以最上面的图层高度增加 */
		}
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height); /* 按新图层信息重新绘制画面 */
	}
	return;
}

void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1){
	if (sht->height >= 1) { /* 如果正在显示，则按新图层信息刷新画面 */
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
	return;
}

void sheet_slide(struct SHEET *sht, int vx0, int vy0){
	/* 移动图层位置(不会改变图层高度) */
	struct SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 1) { /* 如果正在显示，则按新图层信息刷新画面 */
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		sheet_refreshmap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
	return;
}

void sheet_free(struct SHEET *sht){
	/* 释放已使用的图层 */
	if (sht->height >= 2) {
		/* 不是sht_back或she_task_bar */
		sheet_updown(sht, -1); /* 如果处于显示状态，则先设定为隐藏 */
	}
	sht->flags = SHEET_NO_USE; /* 将图层标记为未使用状态 */
	return;
}


