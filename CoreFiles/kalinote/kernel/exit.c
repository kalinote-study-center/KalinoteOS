/* 该文件负责进程(任务)终止和退出时的处理，以及相关系统调用 */
/* 多任务处理的主要部分仍在kernel/mtask.c */
#include <bootpack.h>

void task_release(struct TASK *task) {
	/* 释放某个任务(用于任务彻底运行结束) */
	/* 将同时释放该任务的task结构体及所占用内存 */
	
	if(task->flags == TASK_UNUSED)		// 如果task的flag是没有被使用的，则不进行处理
		return;
	
	/* TODO：这里具体需要进行哪些操作还需要再考虑一下 */
	
	return;
}

static inline int task_send_sig(int sig, struct TASK *task, int priv) {
	/* 向任务task发送信号sig，权限为priv */
	/* 其中priv为强制发送信号的标志，也即是如果priv为1，则忽略用户权限，强行发送信号 */
	
	if((task->flags == TASK_UNUSED) || sig < 1 || sig > MAX_SIG)		// sig超出范围，或task为空
		return -1;								// 这里返回一个参数错误的错误代码(errno)，后续进行更改(TODO)
		
	if(priv)									// 这里先只进行权限判断，等以后有用户权限体系后再加上用户权限判断
		task->signal |= (1<<(sig-1));			// 因为是位图，每一位代表一个信号，所以这样处理
	else
		return -2;								// 这里返回一个权限不足(操作不允许)的错误代码，后续进行更改(TODO)
	
	return 0;
}

void task_kill_session(void) {
	/* 终止会话(session) */
	/* 扫描所有task，如果其session等于当前task的session，则发送挂断进程(SIGHUP)的信号 */
	struct TASK *task = task_now();
	int i;
	
	for(i = 0; i < MAX_TASKS ; i++) {
		if(taskctl->tasks0[i].task_id_num.session == task->task_id_num.session)
			taskctl->tasks0[i].signal |= 1<<(SIGHUP-1);		// 发送挂断进程(SIGHUP)信号
	}
	return;
}

int task_kill(int pid, int sig) {
	/* 向进程(任务)或组发送信号 */
	/*
	* 其中：
	* 参数pid是进程号；sig是需要发送的信号。
	* 如果pid > 0, 则信号被发送给进程号是pid的进程。
	* 如果pid = 0, 那么信号就会被发送给当前进程的进程组中的所有进程。
	* 如果pid = -1,则信号sig就会发送给除前两个进程(idle和systask)外的所有进程
	* 如果pid < -1,则信号sig将发送给进程组-pid的所有进程。
	* 如果信号sig=0,则不发送信号，但仍会进行错误检查。如果成功则返回0.
	* 该函数扫描任务数组表，并根据pid的值对满足条件的进程发送指定信号sig。若pid=0,
	* 表明当前进程是进程组组长，因此需要向所有组内进程强制发送信号sig.
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
	/* 通知父进程，向进程pid发送SIGCHLD信号 */
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
	/* TODO：没有找到父进程，后续添加处理 */
	return;
}

void do_exit(int *esp) {
	/* 退出处理函数(TODO：用于代替现有的cmd_exit函数) */
	/* TODO：添加相关处理 */
	asm_end_app(esp);
}
