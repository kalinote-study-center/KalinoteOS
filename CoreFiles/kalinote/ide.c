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
		
		/* 这里需要注册中断 */
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
	*   这里原来(操作系统真象)使用的是outsw，可以指定读出长度，但是io_out暂 *
	* 时没有这个功能，所以后面可能还需要进一步的修改，下同。                *
	************************************************************************/
	buf = (void*)io_in8(reg_data(hd->my_channel));			// 读出来的数据在地址为buf的内存里
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


char busy_wait(struct disk* hd) {
	/* 等待30秒 */
	struct ide_channel* channel = hd->my_channel;
	unsigned short time_limit = 30 * 1000;	     // 可以等待30000毫秒
	while (time_limit -= 10 >= 0) {
		if (!(io_in8(reg_status(channel)) & BIT_STAT_BSY)) {
			return (io_in8(reg_status(channel)) & BIT_STAT_DRQ);
		} else {
			//mtime_sleep(10);		     // 睡眠10毫秒(暂未实现)
		}
	}
	return 0;
}

void ide_read(struct disk* hd, unsigned int lba, void* buf, unsigned int sec_cnt) {
	/* 从硬盘读取sec_cnt个扇区到buf */
	unsigned int secs_op, secs_done = 0;  		// secs_op:每次操作的扇区数，secs_done:已完成扇区数
	char error[64];

	lock_acquire (&hd->my_channel->lock);		// 这一句暂时没有功能
	
/* 1. 先选择操作的硬盘 */
	select_disk(hd);

	while(secs_done < sec_cnt) {
		if ((secs_done + 256) <= sec_cnt) {
			secs_op = 256;
		} else {
			secs_op = sec_cnt - secs_done;
		}
		
/* 2. 写入待读入的扇区数和起始扇区号 */
		select_sector(hd, lba + secs_done, secs_op);
		
/* 3. 执行的命令写入reg_cmd寄存器 */
		cmd_out(hd->my_channel, CMD_READ_SECTOR);			// 准备开始读数据
		
		/******************************   阻塞自己的时机  **********************************
		* 在硬盘已经开始工作(开始在内部读数据或写数据)后才能阻塞自己,现在硬盘已经开始忙了, *
		* 将自己阻塞,等待硬盘完成读操作后通过中断处理程序唤醒自己                          *
		***********************************************************************************/
		sema_down(&hd->my_channel->disk_done);				// 这一句暂时没有功能
		
/* 4. 检测硬盘状态是否可读 */
		/* 醒来后开始执行下面代码*/
		if(!busy_wait(hd)) {								// busy_wait暂时还没有实现
			/* 如果失败 */
			sprintf(error, "%s read sector %d failed!!!!!!\n", hd->name, lba);
		}

/* 5. 把数据从硬盘的缓冲区中读出 */
		read_from_sector(hd, (void*)((unsigned int)buf + secs_done * 512), secs_op);
		secs_done += secs_op;
	}
	/* 醒来后开始释放锁*/
	lock_release(&hd->my_channel->lock);
}

void ide_write(struct disk* hd, unsigned int lba, void* buf, unsigned int sec_cnt) {
	/* 将buf中sec_cnt扇区数据写入硬盘 */
	unsigned int secs_op, secs_done = 0;  		// secs_op:每次操作的扇区数，secs_done:已完成扇区数
	char error[64];
	
	lock_acquire (&hd->my_channel->lock);			// 这一句暂时没有功能
	
/* 1. 先选择操作的硬盘 */
	select_disk(hd);
	
	while(secs_done < sec_cnt) {
		if ((secs_done + 256) <= sec_cnt) {
			secs_op = 256;
		} else {
			secs_op = sec_cnt - secs_done;
		}
		
/* 2. 写入待读入的扇区数和起始扇区号 */
		select_sector(hd, lba + secs_done, secs_op);
		
/* 3. 执行的命令写入reg_cmd寄存器 */
		cmd_out(hd->my_channel, CMD_WRITE_SECTOR);			// 准备开始写数据
		
/* 4. 检测硬盘状态是否可读 */
		/* 醒来后开始执行下面代码*/
		if(!busy_wait(hd)) {								// busy_wait暂时还没有实现
			/* 如果失败 */
			sprintf(error, "%s write sector %d failed!!!!!!\n", hd->name, lba);
		}
/* 5 将数据写入硬盘 */
		write2sector(hd, (void*)((unsigned int)buf + secs_done * 512), secs_op);

		/* 在硬盘响应期间阻塞自己 */
		sema_down(&hd->my_channel->disk_done);
		secs_done += secs_op;
	}
	/* 醒来后开始释放锁*/
	lock_release(&hd->my_channel->lock);
}

void inthandler(int *esp){
	/* 硬盘中断处理程序 */
}
