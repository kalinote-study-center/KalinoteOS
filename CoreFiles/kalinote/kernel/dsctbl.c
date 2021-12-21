 /*此文件负责GDT,IDT等的处理*/

#include "../bootpack.h"

void init_gdtidt(void) {
	/*初始化GDT和IDT*/
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;		//GDT(全局段号记录表)的地址位于0x270000~0x27ffff(8192条记录，每条记录8字节，一共是65536字节，也就是64KB)
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;		//IDT(中断记录表)的地址位于0x26f800~0x26ffff(255条记录，每条记录8字节，一共2040字节，大约2KB)
	int i;

	/* GDT初始化 */
	for (i = 0; i <= LIMIT_GDT / 8; i++) {
		/* 对LIMIT_GDT / 8(8192)个GDT表进行初始化，将每个表的上限(limit，段的字节数-1)、基址(base)、访问权限都设为0 */
		set_segmdesc(gdt + i, 0, 0, 0);				/* 这里的gdt是指针，由于C语言特性，地址在进行加法时内部有隐含乘法，所以i每次+1，实际地址是+8(GDT结构体8字节) */
	}
	set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW_R0);		/* 段号为1的段，表示CPU管理内存大小，设置0xffffffff是4GB(32位最大识别内存)，地址为0 */
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER_R0);		/* 段号为2的段，包括了整个bootpack.kal，大小为512KB，这个段是系统专用 */
	load_gdtr(LIMIT_GDT, ADR_GDT);											/* 对GDTR寄存器进行设置 */

	/* IDT初始化 */
	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		/* 将IDT初始化，全部置0，这里跟上面GDT的初始化操作一样 */
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADR_IDT);											/* 对IDTR寄存器进行设置 */
	
	/* IDT的设定(中断注册) */
	/* 这里的2<<3表示中断段号为2，左移3位是因为最低3位有其他作用 */
	/* 注册方法：set_gatedesc(idt + 中断号, 偏移量(offset，中断程序标签), 中断段号(固定2 << 3), 权限(系统中断一般为AR_INTGATE32，应用程序需要加上0x60(01100000))); */
	set_gatedesc(idt + 0x00, (int) asm_inthandler00, 2 << 3, AR_INTGATE32);						/* 除0异常中断 */
	set_gatedesc(idt + 0x07, (int) asm_inthandler07, 2 << 3, AR_INTGATE32);						/* FPU异常中断 */
	set_gatedesc(idt + 0x0c, (int) asm_inthandler0c, 2 << 3, AR_INTGATE32);						/* 栈异常中断 */
	set_gatedesc(idt + 0x0d, (int) asm_inthandler0d, 2 << 3, AR_INTGATE32);						/* 一般保护异常中断 */
	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 2 << 3, AR_INTGATE32);						/* 时钟中断 */
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 << 3, AR_INTGATE32);						/* 键盘中断 */
	set_gatedesc(idt + 0x26, (int) asm_inthandler26, 2 << 3, AR_INTGATE32);						/* FDC中断 */
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 << 3, AR_INTGATE32);						/* 没用的中断，仅为了避免老硬件发生错误 */
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 << 3, AR_INTGATE32);						/* 鼠标中断 */
	set_gatedesc(idt + 0x40, (int) asm_kal_api,      2 << 3, AR_INTGATE32 + 0x60);				/* 应用程序API中断，段定义加上0x60(01100000)可以将该段权限设置为应用程序使用 */

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
	/* 设置段描述符(Segement Discriptor,全局段号记录表,GDT) */
	/* 有关SEGMENT_DESCRIPTOR的定义和结构，具体信息可以看bootpack.h中结构体定义处相关描述 */
	if (limit > 0xfffff) {
		/* 如果limit大于1M */
		/* 在这里设置G_bit = 1后，limt的单位就从byte变成page(页，4字节)了 */
		/* 所以可以管理的上限从1MB变成1MB * 4KB = 4GB */
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;									// 段界限低16位
	sd->base_low     = base & 0xffff;									// 段基址低16位
	sd->base_mid     = (base >> 16) & 0xff;								// 段基址中8位
	sd->access_right = ar & 0xff;										// 段权限(属性)低8位
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);		// 低4位是段界限高4位，高四位是权限(属性)高4位
	sd->base_high    = (base >> 24) & 0xff;								// 段基址高8位
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	/* 设置门描述符(Gate discriptor,中断记录表,IDT) */
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
