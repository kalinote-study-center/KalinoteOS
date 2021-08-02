/* IDE硬盘驱动程序 */
#include <stdio.h>
#include "../bootpack.h"

// void init_ide_hd(struct MEMMAN *memman) {
	// /* 初始化IDE硬盘 */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	// struct IDE_HARD_DISK *hda = (struct IDE_HARD_DISK *) HDA_ADDR;
	// struct IDE_HARD_DISK *hdb = (struct IDE_HARD_DISK *) HDB_ADDR;
	
	// hd->disk_num = *((unsigned char *) IDE_HD_NUM_ADDR);			/* 硬盘数量信息 */
	// hd->disk_interrupt = 0;										/* 中断置0(?) */
	// hd->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);
	
	// hda->drive = 0;					/* 主盘 */
	// hdb->drive = 1;					/* 从盘 */
	// hda->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);			/* 主盘缓冲区 */
	// hdb->buf = (unsigned int *) memman_alloc_4k(memman, SECTOR_SIZE*2);			/* 从盘缓冲区 */
	// return;
// }

void inthandler2e(int *esp){
	/* 硬盘中断程序 */
	// struct IDE_DISK_DRIVER *hd = (struct IDE_DISK_DRIVER *) IDE_HD_DRIVER_ADDR;
	// io_out8(PIC0_OCW2, 0x74);	/* 通知PIC：IRQ-01受理完毕(0x60+IRQ编号)，将0x60+IRQ号码输出给OCW2以通知PIC收到IQR的中断通知，然后PIC继续监视IRQ1中断 */
	// hd->disk_interrupt = 1;
	// hd->reg_status = io_in8(IDE1_STATUS);
	return;
}

// void wait_hd_status(unsigned char reg, unsigned char status) {
	// /* 等待硬盘状态 */
	// int i = STATUS_WAIT_TIMEOUT;	
	// 等待寄存器返回正确的状态
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
	// /* 等待硬盘中断 */
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
	// /* 获取硬盘信息 */
	// io_out8(IDE1_DEVICE, MAKE_DEVICE_REG(0, disk->drive, 0));
	// wait_hd_status(STATUS_READY, STATUS_READY);

	// io_out8(REG_DEV_CTRL, 0x08);
	// io_out8(REG_CMD, ATA_IDENTIFY);
	// wait_hd_status(STATUS_DRQ, STATUS_DRQ);
	
	// wait_hd_interrupt();	//等待硬盘中断
	
	// port_read(IDE1_DATA, disk->buf, SECTOR_SIZE);	//读取数据
	
	// print_identify_info(cons, disk, (unsigned short *)disk->buf);		//分析并显示出来
	// return;
// }

// void print_identify_info(struct CONSOLE *cons, struct IDE_HARD_DISK *disk, unsigned short *hdinfo){
	// /* 显示获取的硬盘信息 */
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
	/* 读扇区 */
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
			// /* 读取失败 */
			// debug_print("ide> read err");
			// return;
		// }
	// } while ((status & 0x8) != 0x8);	/* 为0x8时数据准备好 */
	
	// for(x = 0; x < counts*SECTOR_SIZE; x+=2){
		// /* 一次读取两个字节 */
		// inChar = io_in16(IDE1_DATA);
		// first = (inChar << 8) >> 8;		/* 第一个字节 */
		// debug_print("ide read> %d:1st:%c\n",x,first);
		// second = inChar >> 8;		/* 第二个字节 */
		// debug_print("ide read> %d:2nd:%c\n",x+1,second);
		// buf[x] = first;
		// buf[x + 1] = second;
	// }
	
	// return;
}

void hd_write_sectors(int lba, char *buf, int counts,int hd_num) {
	
}

// void hd_read_sectors(int lba, void *buf, int counts, int hd_num){
	// /* 读扇区 */
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
	// /* 写扇区 */
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
