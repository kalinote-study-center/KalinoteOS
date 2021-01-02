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
	unsigned char channel_no = 0;
	
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

void select_disk(struct disk* hd) {
	/* ѡ��Ҫ��д��Ӳ�� */
	unsigned char reg_device = BIT_DEV_MBS | BIT_DEV_LBA;
	if (hd->dev_no == 1) {	// ���Ǵ��̾���DEVλΪ1
		reg_device |= BIT_DEV_DEV;
	}
	io_out8(reg_dev(hd->my_channel), reg_device);
}

void select_sector(struct disk* hd, unsigned int lba, unsigned char sec_cnt) {
	/* ��Ӳ�̿�����д����ʼ������ַ��Ҫ��д�������� */
	struct ide_channel* channel = hd->my_channel;

	/* д��Ҫ��д��������*/
	io_out8(reg_sect_cnt(channel), sec_cnt);	 // ���sec_cntΪ0,���ʾд��256������

	/* д��lba��ַ(��������) */
	io_out8(reg_lba_l(channel), lba);			 // lba��ַ�ĵ�8λ
	io_out8(reg_lba_m(channel), lba >> 8);		 // lba��ַ��8~15λ
	io_out8(reg_lba_h(channel), lba >> 16);		 // lba��ַ��16~23λ

	/*
	* ��Ϊlba��ַ��24~27λҪ�洢��device�Ĵ�����0��3λ,
    * �޷�����д����4λ,�����ڴ˴���device�Ĵ���������д��һ��
	*/
	io_out8(reg_dev(channel), BIT_DEV_MBS | BIT_DEV_LBA | (hd->dev_no == 1 ? BIT_DEV_DEV : 0) | lba >> 24);
}

void cmd_out(struct ide_channel* channel, unsigned char cmd) {
/* ��ͨ��channel������cmd */
/* ֻҪ��Ӳ�̷���������㽫�˱����Ϊtrue(1),Ӳ���жϴ��������Ҫ���������ж� */
   channel->expecting_intr = 1;
   io_out8(reg_cmd(channel), cmd);
}

void read_from_sector(struct disk* hd, void* buf, unsigned char sec_cnt) {
	/* Ӳ�̶���sec_cnt�����������ݵ�buf */
	unsigned int size_in_byte;
	if (sec_cnt == 0) {
		/* ��Ϊsec_cnt��8λ����,�������������丳ֵʱ,��Ϊ256��Ὣ���λ��1������Ϊ0 */
		size_in_byte = 256 * 512;
	} else { 
		size_in_byte = sec_cnt * 512; 
	}
	/************************************************************************
	*                             ���￼�����ݳ���                          *
	************************************************************************/
	buf = io_in8(reg_data(hd->my_channel));			// �������������ڵ�ַΪbuf���ڴ���
}

void write2sector(struct disk* hd, void* buf, unsigned char sec_cnt) {
	/* ��buf��sec_cnt����������д��Ӳ�� */
	unsigned int size_in_byte;
	if (sec_cnt == 0) {
	/* ��Ϊsec_cnt��8λ����,�������������丳ֵʱ,��Ϊ256��Ὣ���λ��1������Ϊ0 */
		size_in_byte = 256 * 512;
	} else { 
		size_in_byte = sec_cnt * 512; 
	}
	/************************************************************************
	*                             ���￼�����ݳ���                          *
	************************************************************************/
	io_out8(reg_data(hd->my_channel), (int)buf);
}
