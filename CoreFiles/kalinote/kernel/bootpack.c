/*����ʱ��������*/
#include <stdio.h>
#include "../bootpack.h"

void KaliMain(void){
	/* ϵͳ��� */
	
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct TIMER *timer;
	struct FIFO32 fifo, keycmd;
	int fifobuf[128], keycmd_buf[32];
	int mx, my, i, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned int *buf_back, buf_mouse[256], *buf_task_bar;/* , *subbuf_back; */
	struct SHEET *sht_back, *sht_mouse, *sht_task_bar, *sht_debug_cons;/* , *btn_sht; */
	struct TASK *task_a, *task;
	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int j, x, y, mmx = -1, mmy = -1, mmx2 = 0, subx, suby;
	struct SHEET *sht = 0, *subsht = 0, *point_sht = 0, *key_win, *sht2;/* , *subsht_back; */			/* point_sht��������������ƶ�ʱָ��Ĳ�� */
	int *fat_ch, *fat_jp;
	unsigned char *chinese, *nihongo;
	struct FILEINFO *finfo_ch;
	struct FILEINFO *finfo_jp;
	extern char fonts[4096];
	struct SYSINFO sysinfo;
	struct MENU *start_menu; /* , *desktop_menu; */
	/* struct WINDOW *debug_window; */
	struct BUTTON *start_button;
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', 0x27, 0,   0,   0x5c, 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', '@', '#', '$', '%', 0x5e, 0x26, '*', '(', ')', '_', '+', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 0x22, 0,   0,   '|', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '|', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};

	/* ��ʼ��sysinfo */
	*((int *) SYSINFO_ADDR) = (int) &sysinfo;	
	sysinfo.sysmode = 0;
	sysinfo.sysmmainver = 1;
	sysinfo.sysver = 400;

	/* ��ʼ�� */
	init_gdtidt();													// ��ʼ��GDT��IDT
	init_pic();														// ��ʼ���жϿ�����
	io_sti(); 														// IDT/PIC��ʼ�����������CPU���жϽ�ֹ
	fifo32_init(&fifo, 128, fifobuf, 0);							// ��ʼ��FIFO������
	init_pit();														// ��ʼ����ʱ��
	init_keyboard(&fifo, 256);										// ��ʼ������FIFO������
	enable_mouse(&fifo, 512, &mdec);								// ��ʼ�����FIFO������
	io_out8(PIC0_IMR, 0xf8); 										// ����PIT��PIC1��PS/2����(11111000)
	io_out8(PIC1_IMR, 0xaf); 										// ����PS/2����Ӳ��(10101111)
	
	fifo32_init(&keycmd, 32, keycmd_buf, 0);
	*((int *) FIFO_ADDR) = (int) &fifo;		/* ��fifo�������浽0x0fec */

	/* �ڴ洦�� */
	memtotal = memtest(SYS_MEMORY, 0xffffffff); /* ���Կ����ڴ棬���ʶ��3972MB(��ϵͳռ����) */
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* ��0x00001000 - 0x0009efff��գ���һ��������ģʽ��ȡ���̵����ݣ��ڽ��뱣��ģʽ���ⲿ�����ݱ��ƶ���0x00100000�Ժ���һ���ڴ���������ɺ���������; */
	memman_free(memman, SYS_MEMORY, memtotal - SYS_MEMORY); /* ��ճ���ϵͳռ�õ����б���ģʽ��չ�ڴ�(0x00400000�Ժ�) */
	sysinfo.memtotal = memtotal;
	
	/* Ӳ�� */
	init_ide_hd(memman);											// ��ʼ��IDEӲ�̿��Ƴ���
	
	//init_palette();												// ��ʼ����ɫ��
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);					/* ϵͳ������ */
	*((int *) SHTCTL_ADDR) = (int) shtctl;		/* ��shtctl��ֵ�浽��ַ0xfe4�ĵط� */
	task_a->langmode = 0;

	/* DEBUG cons */
	*((int *) DEBUG_ADDR) = (int)open_console(shtctl, memtotal, 1);
	sht_debug_cons = (struct SHEET *) *((int *) DEBUG_ADDR);
	sht_debug_cons->flags = SHEET_DEBUG_CONS;

	/* sht_back */
	sht_back  = sheet_alloc(shtctl);
	buf_back  = (unsigned int *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * 4);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	init_screen8(buf_back, binfo->scrnx, binfo->scrny, binfo->vmode);
	sht_back->flags = SHEET_BACK;
	/* �����Ҽ��˵��� */
	// desktop_menu = make_menu(memman, 0, 0);
	// add_options(desktop_menu, "Desktop Menu");
	// add_options(desktop_menu, "refresh");
	// add_options(desktop_menu, "nouse1");
	// add_options(desktop_menu, "nouse2");
	/* ���Դ��� */
	// sht_back->subctl = shtctl_init(memman, sht_back->buf, sht_back->bxsize, sht_back->bysize);		/* ������ͼ������� */
	// subsht_back = sheet_alloc(sht_back->subctl);
	// subbuf_back = (unsigned int *) memman_alloc_4k(memman, 16 * 64 * 4);
	// sheet_setbuf(subsht_back, subbuf_back, 64, 16, -1);
	// subsht_back->flags = SHEET_NO_TITLE;
	// putfonts8_asc_sht(subsht_back, 0, 0, 0x000000, 0xffffff, "testtest", 8);
	// sheet_slide(subsht_back, 10, 10);
	// sheet_updown(subsht_back,  0);
	/* ���Դ��� */

	/* sht_taskbar */
	//init_taskbar(buf_back, binfo->scrnx, binfo->scrny);
	sht_task_bar = sheet_alloc(shtctl);
	buf_task_bar = (unsigned int *) memman_alloc_4k(memman, binfo->scrnx * 28 * 4);			/* �������Ҫ��������task_bar�Ľṹ�壬���԰�task_bar��y�߶ȷŽ�ȥ������������ʹ�ù̶�ֵ28(���������sheet_setbuf) */
	sheet_setbuf(sht_task_bar, buf_task_bar, binfo->scrnx, 28, -1);
	start_menu = init_taskbar(memman, buf_task_bar, binfo->scrnx, 28);
	putfonts8_asc_sht(sht_task_bar, 6, 8, COL_BLACK, COL_BGREY, "Function", 8);
	sht_task_bar->flags = SHEET_TASKBAR;
	/* start��ť */
	start_button = make_button(memman, 40, 23, 150, 4, "test", COL_BGREY, onStartButtonClick);
	show_button(sht_task_bar, memman, start_button);
	
	/* sht_cons */
	key_win = open_console(shtctl, memtotal, 0);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2;								/* ������λ�����ڻ������� */
	my = (binfo->scrny - 28 - 16) / 2;
	sht_mouse->flags = SHEET_MOUSE;

	sheet_slide(sht_back,  0,  0);
	sheet_slide(sht_task_bar, 0, binfo->scrny - 28);
	sheet_slide(key_win,   32, 4);
	sheet_slide(sht_mouse, mx, my);
	sheet_slide(sht_debug_cons, 10, 10);
	sheet_updown(sht_back,  0);
	sheet_updown(sht_task_bar, 1);
	sheet_updown(key_win,   2);
	sheet_updown(sht_mouse, 3);
	sheet_updown(sht_debug_cons, -1);
	keywin_on(key_win);
	
	/* Ϊ�˱���ͼ��̵�ǰ״̬���ڳ�ͻ����һ��ʼ�Ƚ������� */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);
	
	/* ����HZK�ֿ� */
	//chinese = (unsigned char *) memman_alloc_4k(memman, 0x5d5d * 32);
	fat_ch = (int *) memman_alloc_4k(memman, 4 * 2880);
	file_readfat(fat_ch, (unsigned char *) (ADR_DISKIMG + 0x000200));
	finfo_ch = file_search("HZK16.fnt", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo_ch != 0) {
		//file_loadfile(finfo_ch->clustno, finfo_ch->size, chinese, fat_ch, (char *) (ADR_DISKIMG + 0x003e00));
		i = finfo_ch->size;
		chinese = file_loadfile2(finfo_ch->clustno, &i, fat_ch);
	} else {
		chinese = (unsigned char *) memman_alloc_4k(memman, 0x5d5d * 32);
		for (i = 0; i < 16 * 256; i++) {
			chinese[i] = fonts[i]; /* û���ֿ⣬��ǲ���ֱ�Ӹ���Ӣ���ֿ� */
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			chinese[i] = 0xff; /* û���ֿ⣬ȫ�ǲ�����0xff��� */
		}
	}
	*((int *) 0x10fe8) = (int) chinese;
	memman_free_4k(memman, (int) fat_ch, 4 * 2880);
	
	/* ����nihongo�ֿ� */
	//nihongo = (unsigned char *) memman_alloc_4k(memman, 16 * 256 + 32 * 94 * 47);
	fat_jp = (int *) memman_alloc_4k(memman, 4 * 2880);
	file_readfat(fat_jp, (unsigned char *) (ADR_DISKIMG + 0x000200));
	finfo_jp = file_search("nihongo.fnt", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo_jp != 0) {
		//file_loadfile(finfo_jp->clustno, finfo_jp->size, nihongo, fat_jp, (char *) (ADR_DISKIMG + 0x003e00));
		i = finfo_jp->size;
		nihongo = file_loadfile2(finfo_jp->clustno, &i, fat_jp);
	} else {
		nihongo = (unsigned char *) memman_alloc_4k(memman, 16 * 256 + 32 * 94 * 47);
		for (i = 0; i < 16 * 256; i++) {
			nihongo[i] = fonts[i]; /* û���ֿ⣬��ǲ���ֱ�Ӹ���Ӣ���ֿ�*/
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			nihongo[i] = 0xff; /* û���ֿ⣬��ǲ���ֱ�Ӹ���Ӣ���ֿ� */
		}
	}
	*((int *) 0x0ef9) = (int) nihongo;
	memman_free_4k(memman, (int) fat_jp, 4 * 2880);
	
	/* ʱ�Ӹ��¶�ʱ�� */
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 100);	/* ÿ1�����һ��ʱ�� */
	
	for(;;){
		/***************************************************************
		*         �����￪ʼ������ϵͳ��ʼ����ϣ���ʼ��������         *
		***************************************************************/
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* �����������̿��������͵����ݣ������� */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			/* FIFOΪ�գ������ڸ��õĻ�ͼ����ʱ����ִ�� */
			if (new_mx >= 0) {
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			} else if (new_wx != 0x7fffffff) {
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			} else {
				task_sleep(task_a);
				io_sti();
			}
		} else {
			i = fifo32_get(&fifo); /* ��fifo��ȡ������ */
			io_sti();
			if (key_win != 0 && key_win->flags == 0) {	/* ���ڱ��ر� */
				if (shtctl->top == 2) {	/* ֻʣ��ꡢ������������ʱ */
					key_win = 0;
				} else {
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
				}
			}
			if (i == 1) {
				/* ����ʱ�� */
				/*���½�ʱ����ʾ*/
				/*
				* ���ﻹ�����Ż�һ�£����԰�ʱ����ֱ���ʾ��ˢ�£����ⲻ��Ҫ�Ļ���ˢ��
				*/
				timer_init(timer, &fifo, 1);		/* ����һ����ʱ������һ���ٴ����� */
				sprintf(s,"%02d:%02d:%02d",get_hour_hex(), get_min_hex(), get_sec_hex());
				putfonts8_asc_sht(sht_task_bar, sht_task_bar->bxsize - 70, sht_task_bar->bysize - 20, COL_BLACK, COL_BGREY, s, 8);
				/* ���� */
				sprintf(s,"%d/%d/%d",get_year(), get_mon_hex(), get_day_of_month());
				putfonts8_asc_sht(sht_task_bar, sht_task_bar->bxsize - 160, sht_task_bar->bysize - 20, COL_BLACK, COL_BGREY, s, 10);
				timer_settime(timer, 100);
				sheet_refresh(sht_task_bar, sht_task_bar->bxsize - 160, sht_task_bar->bysize - 20, sht_task_bar->bxsize - 70 + 8 * 8, sht_task_bar->bysize - 50 + 16);
			} else if (i == 2) {
				/* ���µ������ */
				/* ����Ĵ���ʵ���ϸ�Shift+F2��һ���� */
				if (key_win != 0) {
					keywin_off(key_win);
				}
				keywin_off(key_win);
				key_win = open_console(shtctl, memtotal, 0);
				sheet_slide(key_win, 32, 4);
				sheet_updown(key_win, shtctl->top);
				keywin_on(key_win);
			} else if (256 <= i && i <= 511) { /* �������� */
				if (i < 0x80 + 256) { /* ����������ת��Ϊ�ַ����� */
					if (key_shift == 0) {
						s[0] = keytable0[i - 256];
					} else {
						s[0] = keytable1[i - 256];
					}
				} else {
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z') {	/* �������ַ�ΪӢ����ĸʱ */
					if (((key_leds & 4) == 0 && key_shift == 0) ||
							((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20;	/* ����д��ĸת��ΪСд��ĸ */
					}
				}
				if (s[0] != 0 && key_win != 0) { /* һ���ַ���BackSpace��Enter */
					fifo32_put(&key_win->task->fifo, s[0] + 256);
				}
				if (i == 256 + 0x0f && key_win != 0){	/* Tab */
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0 || j == 1) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
				}
				if (i == 256 + 0x2a) {	/* ��SHIFT ON */
					key_shift |= 1;
				}
				if (i == 256 + 0x36) {	/* ��SHIFT ON */
					key_shift |= 2;
				}
				if (i == 256 + 0xaa) {	/* ��SHIFT OFF */
					key_shift &= ~1;
				}
				if (i == 256 + 0xb6) {	/* ��SHIFT OFF */
					key_shift &= ~2;
				}
				if (i == 256 + 0x3a) {	/* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) {	/* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) {	/* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x3b && key_shift != 0 && key_win != 0) {/* Shift+F1 ǿ��ֹͣӦ�ó��� */
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {	
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli();	/* ǿ�ƽ�������ʱ��ֹ�����л� */
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);	/* Ϊ��ȷʵִ�н������������������״̬���� */
					}
				}
				if (i == 256 + 0x3c && key_shift != 0) {	/* Shift+F2 ���µ������ */
					/* �Զ������뽹���л����´򿪵������д��� */
					if (key_win != 0) {
						keywin_off(key_win);
					}
					keywin_off(key_win);
					key_win = open_console(shtctl, memtotal, 0);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				}
				if (i == 256 + 0x57 && shtctl->top > 2) {	/* F11 �л����� */
					sheet_updown(shtctl->sheets[2], shtctl->top - 1);
				}
				if (i == 256 + 0xfa) {	/* ���̳ɹ����յ����� */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {	/* ����û�гɹ����յ����� */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			} else if (512 <= i && i <= 767) { /* ������� */
				if (mouse_decode(&mdec, i - 512) != 0) {
					/* ���ָ���ƶ� */
					mx += mdec.x;
					my += mdec.y;
					//�������ָ��߽�
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
					new_mx = mx;
					new_my = my;
					/* ���λ�� */
					// sprintf(s, "(%3d, %3d)", mx, my);
					// putfonts8_asc_sht(sht_back, 0, 0, COL_WHITE, COL_LDBLUE, s, 10);
					/* �������λ����ʾ�ᵼ�»��濨�� */
					sheet_slide(sht_mouse, mx, my); /* ����sheet_refresh */
					/* ����ǰ���ָ��ͼ�� */
					for (j = shtctl->top - 1; j > 0; j--) {
						/* ���ϵ���Ѱ�� */
						point_sht = shtctl->sheets[j];
						x = mx - point_sht->vx0;
						y = my - point_sht->vy0;
						if(0 <= x && x < point_sht->bxsize && 0 <= y && y < point_sht->bysize) {
							/* ������ָ����ͼ�㷶Χ�� */
							if (point_sht->buf[y * point_sht->bxsize + x] != point_sht->col_inv) {/* �������͸���� */
								if(point_sht->flags == SHEET_MENU) {
									/* ����ǲ˵���(������滹Ҫ�ٸ�) */
									option_change((struct MENU *)(point_sht->win), y);
								}
							}
						}

					}
					if ((mdec.btn & 0x01) != 0) {
						/* ������� */
						if (mmx < 0) {
							/* �������ͨ��ģʽ */
							/* ���մ��ϵ��µ�˳��Ѱ�������ָ��ͼ�� */
							for (j = shtctl->top - 1; j > 0; j--) {
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (sht->flags != 4 && sht->flags < 100) {
									/* �б���������ͨ����(�����ⲿAPI���ڡ������к���ͨ����) */
									if(0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
										/* ������ָ����ͼ�㷶Χ�� */
										if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {/* �������͸���� */
											sheet_updown(sht, shtctl->top - 1);
											if (sht != key_win) {
												keywin_off(key_win);
												key_win = sht;
												keywin_on(key_win);
											}
											if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
												mmx = mx;	/* ���봰���ƶ�ģʽ */
												mmy = my;
												mmx2 = sht->vx0;
												new_wy = sht->vy0;
											}
											if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {	/* �ж��Ƿ�����ָ������ */
												/* ����X��ť */
												/* �ͷŴ��ڽṹ���ڴ� */
												/***************************************************************
												*                �ڵ��[x]�ر������ʱ�ᷢ���ڴ�й¶         *
												*                       ��Ҫ��һ������ڴ��ͷ�                 *
												***************************************************************/
												memman_free_4k(memman, (unsigned int)(sht->win), sizeof (struct WINDOW));
												if(sht->flags == SHEET_DEBUG_CONS) {
													/* DEBUG���ڲ����� */
													break;
												}
												if (/*sht->flags == SHEET_USE || */sht->flags == SHEET_APIWIN) {		/* �Ƿ�ΪӦ�ó��򴰿� */
													task = sht->task;
													cons_putstr0(task->cons, "\nBreak(mouse) :\n");
													io_cli();	/* ��ֹ��ǿ�ƽ�������ʱ�л����� */
													task->tss.eax = (int) &(task->tss.esp0);
													task->tss.eip = (int) asm_end_app;
													io_sti();
													task_run(task, -1, 0);
												} else {	/* �����д��� */
													task = sht->task;
													sheet_updown(sht, -1); /* ����ͼ�� */
													keywin_off(key_win);
													key_win = shtctl->sheets[shtctl->top - 1];
													keywin_on(key_win);
													io_cli();
													fifo32_put(&task->fifo, 4);
													io_sti();
												}
											}
											break;
										}
									}
								} else if (sht->flags == SHEET_TASKBAR) {
									/* task_bar */
									if(0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {/* ������ָ����ͼ�㷶Χ�� */
										debug_print("taskbar> sht->subctl->top = %d\n",sht->subctl->top);
										for(j = sht->subctl->top; j > -1; j--) {
											/* ���ϵ���Ѱ����ͼ��(��ײ���0) */
											subsht = sht->subctl->sheets[j];
											subx = x - subsht->vx0;				/* ָ������ͼ��λ�� */
											suby = y - subsht->vy0;
											if(0 <= subx && subx < subsht->bxsize && 0 <= suby && suby < subsht->bysize) {
												/* ������ָ������ͼ���� */
												if(subsht->flags == SHEET_BUTTON) {
													/* ����ǰ�ť */
													// if(((struct BUTTON *)subsht->win)->click_old == 1) {
														// change_button((struct BUTTON *)subsht->win, 0);
													// } else {
														// change_button((struct BUTTON *)subsht->win, 1);
													// }
													click_button((struct BUTTON *)(subsht->win));
													break;
												}
											}
										}
										// if (3 <= x && x < 75 && sht->bysize - 23 <= y && y < sht->bysize - 4) {
											// /* ���[��ʼ]��ť */
											// switch(start_menu->flags) {
												// case 0:show_menu(shtctl, memman, start_menu);break;
												// case 1:hide_menu(memman, start_menu);break;
											// }
										// }
										break;
									}
								} else if (sht->flags == SHEET_MENU) {
									/* �˵�������¼� */
									if(0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {/* ������ָ����ͼ�㷶Χ�� */
										menu_click((struct MENU *)(sht->win), y);
										break;
									}
								}
							}
						} else {
							/* ������ڴ����ƶ�ģʽ */
							x = mx - mmx;	/* ���������ƶ����� */
							y = my - mmy;
							new_wx = (mmx2 + x + 2) & ~3;
							new_wy = new_wy + y;
							mmy = my;	/* ����Ϊ�ƶ�������� */
						}
					} else if ((mdec.btn & 0x02) != 0) {
						/* �����Ҽ� */
						
					} else {
						/* û�а����κΰ��� */
						mmx = -1;	/* ����ͨ��ģʽ */
						if (new_wx != 0x7fffffff) {
							sheet_slide(sht, new_wx, new_wy);	/* �̶�ͼ��λ�� */
							new_wx = 0x7fffffff;
						}
					}
				}
			} else if (768 <= i && i <= 1023) {	/* �����д��ڹرմ��� */
				/* �ͷŴ��ڽṹ���ڴ� */
				memman_free_4k(memman, (unsigned int)((shtctl->sheets0 + (i - 768))->win), sizeof (struct WINDOW));
				close_console(shtctl->sheets0 + (i - 768));
			} else if (1024 <= i && i <= 2023) {
				close_constask(taskctl->tasks0 + (i - 1024));
			} else if (2024 <= i && i <= 2279) {	/* ֻ�ر������д��� */
				sht2 = shtctl->sheets0 + (i - 2024);
				/* �ͷŴ��ڽṹ���ڴ� */
				memman_free_4k(memman, (unsigned int)(sht2->win), sizeof (struct WINDOW));
				memman_free_4k(memman, (int) sht2->buf, 256 * 165);
				sheet_free(sht2);
			}
		}
	}
}
