/*��������ж������������*/
#include <stdio.h>
#include "bootpack.h"

#define MEMMAN_FREES		4090	/* ��Լ32KB */
#define MEMMAN_ADDR			0x003c0000

struct FREEINFO {	/* ������Ϣ */
	unsigned int addr, size;
};

struct MEMMAN {		/* �ڴ���� */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);							//����CPU����
void memman_init(struct MEMMAN *man);												//��ʼ���ڴ�������
unsigned int memman_total(struct MEMMAN *man);										//��ȡʣ���ڴ��С
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);					//�����ڴ�
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);			//�ͷ��ڴ�

void KaliMain(void){
	/*������������*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//������Ϣ(BOOTINFO�ṹ��)
	char s[40], mcursor[256], keybuf[32], mousebuf[128];
	int mx, my, i;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC��ʼ�����������CPU���жϽ�ֹ */
	fifo8_init(&keyfifo, 32, keybuf);							//��ʼ������fifo������
	fifo8_init(&mousefifo, 128, mousebuf);						//��ʼ�����fifo������
	io_out8(PIC0_IMR, 0xf9); /* ����PIC1�ͼ���(11111001) */
	io_out8(PIC1_IMR, 0xef); /* �������(11101111) */

	init_keyboard();				//��ʼ������
	enable_mouse(&mdec);			//�������
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();												//��ʼ����ɫ��
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);		//��ʼ����Ļ
	/*
	* ע������� binfo->vram �ȼ���(*binfo).vram
	* ������ͬ��
	*/
	mx = (binfo->scrnx - 16) / 2; 								//���ָ��λ�ã�Ĭ��Ϊ��Ļ����
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL_LDBLUE);					//��ʼ�����ָ��
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16,
		mx, my, mcursor, 16);									//�������
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, s);
	/*
	* ע����ʱ����걳��ֻ����COL_LDBLUE�Ĵ�ɫ�������ƶ��ǻḲ�ǵ����������
	* ������������������
	*/
	
	putfonts8_asc(binfo->vram, binfo->scrnx, 236, 181, COL_DRED, "KalinoteOS");
	putfonts8_asc(binfo->vram, binfo->scrnx, 235, 180, COL_BRED, "KalinoteOS");
	/*
	* ֻҪ�Ȼ���һ�鰵ɫ���壬Ȼ���ٻ���һ����ɫ���壬Ȼ�������ַ�����1�����أ��Ϳ��������������
	* ������������ļ��ṹ����ԼӸ�������ר�Ż��������ַ���
	*/
	
	//����������ݣ�ԭ���ڵ�98ҳ
	//sprintf(s, "scrnx = %d", binfo->scrnx);
	//putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL_WHITE, s);

	sprintf(s, "memory %dMB   free : %dKB",
			memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL_WHITE, s);			//��ʾ�ڴ�
	
	for(;;){
		//ֹͣCPU
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
					/* 3�ַ����룬��ʾ���� */
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
					/* ���ָ���ƶ� */
					boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, mx, my, mx + 15, my + 15); /* ������� */
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
					boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 0, 0, 79, 15); /* �������� */
					putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, s); /* ��ʾ���� */
					putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16); /* �軭��� */
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

	/* ȷ��CPU��386����486���ϵ�(����Ӧ�ò���Ҫ�ˣ���������д��) */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* �����386����ʹ�趨AC=1��AC��ֵ���ǻ��Զ��ص�0 */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* ��ֹ���� */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* ������ */
		store_cr0(cr0);
	}

	return i;
}


void memman_init(struct MEMMAN *man){
	man->frees = 0;			/* ������Ϣ��Ŀ */
	man->maxfrees = 0;		/* ���ڹ۲����״����frees�����ֵ */
	man->lostsize = 0;		/* �ͷ�ʧ��ʱ���ڴ��С�ܺ� */
	man->losts = 0;			/* �ͷ�ʧ�ܴ��� */
	return;
}

unsigned int memman_total(struct MEMMAN *man){
	/* ��������ڴ��С�ĺϼ� */
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size){
	/*�����ڴ�*/
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* �ҵ��㹻����ڴ� */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* free[i]�����һ��������Ϣ */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* ����ṹ�� */
				}
			}
			return a;
		}
	}
	return 0; /* û�п��ÿռ� */
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size){
	/* �ͷ��ڴ� */
	int i, j;
	/* Ϊ�˱��ڹ����ڴ棬��free[]����addr��˳������ */
	/* ���ԣ��Ⱦ���Ӧ�÷������� */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* ǰ���п����ڴ� */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* ������ǰ��Ŀ����ڴ���ɵ�һ�� */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* ����Ҳ�� */
				if (addr + size == man->free[i].addr) {
					/* ���ɵ�һ�� */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]ɾ�� */
					/* free[i]���0����ɵ�ǰ��ȥ*/
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* ����ṹ�� */
					}
				}
			}
			return 0; /* �ɹ���� */
		}
	}
	/* ������ǰ��Ŀ��ÿռ���ɵ�һ�� */
	if (i < man->frees) {
		/* ���滹�� */
		if (addr + size == man->free[i].addr) {
			/* �������ɵ�һ�� */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* �ɹ���� */
		}
	}
	/* ǰ�󶼲��� */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]֮��ģ�����ƶ����ڳ�һ����ÿռ� */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* �������ֵ */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* �ɹ���� */
	}
	/* ���������ƶ� */
	man->losts++;
	man->lostsize += size;
	return -1; /* ʧ�� */
}

void HariMain(void){
	/*ϵͳ�������*/
	KaliMain();
}
