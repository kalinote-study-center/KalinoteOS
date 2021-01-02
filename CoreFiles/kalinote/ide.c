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
	unsigned char channel_no = 0, dev_no = 0;
	
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
