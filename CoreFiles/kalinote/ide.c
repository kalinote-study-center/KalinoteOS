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
		
		/* ������Ҫע���ж� */
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
	*   ����ԭ��(����ϵͳ����)ʹ�õ���outsw������ָ���������ȣ�����io_out�� *
	* ʱû��������ܣ����Ժ�����ܻ���Ҫ��һ�����޸ģ���ͬ��                *
	************************************************************************/
	buf = (void*)io_in8(reg_data(hd->my_channel));			// �������������ڵ�ַΪbuf���ڴ���
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


char busy_wait(struct disk* hd) {
	/* �ȴ�30�� */
	struct ide_channel* channel = hd->my_channel;
	unsigned short time_limit = 30 * 1000;	     // ���Եȴ�30000����
	while (time_limit -= 10 >= 0) {
		if (!(io_in8(reg_status(channel)) & BIT_STAT_BSY)) {
			return (io_in8(reg_status(channel)) & BIT_STAT_DRQ);
		} else {
			//mtime_sleep(10);		     // ˯��10����(��δʵ��)
		}
	}
	return 0;
}

void ide_read(struct disk* hd, unsigned int lba, void* buf, unsigned int sec_cnt) {
	/* ��Ӳ�̶�ȡsec_cnt��������buf */
	unsigned int secs_op, secs_done = 0;  		// secs_op:ÿ�β�������������secs_done:�����������
	char error[64];

	lock_acquire (&hd->my_channel->lock);		// ��һ����ʱû�й���
	
/* 1. ��ѡ�������Ӳ�� */
	select_disk(hd);

	while(secs_done < sec_cnt) {
		if ((secs_done + 256) <= sec_cnt) {
			secs_op = 256;
		} else {
			secs_op = sec_cnt - secs_done;
		}
		
/* 2. д������������������ʼ������ */
		select_sector(hd, lba + secs_done, secs_op);
		
/* 3. ִ�е�����д��reg_cmd�Ĵ��� */
		cmd_out(hd->my_channel, CMD_READ_SECTOR);			// ׼����ʼ������
		
		/******************************   �����Լ���ʱ��  **********************************
		* ��Ӳ���Ѿ���ʼ����(��ʼ���ڲ������ݻ�д����)����������Լ�,����Ӳ���Ѿ���ʼæ��, *
		* ���Լ�����,�ȴ�Ӳ����ɶ�������ͨ���жϴ���������Լ�                          *
		***********************************************************************************/
		sema_down(&hd->my_channel->disk_done);				// ��һ����ʱû�й���
		
/* 4. ���Ӳ��״̬�Ƿ�ɶ� */
		/* ������ʼִ���������*/
		if(!busy_wait(hd)) {								// busy_wait��ʱ��û��ʵ��
			/* ���ʧ�� */
			sprintf(error, "%s read sector %d failed!!!!!!\n", hd->name, lba);
		}

/* 5. �����ݴ�Ӳ�̵Ļ������ж��� */
		read_from_sector(hd, (void*)((unsigned int)buf + secs_done * 512), secs_op);
		secs_done += secs_op;
	}
	/* ������ʼ�ͷ���*/
	lock_release(&hd->my_channel->lock);
}

void ide_write(struct disk* hd, unsigned int lba, void* buf, unsigned int sec_cnt) {
	/* ��buf��sec_cnt��������д��Ӳ�� */
	unsigned int secs_op, secs_done = 0;  		// secs_op:ÿ�β�������������secs_done:�����������
	char error[64];
	
	lock_acquire (&hd->my_channel->lock);			// ��һ����ʱû�й���
	
/* 1. ��ѡ�������Ӳ�� */
	select_disk(hd);
	
	while(secs_done < sec_cnt) {
		if ((secs_done + 256) <= sec_cnt) {
			secs_op = 256;
		} else {
			secs_op = sec_cnt - secs_done;
		}
		
/* 2. д������������������ʼ������ */
		select_sector(hd, lba + secs_done, secs_op);
		
/* 3. ִ�е�����д��reg_cmd�Ĵ��� */
		cmd_out(hd->my_channel, CMD_WRITE_SECTOR);			// ׼����ʼд����
		
/* 4. ���Ӳ��״̬�Ƿ�ɶ� */
		/* ������ʼִ���������*/
		if(!busy_wait(hd)) {								// busy_wait��ʱ��û��ʵ��
			/* ���ʧ�� */
			sprintf(error, "%s write sector %d failed!!!!!!\n", hd->name, lba);
		}
/* 5 ������д��Ӳ�� */
		write2sector(hd, (void*)((unsigned int)buf + secs_done * 512), secs_op);

		/* ��Ӳ����Ӧ�ڼ������Լ� */
		sema_down(&hd->my_channel->disk_done);
		secs_done += secs_op;
	}
	/* ������ʼ�ͷ���*/
	lock_release(&hd->my_channel->lock);
}

void inthandler(int *esp){
	/* Ӳ���жϴ������ */
}
