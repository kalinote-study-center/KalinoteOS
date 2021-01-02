/* IDEӲ���������� */
#include "bootpack.h"
#include <stdio.h>

unsigned char channel_cnt;						// ��Ӳ���������ͨ����
struct ide_channel channels[2];					// ������ideͨ��
unsigned int ext_lba_base = 0;					// ���ڼ�¼����չ��������ʼlba,��ʼΪ0,partition_scanʱ�Դ�Ϊ���
unsigned char p_no = 0, l_no = 0;				// ������¼Ӳ�����������߼��������±�
struct list partition_list;						// ��������

void ide_init(){
	/* Ӳ�����ݽṹ��ʼ�� */
	unsigned char hd_cnt = *((unsigned char*)(0x475));			// ��ȡӲ�̵�����
	list_init(&partition_list);
	channel_cnt = (int)hd_cnt / 2;								// һ��ideͨ����������Ӳ��,����Ӳ�����������м���ideͨ��
	struct ide_channel* channel;
	unsigned char channel_no = 0, dev_no = 0;
	
	/* ����ÿ��ͨ���ϵ�Ӳ�� */
	while(channel_no < channel_cnt) {
		channel = &channels[channel_no];
		sprintf(channel->name, "ide%d", channel_no);
		
		switch(channel_no) {
		case 0:
			channel->port_base	 = 0x1f0;		// ide0ͨ������ʼ�˿ں���0x1f0
			channel->irq_no	 = 0x20 + 14;		// ��PIC�ϵ����ڶ����ж�����,����,Ҳ����ide0ͨ���ĵ��ж������ţ��жϺ�Ϊ0x2e
			break;
		case 1:
			channel->port_base	 = 0x170;		// ide1ͨ������ʼ�˿ں���0x170
			channel->irq_no	 = 0x20 + 15;		// ��PIC�����һ���ж����ţ�������Ӧide1ͨ���ϵ�Ӳ���жϣ��жϺ�Ϊ0x2f
			break;
		}
		
		channel->expecting_intr = 0;			// δ��Ӳ��д��ָ��ʱ���ڴ�Ӳ�̵��жϣ������0��falseһ��
		lock_init(&channel->lock);
		
		/*
		* ��ʼ��Ϊ0,Ŀ������Ӳ�̿������������ݺ�,Ӳ������sema_down���ź����������߳�,
		* ֱ��Ӳ����ɺ�ͨ�����ж�,���жϴ�����򽫴��ź���sema_up,�����߳�.
		*/
		
		sema_init(&channel->disk_done, 0);
		channel_no++;							// ��һ��channel
	}
	
}
