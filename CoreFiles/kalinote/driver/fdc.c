/* �������� */
#include "../bootpack.h"

struct FIFO32 *ffifo;		/* FDC task��FIFO */
struct FIFO32 fififo;		/* ������ FDC ���е���� FIFO */
int ffbuf[32], fibuf[32];
struct FDC fdc;

void fdcstruct_init(void) {
	/* FDC�ṹ���ʼ�� */
	fdc.mode = 0;
	fdc.cyl = 0;
	fdc.head = 0;
	fdc.sect = 0;
	fdc.sects = 0;
	fdc.st0 = 0;
	return;
}

void fdc_init(void) {
	fdcstruct_init();
	
	/* DMA��ʼ�� */
	io_out8(0x00d6, 0xc0);	/* ����ch0���ڼ���ģʽ */
	io_out8(0x00c0, 0x00);	/* ������豸 DMA */
	io_out8(0x000a, 0x06);	/* ����master��ch2��DMA */
	return;
}

void fdc_initwait(int wait) {
	for(;;) {
		if((io_in8(0x03f4) & wait) == 0) {
			break;
		}
	}
	return;
}

void fdc_sendcmd(int data) {
	for(;;) {
		if((io_in8(0x03f4) & 0xc0) == 0x80) {
			io_out8(0x03f5, data);
			return;
		}
	}
}

void fdc_setdma(void) {
	int addr = ADR_DISKIMG + (fdc.cyl * 36 + fdc.head * 18 + fdc.sect - 1) * 512;

	if(fdc.mode == 1) {
		io_out8(0x000b, 0x06);	/* ���󣬵�ַ���ӷ���д���ڴ棬ch2 */
	} else if(fdc.mode == 2) {
		io_out8(0x000b, 0x0a);	/* ���󣬵�ַ���ӷ��򣬶�ȡ�ڴ棬ch2 */
	}

	io_out8(0x0005, 0xff);	/* �ֽ����� */
	io_out8(0x0005, fdc.sects * 2 - 1);

	io_out8(0x0004, addr & 0xff);	/* �ڴ��ַ���� */
	io_out8(0x0004, (addr >> 8) & 0xff);
	io_out8(0x0081, (addr >> 16) & 0xff);

	io_out8(0x000a, 0x02);	/* ȡ��master��ch2���� */
	return;
}

void fdc_sethead(void) {
	fdc_initwait(0x11);
	fdc_sendcmd(0x0f);			/* Ѱ�� */
	fdc_sendcmd(fdc.head << 2);	/* head */
	fdc_sendcmd(fdc.cyl);		/* �� */
	return;
}

void fdc_setcmd(void) {
	fdc_initwait(0x11);

	if(fdc.mode == 1) {	/* ģʽ */
		fdc_sendcmd(0xe6);	/* ��ȡ */
	} else if(fdc.mode == 2) {
		fdc_sendcmd(0xc5);	/* д�� */
	}

	fdc_sendcmd(fdc.head << 2);	/* ָ��FD��ַ */
	fdc_sendcmd(fdc.cyl);
	fdc_sendcmd(fdc.head);
	fdc_sendcmd(fdc.sect);

	fdc_sendcmd(0x02);	/* �������ȣ�512B */
	if(fdc.mode == 1) {
		fdc_sendcmd(0x12);	/* �������18 */
		fdc_sendcmd(0x01);	/* GAP3 */
	} else if(fdc.mode == 2) {
		fdc_sendcmd(0x7f);	/* ? */
		fdc_sendcmd(0x12);	/* ? */
	}
	fdc_sendcmd(0xff);	/* ����������������С */
	return;
}

char fdc_getrstatsub(void) {
	char i;

	for(;;) {
		if((io_in8(0x03f4) & 0xc0) == 0xc0) {
			break;
		}
	}
	i = (char) io_in8(0x03f5);
	return i;
}

void fdc_getint(void) {
	fdc_initwait(0x10);
	fdc_sendcmd(0x08);	/* ��ȡ�ж�״̬ */
	fdc.st0 = fdc_getrstatsub();
	fdc_getrstatsub();	/* ��ĩβ������ */
	return;
}

void fdc_getrstat(void) {
	fdc.st0 = fdc_getrstatsub();
	fdc_getrstatsub();	/* st1 */
	fdc_getrstatsub();	/* st2 */
	fdc_getrstatsub();	/* ��ĩβ������ */
	fdc_getrstatsub();	/* headĩβ������ */
	fdc_getrstatsub();	/* ����ĩβ������ */
	fdc_getrstatsub();	/* ������С������ */
	return;
}

int fdc_rdwri(void) {
	struct TASK *task = task_now();
	struct TIMER *timer = 0;
	int err = 0, ret = 0, ph = 0, i;

	fdc_setdma();
	if(fdc.mot == 1) {
		fdc_sethead();
	} else {	/* �ȴ����������ת���ȶ� */
		io_out8(0x03f2, 0x1c);	/* �������  */

		timer = timer_alloc();
		timer_init(timer, &fififo, 1);
		timer_settime(timer, 300);
	}

	for(;;) {
		io_cli();
		if(fifo32_status(&fififo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&fififo);
			io_sti();
			if(i == 1) {
				fdc_sethead();
			} else if(i == 6) {
				if(ph == 0) {	/* ������� */
					fdc_getint();
					if((fdc.st0 & 0xc0) == 0x00) {	/* �������� */
						fdc_setcmd();
						ph = 1;
					} else {	/* �쳣״̬ */
						ret = 2;
						goto end;
					}
				} else {	/* ������� */
					fdc_getrstat();
					if((fdc.st0 & 0xc0) == 0x00) {		/* �������� */
						ret = 0;
						goto end;
					} else if((fdc.st0 & 0xc0) == 0x40) {	/* �쳣���� */
						if(err > 5) {
							ret = 1;
							goto end;
						} else {
							err++;
							fdc_setcmd();
						}
					} else {	/* �쳣״̬ */
						ret = 2;
						goto end;
					}
				}
			}
		}
	}

	/* ��������� */
end:
	io_out8(0x000a, 0x06);	/* master ������ ch2 */

	if(fdc.mot != 1) {
		timer_free(timer);
		fdc.mot = 1;
	}
	return ret;
}

/* ���������⵽ FDC��
  * ʹ��cli-sti �����������ݵ�ffifo �ᷢ���ض����ݵ�ָ����(fifo *)
  * Ҫ������λ��������DISKIMG_ADDR �Ĵ���ӳ�������λ�á� 
  * ��ȡ��[1 << 12 | ������] [������] [fifo *]
  * д��[2 << 12 | ������] [������] [fifo *]
  * ����ֵ��0��������ֹ
  * 1���쳣��ֹ
  * 2: �ԷǷ�ָ����ֹ
  * ����������ͨ�� ����*36+��ͷ*18+(sector-1) �����㡣 
*/ 

void fdc_task(void) {
	struct TASK *task = task_now();
	struct FIFO32 *f;
	struct TIMER *timer;	/* ������ƶ�ʱ�� */
	int ret, i, j;

	fifo32_init(&fififo, 32, fibuf, task);

	fdc_init();
	io_out8(0x03f2, 0x0c);	/* ֹͣ��� */
	fdc.mot = 0;

	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);

	for(;;) {
		io_cli();
		if(fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if(i == 1) {	/* ����ֹͣ��� */
				fdc.mot = 0;
				io_out8(0x03f2, 0x0c);
			} else {	/* ��ȡ��ָ�� */
				timer_cancel(timer);

				/* �洢�� fdc �ṹ�� */
				fdc.mode = (char) ((i >> 12) & 0x03);

				j = (short) (i & 0x0fff);
				fdc.cyl = (char) (j / 36);
				fdc.head = (char) (j % 36 / 18);
				fdc.sect = (char) (j % 36 % 18 + 1);
				fdc.sects = fifo32_get(&task->fifo);

				f = (struct FIFO32 *) fifo32_get(&task->fifo);

				if(fdc.mode == 1 || fdc.mode == 2) {
					ret = fdc_rdwri();
					fifo32_put(f, ret);
				} else {	/* �ԷǷ�ָ����� */
					fifo32_put(f, 2);
				}

				/* �´γ�ʼ�� */
				fdcstruct_init();
				timer_settime(timer, 300);
			}
		}
	}
}

void inthandler26(int *esp) {
	/* �����жϳ��� */
	io_in8(0x03f4);
	io_out8(PIC0_OCW2, 0x66);	/* ��� IRQ-06 ��֪ͨ */
	fifo32_put(&fififo, 6);
	return;
}
