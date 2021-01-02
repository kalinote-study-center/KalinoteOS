/* IDE硬盘驱动程序 */
#include "bootpack.h"
#include <stdio.h>

unsigned char channel_cnt;						// 按硬盘数计算的通道数
struct ide_channel channels[2];					// 有两个ide通道
unsigned int ext_lba_base = 0;					// 用于记录总扩展分区的起始lba,初始为0,partition_scan时以此为标记
unsigned char p_no = 0, l_no = 0;				// 用来记录硬盘主分区和逻辑分区的下标
struct list partition_list;						// 分区队列

void ide_init(){
	/* 硬盘数据结构初始化 */
	unsigned char hd_cnt = *((unsigned char*)(0x475));			// 获取硬盘的数量
	list_init(&partition_list);
	channel_cnt = (int)hd_cnt / 2;								// 一个ide通道上有两个硬盘,根据硬盘数量反推有几个ide通道
	struct ide_channel* channel;
	unsigned char channel_no = 0;
	
	/* 处理每个通道上的硬盘 */
	while(channel_no < channel_cnt) {
		channel = &channels[channel_no];
		sprintf(channel->name, "ide%d", channel_no);
		
		switch(channel_no) {
		case 0:
			channel->port_base	 = 0x1f0;		// ide0通道的起始端口号是0x1f0
			channel->irq_no	 = 0x20 + 14;		// 从PIC上倒数第二的中断引脚,温盘,也就是ide0通道的的中断向量号，中断号为0x2e
			break;
		case 1:
			channel->port_base	 = 0x170;		// ide1通道的起始端口号是0x170
			channel->irq_no	 = 0x20 + 15;		// 从PIC上最后一个中断引脚，用来响应ide1通道上的硬盘中断，中断号为0x2f
			break;
		}
		
		channel->expecting_intr = 0;			// 未向硬盘写入指令时不期待硬盘的中断，这里的0和false一样
		lock_init(&channel->lock);
		
		/*
		* 初始化为0,目的是向硬盘控制器请求数据后,硬盘驱动sema_down此信号量会阻塞线程,
		* 直到硬盘完成后通过发中断,由中断处理程序将此信号量sema_up,唤醒线程.
		*/
		
		sema_init(&channel->disk_done, 0);
		channel_no++;							// 下一个channel
	}
	
}

void select_disk(struct disk* hd) {
	/* 选择要读写的硬盘 */
	unsigned char reg_device = BIT_DEV_MBS | BIT_DEV_LBA;
	if (hd->dev_no == 1) {	// 若是从盘就置DEV位为1
		reg_device |= BIT_DEV_DEV;
	}
	io_out8(reg_dev(hd->my_channel), reg_device);
}

void select_sector(struct disk* hd, unsigned int lba, unsigned char sec_cnt) {
	/* 向硬盘控制器写入起始扇区地址及要读写的扇区数 */
	struct ide_channel* channel = hd->my_channel;

	/* 写入要读写的扇区数*/
	io_out8(reg_sect_cnt(channel), sec_cnt);	 // 如果sec_cnt为0,则表示写入256个扇区

	/* 写入lba地址(即扇区号) */
	io_out8(reg_lba_l(channel), lba);			 // lba地址的低8位
	io_out8(reg_lba_m(channel), lba >> 8);		 // lba地址的8~15位
	io_out8(reg_lba_h(channel), lba >> 16);		 // lba地址的16~23位

	/*
	* 因为lba地址的24~27位要存储在device寄存器的0～3位,
    * 无法单独写入这4位,所以在此处把device寄存器再重新写入一次
	*/
	io_out8(reg_dev(channel), BIT_DEV_MBS | BIT_DEV_LBA | (hd->dev_no == 1 ? BIT_DEV_DEV : 0) | lba >> 24);
}

void cmd_out(struct ide_channel* channel, unsigned char cmd) {
/* 向通道channel发命令cmd */
/* 只要向硬盘发出了命令便将此标记置为true(1),硬盘中断处理程序需要根据它来判断 */
   channel->expecting_intr = 1;
   io_out8(reg_cmd(channel), cmd);
}

void read_from_sector(struct disk* hd, void* buf, unsigned char sec_cnt) {
	/* 硬盘读入sec_cnt个扇区的数据到buf */
	unsigned int size_in_byte;
	if (sec_cnt == 0) {
		/* 因为sec_cnt是8位变量,由主调函数将其赋值时,若为256则会将最高位的1丢掉变为0 */
		size_in_byte = 256 * 512;
	} else { 
		size_in_byte = sec_cnt * 512; 
	}
	/************************************************************************
	*                             这里考虑数据长度                          *
	************************************************************************/
	buf = io_in8(reg_data(hd->my_channel));			// 读出来的数据在地址为buf的内存里
}

void write2sector(struct disk* hd, void* buf, unsigned char sec_cnt) {
	/* 将buf中sec_cnt扇区的数据写入硬盘 */
	unsigned int size_in_byte;
	if (sec_cnt == 0) {
	/* 因为sec_cnt是8位变量,由主调函数将其赋值时,若为256则会将最高位的1丢掉变为0 */
		size_in_byte = 256 * 512;
	} else { 
		size_in_byte = sec_cnt * 512; 
	}
	/************************************************************************
	*                             这里考虑数据长度                          *
	************************************************************************/
	io_out8(reg_data(hd->my_channel), (int)buf);
}
