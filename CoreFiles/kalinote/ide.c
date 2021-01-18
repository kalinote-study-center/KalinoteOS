/* IDEӲ���������� */
#include <stdio.h>
#include "bootpack.h"

void init_ide_hd(struct MEMMAN *memman) {
	/* ��ʼ��IDEӲ�� */
	struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	struct IDE_HARD_DISK *hda = (struct IDE_HARD_DISK *) HDA_ADDR;
	struct IDE_HARD_DISK *hdb = (struct IDE_HARD_DISK *) HDB_ADDR;
	
	hd->disk_num = *((unsigned char *) IDE_HD_NUM_ADDR);			/* Ӳ��������Ϣ */
	hd->disk_interrupt = 0;										/* �ж���0(?) */
	hd->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);
	
	hda->drive = 0;					/* ���� */
	hdb->drive = 1;					/* ���� */
	hda->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);			/* ���̻����� */
	hdb->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);			/* ���̻����� */
	return;
}

void inthandler2e(int *esp){
	/* Ӳ���жϳ��� */
	struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	io_out8(PIC0_OCW2, 0x74);	/* ֪ͨPIC��IRQ-01�������(0x60+IRQ���)����0x60+IRQ���������OCW2��֪ͨPIC�յ�IQR���ж�֪ͨ��Ȼ��PIC��������IRQ1�ж� */
	hd->disk_interrupt = 1;
	hd->reg_status = io_in8(IDE0_STATUS);
	return;
}

void wait_hd_status(unsigned char reg, unsigned char status) {
	/* �ȴ�Ӳ��״̬ */
	int i = STATUS_WAIT_TIMEOUT;	
	//�ȴ��Ĵ���������ȷ��״̬
	while(i-- > 0){
		if((io_in8(IDE0_STATUS)&reg) == status){
			break;
		}
	}	
	return;
}

void wait_hd_interrupt(void) {
	/* �ȴ�Ӳ���ж� */
	struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	int i = STATUS_WAIT_TIMEOUT;
	while(i-- > 0){
		if(hd->disk_interrupt){
			hd->disk_interrupt = 0;
			break;
		}
	}
	return;
}

void hd_identify(struct CONSOLE *cons, struct IDE_HARD_DISK *disk){
	/* ��ȡӲ����Ϣ */
	io_out8(IDE0_DEVICE, MAKE_DEVICE_REG(0, disk->drive, 0));
	wait_hd_status(STATUS_READY, STATUS_READY);

	io_out8(REG_DEV_CTRL, 0x08);
	io_out8(REG_CMD, ATA_IDENTIFY);
	wait_hd_status(STATUS_DRQ, STATUS_DRQ);
	
	wait_hd_interrupt();	//�ȴ�Ӳ���ж�
	
	port_read(IDE0_DATA, disk->buf, SECTOR_SIZE);	//��ȡ����
	
	print_identify_info(cons, disk, (unsigned short *)disk->buf);		//��������ʾ����
	return;
}

void print_identify_info(struct CONSOLE *cons, struct IDE_HARD_DISK *disk, unsigned short *hdinfo){
	/* ��ʾ��ȡ��Ӳ����Ϣ */
	char s[50];
	int capabilities = hdinfo[49];
	int cmd_set_supported = hdinfo[83];
	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	sprintf(s, "LBA supported:%s\n", (capabilities & 0x0200) ? "Yes" : "No");
	cons_putstr0(cons, s);
	sprintf(s, "LBA48 supported:%s\n",((cmd_set_supported & 0x0400) ? "Yes" : "No"));
	cons_putstr0(cons, s);
	disk->sectors = sectors;
	sprintf(s, "HD size: 0x%x MB\n",sectors * SECTOR_SIZE / (1024*1024));
	cons_putstr0(cons, s);
	return;
}
