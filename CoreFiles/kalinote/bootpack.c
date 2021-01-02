/*启动时的主程序*/
#include <stdio.h>
#include "bootpack.h"

#define KEYCMD_LED		0xed

void keywin_off(struct SHEET *key_win);														//控制窗口标题栏颜色和光标激活状态
void keywin_on(struct SHEET *key_win);														//控制窗口标题栏颜色和光标激活状态
//struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal);					//新建命令行窗口
void close_console(struct SHEET *sht);														//关闭命令窗口
void close_constask(struct TASK *task);														//结束任务

void KaliMain(void){
	/* 系统入口 */
	
	/*这里是主程序*/
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
	unsigned int *buf_back, buf_mouse[256];
	struct SHEET *sht_back, *sht_mouse;
	struct TASK *task_a, *task;
	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int j, x, y, mmx = -1, mmy = -1, mmx2 = 0;
	struct SHEET *sht = 0, *key_win, *sht2;
	int *fat_ch, *fat_jp;
	unsigned char *chinese, *nihongo;
	struct FILEINFO *finfo_ch;
	struct FILEINFO *finfo_jp;
	extern char fonts[4096];
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};

	/* 初始化 */
	init_gdtidt();													// 初始化GDT和IDT
	init_pic();														// 初始化中断控制器
	io_sti(); 														// IDT/PIC初始化结束，解除CPU的中断禁止
	fifo32_init(&fifo, 128, fifobuf, 0);							// 初始化FIFO缓冲区
	init_pit();														// 初始化定时器
	init_keyboard(&fifo, 256);										// 初始化键盘FIFO缓冲区
	enable_mouse(&fifo, 512, &mdec);								// 初始化鼠标FIFO缓冲区
	io_out8(PIC0_IMR, 0xf8); 										// 允许PIT、PIC1和PS/2键盘(11111000)
	io_out8(PIC1_IMR, 0xaf); 										// 允许PS/2鼠标和硬盘(10101111)
	
	fifo32_init(&keycmd, 32, keycmd_buf, 0);
	*((int *) 0x0fec) = (int) &fifo;
	
	/* 内存处理 */
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);
	
	//init_palette();												// 初始化调色板
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;
	task_a->langmode = 0;

	/* sht_back */
	sht_back  = sheet_alloc(shtctl);
	buf_back  = (unsigned int *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * 4);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	init_screen8(buf_back, binfo->scrnx, binfo->scrny, binfo->vmode);

	/* sht_taskbar */
	init_taskbar(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_cons */
	key_win = open_console(shtctl, memtotal);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;

	sheet_slide(sht_back,  0,  0);
	sheet_slide(key_win,   32, 4);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back,  0);
	sheet_updown(key_win,   1);
	sheet_updown(sht_mouse, 2);
	keywin_on(key_win);
	
	/* 为了避免和键盘当前状态存在冲突，在一开始先进行设置 */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);
	
	/* 载入HZK字库 */
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
			chinese[i] = fonts[i]; /* 没有字库，半角部分直接复制英文字库 */
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			chinese[i] = 0xff; /* 没有字库，全角部分以0xff填充 */
		}
	}
	*((int *) 0x0fe8) = (int) chinese;
	memman_free_4k(memman, (int) fat_ch, 4 * 2880);
	
	/* 载入nihongo字库 */
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
			nihongo[i] = fonts[i]; /* 没有字库，半角部分直接复制英文字库*/
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			nihongo[i] = 0xff; /* 没有字库，半角部分直接复制英文字库 */
		}
	}
	*((int *) 0x0ef9) = (int) nihongo;
	memman_free_4k(memman, (int) fat_jp, 4 * 2880);
	
	/* 时钟更新定时器 */
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 100);	/* 每秒更新一次时间 */
	
	for(;;){

		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* 如果存在向键盘控制器发送的数据，则发送它 */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			/* FIFO为空，当存在搁置的绘图操作时立即执行 */
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
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win != 0 && key_win->flags == 0) {	/* 窗口被关闭 */
				if (shtctl->top == 1) {	/* 只剩鼠标和背景时 */
					key_win = 0;
				} else {
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
				}
			}
			if (i == 1) {
				/* 更新时间 */
				/*右下角时间显示*/
				timer_init(timer, &fifo, 1);
				sprintf(s,"%02d:%02d:%02d",get_hour_hex(), get_min_hex(), get_sec_hex());
				putfonts8_asc_sht(sht_back, binfo->scrnx - 70, binfo->scrny - 20, COL_BLACK, COL_BGREY, s, 8);
				/* 日期 */
				sprintf(s,"%d/%d/%d",get_year(), get_mon_hex(), get_day_of_month());
				putfonts8_asc_sht(sht_back, binfo->scrnx - 160, binfo->scrny - 20, COL_BLACK, COL_BGREY, s, 10);
				timer_settime(timer, 100);
				sheet_refresh(sht_back, binfo->scrnx - 160, binfo->scrny - 20, binfo->scrnx - 70 + 8 * 8, binfo->scrny - 50 + 16);
			} else if (256 <= i && i <= 511) { /* 键盘数据 */
				if (i < 0x80 + 256) { /* 将按键编码转换为字符编码 */
					if (key_shift == 0) {
						s[0] = keytable0[i - 256];
					} else {
						s[0] = keytable1[i - 256];
					}
				} else {
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z') {	/* 当输入字符为英文字母时 */
					if (((key_leds & 4) == 0 && key_shift == 0) ||
							((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20;	/* 将大写字母转换为小写字母 */
					}
				}
				if (s[0] != 0 && key_win != 0) { /* 一般字符、BackSpace、Enter */
					fifo32_put(&key_win->task->fifo, s[0] + 256);
				}
				if (i == 256 + 0x0f && key_win != 0){	/* Tab */
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
				}
				if (i == 256 + 0x2a) {	/* 左SHIFT ON */
					key_shift |= 1;
				}
				if (i == 256 + 0x36) {	/* 右SHIFT ON */
					key_shift |= 2;
				}
				if (i == 256 + 0xaa) {	/* 左SHIFT OFF */
					key_shift &= ~1;
				}
				if (i == 256 + 0xb6) {	/* 右SHIFT OFF */
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
				if (i == 256 + 0x3b && key_shift != 0 && key_win != 0) {/* Shift+F1 强行停止应用程序 */
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {	
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli();	/* 强制结束处理时禁止任务切换 */
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);	/* 为了确实执行结束处理，如果处于休眠状态则唤醒 */
					}
				}
				if (i == 256 + 0x3c && key_shift != 0) {	/* Shift+F2 打开新的命令窗口 */
					/* 自动将输入焦点切换到新打开的命令行窗口 */
					if (key_win != 0) {
						keywin_off(key_win);
					}
					keywin_off(key_win);
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				}
				if (i == 256 + 0x57 && shtctl->top > 2) {	/* F11 切换窗口 */
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
				}
				if (i == 256 + 0xfa) {	/* 键盘成功接收到数据 */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {	/* 键盘没有成功接收到数据 */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			} else if (512 <= i && i <= 767) { /* 鼠标数据 */
				if (mouse_decode(&mdec, i - 512) != 0) {
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
					new_mx = mx;
					new_my = my;
					/* 鼠标位置 */
					sprintf(s, "(%3d, %3d)", mx, my);
					putfonts8_asc_sht(sht_back, 0, 0, COL_WHITE, COL_LDBLUE, s, 10);
					sheet_slide(sht_mouse, mx, my); /* 包含sheet_refresh */
					if ((mdec.btn & 0x01) != 0) {
						/* 按下左键 */
						if (mmx < 0) {
							/* 如果处于通常模式 */
							/* 按照从上到下的顺序寻找鼠标所指的图层 */
							for (j = shtctl->top - 1; j > 0; j--) {
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
									if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {
										sheet_updown(sht, shtctl->top - 1);
										if (sht != key_win) {
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
											mmx = mx;	/* 进入窗口移动模式 */
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
										}
										if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {	/* 判断是否点击在指定区域 */
											/* 按下X按钮 */
											if ((sht->flags & 0x10) != 0) {		/* 是否为应用程序窗口 */
												task = sht->task;
												cons_putstr0(task->cons, "\nBreak(mouse) :\n");
												io_cli();	/* 禁止在强制结束处理时切换任务 */
												task->tss.eax = (int) &(task->tss.esp0);
												task->tss.eip = (int) asm_end_app;
												io_sti();
												task_run(task, -1, 0);
											} else {	/* 命令行窗口 */
												task = sht->task;
												sheet_updown(sht, -1); /* 隐藏图层 */
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
							}
						} else {
							/* 如果处于窗口移动模式 */
							x = mx - mmx;	/* 计算鼠标的移动距离 */
							y = my - mmy;
							new_wx = (mmx2 + x + 2) & ~3;
							new_wy = new_wy + y;
							mmy = my;	/* 更新为移动后的坐标 */
						}
					} else {
						/* 没有按下左键 */
						mmx = -1;	/* 返回通常模式 */
						if (new_wx != 0x7fffffff) {
							sheet_slide(sht, new_wx, new_wy);	/* 固定图层位置 */
							new_wx = 0x7fffffff;
						}
					}
				}
			} else if (768 <= i && i <= 1023) {	/* 命令行窗口关闭处理 */
				close_console(shtctl->sheets0 + (i - 768));
			} else if (1024 <= i && i <= 2023) {
				close_constask(taskctl->tasks0 + (i - 1024));
			} else if (2024 <= i && i <= 2279) {	/* 只关闭命令行窗口 */
				sht2 = shtctl->sheets0 + (i - 2024);
				memman_free_4k(memman, (int) sht2->buf, 256 * 165);
				sheet_free(sht2);
			}
		}
	}
}

void keywin_off(struct SHEET *key_win){
	change_wtitle8(key_win, 0);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 3); /* 命令行窗口光标OFF */
	}
	return;
}

void keywin_on(struct SHEET *key_win){
	change_wtitle8(key_win, 1);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 2); /* 命令行窗口光标ON */
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2); /* level=2, priority=2 */
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	char icon[16][16];
	//unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 256 * 165 * 4);
	unsigned int *buf = (unsigned int *) memman_alloc_4k(memman, 525 * 479 * 4);
	//sheet_setbuf(sht, buf, 256, 165, -1); /* 无透明色 */
	sheet_setbuf(sht, buf, 525, 479, -1); /* 无透明色 */
	make_window8(buf, 525, 479, "console", 0);
	make_textbox8(sht, 3, 24, 519, 452, COL_BLACK);
	make_icon(buf, 525, icon, 1);
	sht->task = open_constask(sht, memtotal);
	sht->flags |= 0x20;	/* 有光标 */
	return sht;
}

void close_constask(struct TASK *task){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = 0; /* 用来替代task_free(task); */
	return;
}

void close_console(struct SHEET *sht){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165 * 4);
	sheet_free(sht);
	close_constask(task);
	return;
}
