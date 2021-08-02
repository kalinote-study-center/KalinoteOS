/* IDEӲ���������� */
#include <stdio.h>
#include "../bootpack.h"

// void init_ide_hd(struct MEMMAN *memman) {
	// /* ��ʼ��IDEӲ�� */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	// struct IDE_HARD_DISK *hda = (struct IDE_HARD_DISK *) HDA_ADDR;
	// struct IDE_HARD_DISK *hdb = (struct IDE_HARD_DISK *) HDB_ADDR;
	
	// hd->disk_num = *((unsigned char *) IDE_HD_NUM_ADDR);			/* Ӳ��������Ϣ */
	// hd->disk_interrupt = 0;										/* �ж���0(?) */
	// hd->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);
	
	// hda->drive = 0;					/* ���� */
	// hdb->drive = 1;					/* ���� */
	// hda->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);			/* ���̻����� */
	// hdb->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);			/* ���̻����� */
	// return;
// }

void inthandler2e(int *esp){
	/* Ӳ���жϳ��� */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	// io_out8(PIC0_OCW2, 0x74);	/* ֪ͨPIC��IRQ-01�������(0x60+IRQ���)����0x60+IRQ���������OCW2��֪ͨPIC�յ�IQR���ж�֪ͨ��Ȼ��PIC��������IRQ1�ж� */
	// hd->disk_interrupt = 1;
	// hd->reg_status = io_in8(IDE1_STATUS);
	return;
}

// void wait_hd_status(unsigned char reg, unsigned char status) {
	// /* �ȴ�Ӳ��״̬ */
	// int i = STATUS_WAIT_TIMEOUT;	
	// �ȴ��Ĵ���������ȷ��״̬
	// while(i-- > 0){
		// if((io_in8(IDE1_STATUS)&reg) == status){
			// debug_print("ide> IDE1_STATUS = 0x%x\n" , io_in8(IDE1_STATUS));
			// break;
		// }
	// }	
	// debug_print("ide> IDE1_STATUS = 0x%x\n" , io_in8(IDE1_STATUS));
	// return;
// }

// void wait_hd_interrupt(void) {
	// /* �ȴ�Ӳ���ж� */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	// int i = STATUS_WAIT_TIMEOUT;
	// while(i-- > 0){
		// if(hd->disk_interrupt){
			// debug_print("ide> hd->disk_interrupt = %d\n" , hd->disk_interrupt);
			// hd->disk_interrupt = 0;
			// break;
		// }
	// }
	// return;
// }

// void hd_identify(struct CONSOLE *cons, struct IDE_HARD_DISK *disk){
	// /* ��ȡӲ����Ϣ */
	// io_out8(IDE1_DEVICE, MAKE_DEVICE_REG(0, disk->drive, 0));
	// wait_hd_status(STATUS_READY, STATUS_READY);

	// io_out8(REG_DEV_CTRL, 0x08);
	// io_out8(REG_CMD, ATA_IDENTIFY);
	// wait_hd_status(STATUS_DRQ, STATUS_DRQ);
	
	// wait_hd_interrupt();	//�ȴ�Ӳ���ж�
	
	// port_read(IDE1_DATA, disk->buf, SECTOR_SIZE);	//��ȡ����
	
	// print_identify_info(cons, disk, (unsigned short *)disk->buf);		//��������ʾ����
	// return;
// }

// void print_identify_info(struct CONSOLE *cons, struct IDE_HARD_DISK *disk, unsigned short *hdinfo){
	// /* ��ʾ��ȡ��Ӳ����Ϣ */
	// char s[50];
	// int capabilities = hdinfo[49];
	// int cmd_set_supported = hdinfo[83];
	// int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	// sprintf(s, "LBA supported:%s\n", (capabilities & 0x0200) ? "Yes" : "No");
	// cons_putstr0(cons, s);
	// sprintf(s, "LBA48 supported:%s\n",((cmd_set_supported & 0x0400) ? "Yes" : "No"));
	// cons_putstr0(cons, s);
	// disk->sectors = sectors;
	// sprintf(s, "HD size: %d MB\n",sectors * SECTOR_SIZE / (1024*1024));
	// cons_putstr0(cons, s);
	// return;
// }

void hd_read_sectors(int lba, char *buf, int counts, int hd_num) {
	/* ������ */
	// unsigned char status = 0;
	// int timeout = STATUS_WAIT_TIMEOUT;
	// unsigned int x;
	// short inChar;
	// char first,second;
	
	// io_out8(IDE1_NSECTOR, counts);
	// io_out8(IDE1_LBA_LOW, lba);
	// io_out8(IDE1_LBA_MID, lba>>8);
	// io_out8(IDE1_LBA_HIGH, (lba>>16)&0xff);
	// io_out8(IDE1_DEVICE, 0xe0 | (hd_num << 4));
	// io_out8(IDE1_STATUS, ATA_READ);
	
	// do{
		// status = io_in8(IDE1_STATUS);
		// if (timeout-- == 0) {
			// /* ��ȡʧ�� */
			// debug_print("ide> read err");
			// return;
		// }
	// } while ((status & 0x8) != 0x8);	/* Ϊ0x8ʱ����׼���� */
	
	// for(x = 0; x < counts*SECTOR_SIZE; x+=2){
		// /* һ�ζ�ȡ�����ֽ� */
		// inChar = io_in16(IDE1_DATA);
		// first = (inChar << 8) >> 8;		/* ��һ���ֽ� */
		// debug_print("ide read> %d:1st:%c\n",x,first);
		// second = inChar >> 8;		/* �ڶ����ֽ� */
		// debug_print("ide read> %d:2nd:%c\n",x+1,second);
		// buf[x] = first;
		// buf[x + 1] = second;
	// }
	
	// return;
}

void hd_write_sectors(int lba, char *buf, int counts,int hd_num) {
	
}

// void hd_read_sectors(int lba, void *buf, int counts, int hd_num){
	// /* ������ */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	// struct IDE_HARD_DISK *hda = (struct IDE_HARD_DISK *) HDA_ADDR;
	// struct IDE_HARD_DISK *hdb = (struct IDE_HARD_DISK *) HDB_ADDR;
	
	// wait_hd_status(STATUS_BUSY, STATUS_BUSY);
	
	// hd->disk_interrupt = 0;
	// io_out8(IDE1_NSECTOR, counts);
	// io_out8(IDE1_LBA_LOW, lba);
	// io_out8(IDE1_LBA_MID, lba>>8);
	// io_out8(IDE1_LBA_HIGH, lba>>16);
	// if(hd_num == 0){
		// io_out8(IDE1_DEVICE, MAKE_DEVICE_REG(1, hda->drive, (lba >> 24) & 0xF));
	// } else {
		// io_out8(IDE1_DEVICE, MAKE_DEVICE_REG(1, hdb->drive, (lba >> 24) & 0xF));
	// }
	
	// wait_hd_status(STATUS_READY, STATUS_READY);
	
	// io_out8(REG_DEV_CTRL, 0x08);
	// io_out8(REG_CMD, ATA_READ);
	
	// wait_hd_status(STATUS_DRQ, STATUS_DRQ);

	// wait_hd_interrupt();

	// port_read(IDE1_DATA, buf, counts * SECTOR_SIZE);
// }

// void hd_write_sectors(int lba, void *buf, int counts,int hd_num){
	// /* д���� */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	// struct IDE_HARD_DISK *hda = (struct IDE_HARD_DISK *) HDA_ADDR;
	// struct IDE_HARD_DISK *hdb = (struct IDE_HARD_DISK *) HDB_ADDR;
	
	// wait_hd_status(STATUS_BUSY, STATUS_BUSY);

	// hd->disk_interrupt = 0;
	// io_out8(IDE1_NSECTOR, counts);
	// io_out8(IDE1_LBA_LOW, lba);
	// io_out8(IDE1_LBA_MID, lba>>8);
	// io_out8(IDE1_LBA_HIGH, lba>>16);
	// if(hd_num == 0){
		// io_out8(IDE1_DEVICE, MAKE_DEVICE_REG(1, hda->drive, (lba >> 24) & 0xF));
	// } else {
		// io_out8(IDE1_DEVICE, MAKE_DEVICE_REG(1, hdb->drive, (lba >> 24) & 0xF));
	// }
	// wait_hd_status(STATUS_READY, STATUS_READY);

	// io_out8(REG_DEV_CTRL, 0x08);
	// io_out8(REG_CMD, ATA_WRITE);
	
	// wait_hd_status(STATUS_DRQ, STATUS_DRQ);

	// wait_hd_interrupt();
	
	// port_write(IDE1_DATA, buf, counts * SECTOR_SIZE);
// }
