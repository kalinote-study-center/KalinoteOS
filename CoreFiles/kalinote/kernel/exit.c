/* ���ļ��������(����)��ֹ���˳�ʱ�Ĵ����Լ����ϵͳ���� */
/* �����������Ҫ��������kernel/mtask.c */
#include <bootpack.h>

void task_release(struct TASK *task) {
	/* �ͷ�ĳ������(�������񳹵����н���) */
	/* ��ͬʱ�ͷŸ������task�ṹ�弰��ռ���ڴ� */
	
	if(task->flags == TASK_UNUSED)		// ���task��flag��û�б�ʹ�õģ��򲻽��д���
		return;
	
	/* TODO�����������Ҫ������Щ��������Ҫ�ٿ���һ�� */
	
	return;
}

static inline int task_send_sig(int sig, struct TASK *task, int priv) {
	/* ������task�����ź�sig��Ȩ��Ϊpriv */
	/* ����privΪǿ�Ʒ����źŵı�־��Ҳ�������privΪ1��������û�Ȩ�ޣ�ǿ�з����ź� */
	
	if((task->flags == TASK_UNUSED) || sig < 1 || sig > MAX_SIG)		// sig������Χ����taskΪ��
		return -1;								// ���ﷵ��һ����������Ĵ������(errno)���������и���(TODO)
		
	if(priv)									// ������ֻ����Ȩ���жϣ����Ժ����û�Ȩ����ϵ���ټ����û�Ȩ���ж�
		task->signal |= (1<<(sig-1));			// ��Ϊ��λͼ��ÿһλ����һ���źţ�������������
	else
		return -2;								// ���ﷵ��һ��Ȩ�޲���(����������)�Ĵ�����룬�������и���(TODO)
	
	return 0;
}

void task_kill_session(void) {
	/* ��ֹ�Ự(session) */
	/* ɨ������task�������session���ڵ�ǰtask��session�����͹ҶϽ���(SIGHUP)���ź� */
	struct TASK *task = task_now();
	int i;
	
	for(i = 0; i < MAX_TASKS ; i++) {
		if(taskctl->tasks0[i].task_id_num.session == task->task_id_num.session)
			taskctl->tasks0[i].signal |= 1<<(SIGHUP-1);		// ���͹ҶϽ���(SIGHUP)�ź�
	}
	return;
}

int task_kill(int pid, int sig) {
	/* �����(����)���鷢���ź� */
	/*
	* ���У�
	* ����pid�ǽ��̺ţ�sig����Ҫ���͵��źš�
	* ���pid > 0, ���źű����͸����̺���pid�Ľ��̡�
	* ���pid = 0, ��ô�źžͻᱻ���͸���ǰ���̵Ľ������е����н��̡�
	* ���pid = -1,���ź�sig�ͻᷢ�͸���ǰ��������(idle��systask)������н���
	* ���pid < -1,���ź�sig�����͸�������-pid�����н��̡�
	* ����ź�sig=0,�򲻷����źţ����Ի���д����顣����ɹ��򷵻�0.
	* �ú���ɨ�����������������pid��ֵ�����������Ľ��̷���ָ���ź�sig����pid=0,
	* ������ǰ�����ǽ������鳤�������Ҫ���������ڽ���ǿ�Ʒ����ź�sig.
	*/
	// struct TASK *task = task_now();
	// int i, err; 
	int ret = 0;
	
	if(pid == 0) {

	} else if(pid > 0) {

	} else if(pid == -1) {

	} else {

	}
	return ret;
}

void tell_father(int pid) {
	/* ֪ͨ�����̣������pid����SIGCHLD�ź� */
	int i;
	
	if(pid)
		for(i = 0; i < MAX_TASKS ; i++){
			if(taskctl->tasks0[i].flags != TASK_UNUSED)
				continue;
			if(taskctl->tasks0[i].task_id_num.pid != pid)
				continue;
			taskctl->tasks0[i].signal |= (1<<(SIGCHLD-1));
			return;
		}
	/* TODO��û���ҵ������̣�������Ӵ��� */
	return;
}

void do_exit(int *esp) {
	/* �˳�������(TODO�����ڴ������е�cmd_exit����) */
	/* TODO�������ش��� */
	asm_end_app(esp);
}
