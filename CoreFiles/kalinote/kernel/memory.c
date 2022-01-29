/* 内存管理部分 */
/* 内存管理原理可以参考《系统支持文档》 */

#include <bootpack.h>

#define EFLAGS_AC_BIT		0x00040000				// 转换成二进制是 0000 0000 0000 0100 0000 0000 0000 0000，为了测试AC标志位
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end){
	/* 内存测试 */
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 确认CPU是386还是486以上的 */
	/*
	*   确认CPU的原因是，386CPU内没有缓存，因此可以直接进行内存测试
	* 而486(及以上)CPU有内部高速缓存，所以在进行测试前需要先关闭告诉缓存
	*/
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg); // 置AC位为1
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { 
		/* 
		* 486以上的CPU第18位是AC标志位，386(及一下)的eflag寄存器只有16位
		* 所以如果是386，即使设定AC=1，AC的值还是会自动回到0
		*/
		flg486 = 1;		// 是486(及以上)CPU
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		/* 如果是486(及以上)CPU，需要在这里禁用缓存 */
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* 禁止缓存 */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);	// 内存测试

	if (flg486 != 0) {
		/* 重新启用缓存 */
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* 允许缓存 */
		store_cr0(cr0);
	}

	return i;
}

void memman_init(struct MEMMAN *man){
	man->frees = 0;			/* 可用信息数目 */
	man->maxfrees = 0;		/* 用于观察可用状况：frees的最大值 */
	man->lostsize = 0;		/* 释放失败时的内存大小总和 */
	man->losts = 0;			/* 释放失败次数 */
	return;
}

unsigned int memman_total(struct MEMMAN *man){
	/* 报告空余内存大小的合计 */
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		/* 遍历搜索可用内存表，并统计可用内存 */
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size){
	/*分配内存*/
	unsigned int i, a;
	struct SYSINFO *sysinfo = (struct SYSINFO *) *((int *) 0x10000);
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* 找到足够大的内存 */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* 分配后free[i]可用空间为0则减掉一条可用信息 */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* 在删除空间为0的内存表后整体向前位移 */
				}
			}
			sysinfo->free_mem = memman_total(man);
			/* 返回一段空内存位置 */
			return a;
		}
	}
	return 0; /* 没有可用空间 */
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size) {
	/* 释放内存 */
	int i, j;
	/* 为了便于归纳内存，将free[]按照addr的顺序排列 */
	/* 所以，先决定应该放在哪里 */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* 前面有可用内存 */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* 如果前一个表的指向地址加上可用空间等于这个表的指向地址 */
			/* 就可以与前面的可用内存归纳到一起 */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* 后面也有 */
				if (addr + size == man->free[i].addr) {
					/* 归纳到一起 */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]删除 */
					/* free[i]变成0后归纳到前面去*/
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* 带入结构体 */
					}
				}
			}
			return 0; /* 成功完成 */
		}
	}
	/* 不能与前面的可用空间归纳到一起 */
	if (i < man->frees) {
		/* 后面还有 */
		if (addr + size == man->free[i].addr) {
			/* 与后面归纳到一起 */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* 成功完成 */
		}
	}
	/* 前后都不能 */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]之后的，向后移动，腾出一点可用空间 */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* 更新最大值 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* 成功完成 */
	}
	/* 不能往后移动 */
	man->losts++;		/* 释放失败。空间损失，损失次数+1 */
	man->lostsize += size;		/* 记录损失空间 */
	return -1; /* 失败 */
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size){
	// 以4K(0x1000)为单位分配内存，防止在不断分配和释放后产生过多小内存片段
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000; /* 向上舍入(以4K为单位进一法取近似值)，先加上0xfff再将最后三位变成0 */
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
	// 以4K(0x1000)为单位释放内存
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
