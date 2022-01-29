/* �ڴ������ */
/* �ڴ����ԭ����Բο���ϵͳ֧���ĵ��� */

#include <bootpack.h>

#define EFLAGS_AC_BIT		0x00040000				// ת���ɶ������� 0000 0000 0000 0100 0000 0000 0000 0000��Ϊ�˲���AC��־λ
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end){
	/* �ڴ���� */
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* ȷ��CPU��386����486���ϵ� */
	/*
	*   ȷ��CPU��ԭ���ǣ�386CPU��û�л��棬��˿���ֱ�ӽ����ڴ����
	* ��486(������)CPU���ڲ����ٻ��棬�����ڽ��в���ǰ��Ҫ�ȹرո��߻���
	*/
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg); // ��ACλΪ1
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { 
		/* 
		* 486���ϵ�CPU��18λ��AC��־λ��386(��һ��)��eflag�Ĵ���ֻ��16λ
		* ���������386����ʹ�趨AC=1��AC��ֵ���ǻ��Զ��ص�0
		*/
		flg486 = 1;		// ��486(������)CPU
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		/* �����486(������)CPU����Ҫ��������û��� */
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* ��ֹ���� */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);	// �ڴ����

	if (flg486 != 0) {
		/* �������û��� */
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
		/* �������������ڴ����ͳ�ƿ����ڴ� */
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size){
	/*�����ڴ�*/
	unsigned int i, a;
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) 0x10000);
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* �ҵ��㹻����ڴ� */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* �����free[i]���ÿռ�Ϊ0�����һ��������Ϣ */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* ��ɾ���ռ�Ϊ0���ڴ���������ǰλ�� */
				}
			}
			sysinfo->free_mem = memman_total(man);
			/* ����һ�ο��ڴ�λ�� */
			return a;
		}
	}
	return 0; /* û�п��ÿռ� */
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size) {
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
			/* ���ǰһ�����ָ���ַ���Ͽ��ÿռ����������ָ���ַ */
			/* �Ϳ�����ǰ��Ŀ����ڴ���ɵ�һ�� */
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
	man->losts++;		/* �ͷ�ʧ�ܡ��ռ���ʧ����ʧ����+1 */
	man->lostsize += size;		/* ��¼��ʧ�ռ� */
	return -1; /* ʧ�� */
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size){
	// ��4K(0x1000)Ϊ��λ�����ڴ棬��ֹ�ڲ��Ϸ�����ͷź��������С�ڴ�Ƭ��
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000; /* ��������(��4KΪ��λ��һ��ȡ����ֵ)���ȼ���0xfff�ٽ������λ���0 */
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
	// ��4K(0x1000)Ϊ��λ�ͷ��ڴ�
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
