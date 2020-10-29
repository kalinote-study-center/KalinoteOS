/*����ʱ��������*/
#include <stdio.h>
#include "bootpack.h"

#define KEYCMD_LED		0xed

void keywin_off(struct SHEET *key_win);														//���ƴ��ڱ�������ɫ�͹�꼤��״̬
void keywin_on(struct SHEET *key_win);														//���ƴ��ڱ�������ɫ�͹�꼤��״̬
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal);					//�½������д���

void KaliMain(void){
	/* ϵͳ��� */
	
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct FIFO32 fifo, keycmd;
	int fifobuf[128], keycmd_buf[32];
	int mx, my, i, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned char *buf_back, buf_mouse[256];
	struct SHEET *sht_back, *sht_mouse;
	struct TASK *task_a, *task;
	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int j, x, y, mmx = -1, mmy = -1, mmx2 = 0;
	struct SHEET *sht = 0, *key_win;
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

	init_gdtidt();													// ��ʼ��GDT��IDT
	init_pic();														// ��ʼ���жϿ�����
	io_sti(); 														// IDT/PIC��ʼ�����������CPU���жϽ�ֹ
	fifo32_init(&fifo, 128, fifobuf, 0);							// ��ʼ��FIFO������
	init_pit();														// ��ʼ����ʱ��
	init_keyboard(&fifo, 256);										// ��ʼ������FIFO������
	enable_mouse(&fifo, 512, &mdec);								// ��ʼ�����FIFO������
	io_out8(PIC0_IMR, 0xf8); 										// ����PIT��PIC1�ͼ���(11111000)
	io_out8(PIC1_IMR, 0xef); 										// �������(11101111)
	fifo32_init(&keycmd, 32, keycmd_buf, 0);
	
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);
	
	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;

	/* sht_back */
	sht_back  = sheet_alloc(shtctl);
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);

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
	
	/* Ϊ�˱���ͼ��̵�ǰ״̬���ڳ�ͻ����һ��ʼ�Ƚ������� */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);
	
	for(;;){
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
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win->flags == 0) {	/* ���봰�ڱ��ر� */
				key_win = shtctl->sheets[shtctl->top - 1];
				keywin_on(key_win);
			}
			if (256 <= i && i <= 511) { /* �������� */
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
				if (s[0] != 0) { /* һ���ַ���BackSpace��Enter */
					fifo32_put(&key_win->task->fifo, s[0] + 256);
				}
				if (i == 256 + 0x0f) {	/* Tab */
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
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
				if (i == 256 + 0x3b && key_shift != 0) {
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {	/* Shift+F1 ǿ��ֹͣӦ�ó��� */
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli();	/* ǿ�ƽ�������ʱ��ֹ�����л� */
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
					}
				}
				if (i == 256 + 0x3c && key_shift != 0 && sht_cons[1] == 0) {	/* Shift+F2 ���µ������ */
					/* �Զ������뽹���л����´򿪵������д��� */
					keywin_off(key_win);
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				}
				if (i == 256 + 0x57 && shtctl->top > 2) {	/* F11 �л����� */
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
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
					sprintf(s, "(%3d, %3d)", mx, my);
					putfonts8_asc_sht(sht_back, 0, 0, COL_WHITE, COL_LDBLUE, s, 10);
					sheet_slide(sht_mouse, mx, my); /* ����sheet_refresh */
					if ((mdec.btn & 0x01) != 0) {
						/* ������� */
						if (mmx < 0) {
							/* �������ͨ��ģʽ */
							/* ���մ��ϵ��µ�˳��Ѱ�������ָ��ͼ�� */
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
											mmx = mx;	/* ���봰���ƶ�ģʽ */
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
										}
										if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
											/* ����X��ť */
											if ((sht->flags & 0x10) != 0) {	/* �ô����Ƿ�ΪӦ�ó��򴰿� */
												task = sht->task;
												cons_putstr0(task->cons, "\nBreak(mouse) :\n");
												io_cli();	/* ǿ�ƽ��������н�ֹ�л����� */
												task->tss.eax = (int) &(task->tss.esp0);
												task->tss.eip = (int) asm_end_app;
												io_sti();
											}
										}
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
					} else {
						/* û�а������ */
						mmx = -1;	/* ����ͨ��ģʽ */
						if (new_wx != 0x7fffffff) {
							sheet_slide(sht, new_wx, new_wy);	/* �̶�ͼ��λ�� */
							new_wx = 0x7fffffff;
						}
					}
				}
			}
		}
	}
}

void keywin_off(struct SHEET *key_win)
{
	change_wtitle8(key_win, 0);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 3); /* �����д��ڹ��OFF */
	}
	return;
}

void keywin_on(struct SHEET *key_win)
{
	change_wtitle8(key_win, 1);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 2); /* �����д��ڹ��ON */
	}
	return;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	sheet_setbuf(sht, buf, 256, 165, -1); /* ��͸��ɫ */
	make_window8(buf, 256, 165, "console", 0);
	make_textbox8(sht, 8, 28, 240, 128, COL_BLACK);
	task->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
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
	sht->task = task;
	sht->flags |= 0x20;	/* ���`���뤢�� */
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return sht;
}
