/* KalinoteOS ϵͳ���� */
#include "../bootpack.h"

int *kal_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
	/* ���Ÿ��ⲿ�����ϵͳAPI */
	/* ��������Ĵ�������ļĴ����ǰ���PUSHAD��д�� */
	int i;
	struct TASK *task = task_now();
	int ds_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) SHTCTL_ADDR);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) FIFO_ADDR);
	struct FILEINFO *finfo;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct WINDOW *window;
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) SYSINFO_ADDR);
	int *reg = &eax + 1;	/* eax����ĵ�ַ */
		/* ǿ�и�дͨ��PUSHAD�����ֵ�����ڷ�����ֵ */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */

	if (edx == 1) {
		//��ӡ�ַ�
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		//��ӡ�ַ���(���ַ�����0��β)
		cons_putstr0(cons, (char *) ebx + ds_base);
	} else if (edx == 3) {
		//��ӡ�ַ���(ָ������)
		cons_putstr1(cons, (char *) ebx + ds_base, ecx);
	} else if (edx == 4) {
		//��������
		return &(task->tss.esp0);
	} else if (edx == 5) {
		//���ɴ���
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags = SHEET_APIWIN;			/* �ⲿAPI���� */
		sheet_setbuf(sht, (int *) ebx + ds_base, esi, edi, eax);
		window = make_window8(sht, esi, edi, TIT_ACT_DEFAULT, TIT_DEACT_DEFAULT, (char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* ������ͼ��߶�ָ��Ϊ��ǰ�������ͼ��ĸ߶ȣ�����Ƶ��ϲ� */
		reg[7] = window->whandle;			/* (ͨ��EAX�Ĵ���)���ش��ھ��((int)sht) */		/* �пհ�����ĳ�ֱ�ӷ���window�ṹ��((int) window) */
	} else if (edx == 6) {
		//�ڴ����ϻ����ַ�
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		//�ڴ����ϻ��Ʒ���
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		/* ��ʼ���ڴ���� */
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0;	/* ��16�ֽ�Ϊ��λ */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		/* �����ڴ� */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* ��16�ֽ�Ϊ��λ��λȡ�� */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	} else if (edx == 10) {
		/* �ͷ��ڴ� */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* ��16�ֽ�Ϊ��λ��λȡ�� */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		/* �������ص� */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		/* ����ˢ�´��� */
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		/* ����ֱ�� */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		kal_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0) {
			if (eax > esi) {
				i = eax;
				eax = esi;
				esi = i;
			}
			if (ecx > edi) {
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {
		/* �رմ��� */
		taskbar_removewin(((struct WINDOW *)(((struct SHEET *) ebx)->win))->tskwinbtn);
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) {
		/* �������� */
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					/* FIFOΪ�գ����ߣ�ֱ�����̽��յ����� */
					task_sleep(task);
				} else {
					/* FIFOΪ�գ�����-1 */
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) { /* ��궨ʱ�� */
				/* Ӧ�ó�������ʱ����ʾ��꣬������ǽ��´���ʾ�õ�ֵ��1 */
				timer_init(cons->timer, &task->fifo, 1); /* ��1 */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* ���ON */
				cons->cur_c = COL_WHITE;
			}
			if (i == 3) {	/* ���OFF */
				cons->cur_c = -1;
			}
			if (i == 4) {	/* ֻ�ر������ */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);	/* 2024��2279 */
				cons->sht = 0;
				io_sti();
			}
			if (256 <= i) { /* �������ݣ�ͨ������A�� */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		/* ��ȡ��ʱ�� */
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1;	/* �����Զ�ȡ�� */
	} else if (edx == 17) {
		/* ���ö�ʱ���������� */
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		/* ��ʱ��ʱ���趨 */
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		/* �ͷŶ�ʱ�� */
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		/* ������ */
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {
		/* ���ļ� */
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0) {
				break;
			}
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char *) ebx + ds_base,
					(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
			if (finfo != 0) {
				reg[7] = (int) fh;
				fh->buf = (char *) memman_alloc_4k(memman, finfo->size);
				fh->size = finfo->size;
				fh->pos = 0;
				file_loadfile(finfo->clustno, finfo->size, fh->buf, task->fat, (char *) (ADR_DISKIMG + 0x003e00));
			}
		}
	} else if (edx == 22) {
		/* �ر��ļ� */
		fh = (struct FILEHANDLE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		/* �ļ���λ */
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			fh->pos = ebx;
		} else if (ecx == 1) {
			fh->pos += ebx;
		} else if (ecx == 2) {
			fh->pos = fh->size + ebx;
		}
		if (fh->pos < 0) {
			fh->pos = 0;
		}
		if (fh->pos > fh->size) {
			fh->pos = fh->size;
		}
	} else if (edx == 24) {
		/* ��ȡ�ļ���С */
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		/* �ļ���ȡ */
		fh = (struct FILEHANDLE *) eax;
		for (i = 0; i < ecx; i++) {
			if (fh->pos == fh->size) {
				break;
			}
			*((char *) ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) {
		/* ������API */
		i = 0;
		for (;;) {
			*((char *) ebx + ds_base + i) =  task->cmdline[i];
			if (task->cmdline[i] == 0) {
				break;
			}
			if (i >= ecx) {
				break;
			}
			i++;
		}
		reg[7] = i;
	} else if (edx == 27) {
		/* ��ѯ����ģʽ */
		reg[7] = task->langmode;
	} else if (edx == 28) {
		/* ����icon */
		
	} else if (edx == 29) {
		/* �������̨ */
		cmd_cls(cons);
	} else if (edx == 30) {
		/* ��ȡϵͳʱ�䣺�� */
		reg[7] = get_year();
	} else if (edx == 31) {
		/* ��ȡϵͳʱ�䣺�� */
		reg[7] = get_mon_hex();
	} else if (edx == 32) {
		/* ��ȡϵͳʱ�䣺���� */
		reg[7] = get_day_of_week();
	} else if (edx == 33) {
		/* ��ȡϵͳʱ�䣺�� */
		reg[7] = get_day_of_month();
	} else if (edx == 34) {
		/* ��ȡϵͳʱ�䣺ʱ */
		reg[7] = get_hour_hex();
	} else if (edx == 35) {
		/* ��ȡϵͳʱ�䣺�� */
		reg[7] = get_min_hex();
	} else if (edx == 36) {
		/* ��ȡϵͳʱ�䣺�� */
		reg[7] = get_sec_hex();
	} else if (edx == 37) {
		/* ��ȡϵͳ����ʱ��(��) */
		reg[7] = sysinfo->time_counter;
	} else if (edx == 38) {
		/* �Ծ���λ����ʾͼƬ */
		/* EBX: ���ھ��	EAX: �ļ���	ESI: ͼƬx	EDI: ͼƬy */
		int img_info;
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		img_info = read_image_32(((char *) eax + ds_base), esi, edi, sht->bxsize, task->fat, sht->buf);
		if(img_info != -1){
			/* ��ȡʧ�ܲ�ִ�д˾� */
			sheet_refresh(sht, 0, 0, sht->bxsize, sht->bysize);
		}
	} else if (edx == 39) {
		/* ִ��ϵͳָ��(��cmd���ͼ�ֵ) */
		unsigned char *p;
        for (p = (unsigned char *) ebx + ds_base; *p != 0; p++) {
            if (*p != 0x0d) {
                io_cli();
                fifo32_put(&task->fifo, *p + 256);
                io_sti();
            }
        }
	}
	return 0;
}

void kal_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col){
	/* ����ֱ�� */
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}

	return;
}
