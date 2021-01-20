/* ͼ����� */
#include "../bootpack.h"

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned int *vram, int xsize, int ysize){
	/* ��ʼ��ͼ��ϵͳ */
	struct SHTCTL *ctl;
	int i;
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));		/* ��������ͼ����ƽṹ������Ŀռ䣬ǰ���(struct SHTCTL *)�ǽ����䵽���ڴ�λ����Ϊ��ͼ����ƽṹ���������ʹ�� */
	if (ctl == 0) {
		/* �ռ����ʧ�� */
		goto err;
	}
	ctl->map = (unsigned int *) memman_alloc_4k(memman, xsize * ysize * 4);
	if (ctl->map == 0) {
		memman_free_4k(memman, (int) ctl, sizeof (struct SHTCTL));
		goto err;
	}
	ctl->vram = vram;						/* ϵͳVRAM��ַ */
	ctl->xsize = xsize;						/* ����ʾ��x��С */
	ctl->ysize = ysize;						/* ����ʾ��y��С */
	ctl->top = -1; 							/* һ��SHEET��û��(��SHTCTL�ṹ���"top(�������)"��ֵ-1����ʾû��ͼ��) */
	ctl->fsheet = 0;						/* ��ͼ�� */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = SHEET_NO_USE;			/* ���Ϊδʹ�� */
		ctl->sheets0[i].ctl = ctl;						/* ��¼���� */
	}
err:
	return ctl;
}

struct SHEET *sheet_alloc(struct SHTCTL *ctl){
	/* ����һ���µ�ͼ�� */
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		/* ��sheets0[]��ѭ������һ��û��ʹ�õ�ͼ�� */
		if (ctl->sheets0[i].flags == SHEET_NO_USE) {
			sht = &ctl->sheets0[i];			/* sht�ĵ�ַ(ctl->sheets0[i]�ĵ�ַ) */
			sht->flags = SHEET_USE;			/* ���Ϊ����ʹ��(��Ϊ����) */
			sht->height = -1;				/* �߶�����Ϊ-1����ʾ�߶Ȼ�û�����ã�����ʾ */
			sht->task = 0;					/* ��ʹ���Զ��ر� */
			sht->win = 0;					/* Ĭ�ϸ�0������������ڱ��洰��(��˵�����)�ĵ�ַ */
			// sht->subctl = 0;				/* ��ͼ���������������ͼ�� */
			/* ����һ����ͼ������� */
			// if(subctl != 0) {
				// /* ��Ҫʱ�ٷ��䣬�����˷��ڴ� */
				// sht->subctl = shtctl_init(memman, sht->buf, sht->bxsize, sht->bysize);
			// } else {
				// sht->subctl = 0;
			// }
			/* �����Ҫʹ����ͼ�����������Ҫ��sheet_setbuf�Ժ�ͨ��sht->subctl = shtctl_init(memman, sht_buf, sht->bxsize, sht->bysize);���� */
			/**************************************************************************************************
			* ��ʹ����ͼ���������ʱ��һ��Ҫע��sht->subctl�Ƿ��Ѿ����壬�����Ƿ�û��ͼ��(ͼ�������top = -1) *
			**************************************************************************************************/
			sht->subctl = 0;
			return sht;
		}
	}
	return 0;								/* ���е�SHEET����������ʹ�õ�״̬ */
}

void sheet_setbuf(struct SHEET *sht, unsigned int *buf, int xsize, int ysize, int col_inv){
	/* �趨ͼ�㻺������͸��ɫ */
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
		sid = sht - ctl->sheets0; /* �������˼�������ĵ�ַ��Ϊͼ�����ʹ�� */
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
			/* ��͸��ɫͼ��ר�ø��ٰ� */
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		} else {
			/* ��͸��ͼ����ͨ�� */
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
	/* ����ˢ�� */
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned int *buf, *vram = ctl->vram, *map = ctl->map, sid;
	struct SHEET *sht;
	/* ���refresh�ķ�Χ�����˻�������������ֹvram��� */
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
	/* ����ͼ��߶� */
	struct SHTCTL *ctl = sht->ctl;
	int h, old = sht->height; /* ��������ǰ�ĸ߶���Ϣ */

	/*
	*   ���������������ľ��ǣ�����ָ��ͼ��߶Ⱥ��ж�ͼ����¸߶���ԭ��ͼ��Ĺ�ϵ(���ߡ����ͻ���ֱ������ͼ��)Ȼ��������Ӧ�ĵ���
	*/

	/* ���ָ���ĸ߶ȹ��߻���ͣ���������� */
	if (height > ctl->top + 1) {
		/* �������top�����㼰���ϣ��������top�� */
		height = ctl->top + 1;
	}
	if (height < -1) {
		/* ���С��-1�㣬�������-1��(����ʾ) */
		height = -1;
	}
	sht->height = height; /* �趨������ͼ��߶� */

	/* ������Ҫ�ǽ���sheets[]���������� */
	if (old > height) {	/* ����ǰ�� */
		if (height >= 2) {
			/* ���м�������� */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		} else {	/* ���� */
			if (ctl->top > old) {
				/* ������Ľ����� */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* ������ʾ�е�ͼ�������һ���������������ͼ��߶��½� */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1); /* ����ͼ�����Ϣ���»��ƻ��� */
		}
	} else if (old < height) {	/* ����ǰ�� */
		if (old >= 2) {
			/* ���м������ȥ */
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* ������״̬תΪ��ʾ״̬ */
			/* ����������������� */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; /* ��������ʾ��ͼ��������1���������������ͼ��߶����� */
		}
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height); /* ����ͼ����Ϣ���»��ƻ��� */
	}
	return;
}

void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1){
	if (sht->height >= 1) { /* ���������ʾ������ͼ����Ϣˢ�»��� */
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
	return;
}

void sheet_slide(struct SHEET *sht, int vx0, int vy0){
	/* �ƶ�ͼ��λ��(����ı�ͼ��߶�) */
	struct SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 1) { /* ���������ʾ������ͼ����Ϣˢ�»��� */
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		sheet_refreshmap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
	return;
}

void sheet_free(struct SHEET *sht){
	/* �ͷ���ʹ�õ�ͼ�� */
	if (sht->height >= 2) {
		/* ����sht_back��she_task_bar */
		sheet_updown(sht, -1); /* ���������ʾ״̬�������趨Ϊ���� */
	}
	sht->flags = SHEET_NO_USE; /* ��ͼ����Ϊδʹ��״̬ */
	return;
}


