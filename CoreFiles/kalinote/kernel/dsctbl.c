 /*���ļ�����GDT,IDT�ȵĴ���*/

#include "../bootpack.h"

void init_gdtidt(void){
	/*��ʼ��GDT��IDT*/
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;		//GDT(ȫ�ֶκż�¼��)�ĵ�ַλ��0x270000~0x27ffff(8192����¼��ÿ����¼8�ֽڣ�һ����65536�ֽڣ�Ҳ����64KB)
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;		//IDT(�жϼ�¼��)�ĵ�ַλ��0x26f800~0x26ffff(255����¼��ÿ����¼8�ֽڣ�һ��2040�ֽڣ���Լ2KB)
	int i;

	/* GDT��ʼ�� */
	for (i = 0; i <= LIMIT_GDT / 8; i++) {
		/* ��LIMIT_GDT / 8(8192)��GDT�����г�ʼ������ÿ����������(limit���ε��ֽ���-1)����ַ(base)������Ȩ�޶���Ϊ0 */
		set_segmdesc(gdt + i, 0, 0, 0);				/* �����gdt��ָ�룬����C�������ԣ���ַ�ڽ��мӷ�ʱ�ڲ��������˷�������iÿ��+1��ʵ�ʵ�ַ��+8(GDT�ṹ��8�ֽ�) */
	}
	set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);			/* �κ�Ϊ1�ĶΣ���ʾCPU�����ڴ��С������0xffffffff��4GB(32λ���ʶ���ڴ�)����ַΪ0 */
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);			/* �κ�Ϊ2�ĶΣ�����������bootpack.h����СΪ512KB */
	load_gdtr(LIMIT_GDT, ADR_GDT);											/* ��GDTR�Ĵ����������� */

	/* IDT��ʼ�� */
	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		/* ��IDT��ʼ����ȫ����0�����������GDT�ĳ�ʼ������һ�� */
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADR_IDT);											/* ��IDTR�Ĵ����������� */
	
	/* IDT���趨(�ж�ע��) */
	/* �����2<<3��ʾ�ж϶κ�Ϊ2������3λ����Ϊ���3λ���������� */
	/* ע�᷽����set_gatedesc(idt + �жϺ�, ƫ����(offset���жϳ����ǩ), �ж϶κ�(�̶�2 << 3), Ȩ��(ϵͳ�ж�һ��ΪAR_INTGATE32��Ӧ�ó�����Ҫ����0x60(01100000))); */
	set_gatedesc(idt + 0x07, (int) asm_inthandler07, 2 << 3, AR_INTGATE32);						/* FPU�ж� */
	set_gatedesc(idt + 0x0c, (int) asm_inthandler0c, 2 << 3, AR_INTGATE32);						/* ջ�쳣�ж� */
	set_gatedesc(idt + 0x0d, (int) asm_inthandler0d, 2 << 3, AR_INTGATE32);						/* һ�㱣���쳣�ж� */
	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 2 << 3, AR_INTGATE32);						/* ʱ���ж� */
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 << 3, AR_INTGATE32);						/* �����ж� */
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 << 3, AR_INTGATE32);						/* û�õ��жϣ���Ϊ�˱�����Ӳ���������� */
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 << 3, AR_INTGATE32);						/* ����ж� */
	set_gatedesc(idt + 0x40, (int) asm_kal_api,      2 << 3, AR_INTGATE32 + 0x60);				/* Ӧ�ó���API�жϣ��ζ������0x60(01100000)���Խ��ö�Ȩ������ΪӦ�ó���ʹ�� */

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
	/* ���ö�������(Segement Discriptor,ȫ�ֶκż�¼��,GDT) */
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	/* ������������(Gate discriptor,�жϼ�¼��,IDT) */
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}