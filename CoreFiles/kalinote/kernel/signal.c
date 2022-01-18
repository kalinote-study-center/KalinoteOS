/* 进程(任务)信号处理 */
#include "../bootpack.h"
#include <string.h>

static inline void save_old(char *from, char *to) {
	/* 复制sigaction数据到fs数据段to处(从系统段复制到任务数据段) */
	/* TODO：这里加一个验证，验证内存空间是否足够 */
	memcpy(from, to, sizeof(struct SIGACTION));
	return;
}

static inline void get_new(char *from, char *to) {
	/* 把sigaction 数据从fs数据段from位置复制到to处(从任务数据段复制到系统段) */
	/* 我知道这个函数跟save_old一模一样，我也知道这两个函数都只用了一个mempcy	*/
	/* 但是这又有什么问题呢:) */
	memcpy(from, to, sizeof(struct SIGACTION));
	return;
}

int signal(int signum, int handler, int restorer) {
	/* 为指定信号设置信号处理程序 */
	/*
	* 参数说明：
	* signum - 指定信号值
	* handler - 句柄
	* restorer - 恢复函数指针
	*/
	struct SIGACTION tmp;
	struct TASK *task = task_now();
	
	if(signum < 1 || signum > MAX_SIG || signum == SIGKILL)	/* 信号值超出指定范围，或信号值为SIGKILL */
		return -1;
		
	tmp.sa_handler = (void (*)(int)) handler;						/* 指定信号处理句柄 */
	tmp.sa_mask = 0;												/* 执行时的信号屏蔽码 */
	tmp.sa_flags = SA_ONESHOT | SA_NOMASK;							/* 该句柄只使用一次就恢复默认值，并允许信号在自己的处理句柄中收到 */
	tmp.sa_restorer = (void (*)(void)) restorer;					/* 保存恢复处理函数指针 */
	
	/* 取该信号原来的处理句柄，并设置该信号的sigaction结构，最后返回原信号句柄 */
	handler = (int) task->sigaction[signum-1].sa_handler;
	task->sigaction[signum-1] = tmp;
	return handler;	
}

int sigaction(int signum, struct SIGACTION *action, struct SIGACTION *oldaction) {
	/* 改变进程收到信号时的操作 */
	struct SIGACTION tmp;
	struct TASK *task = task_now();

	if (signum < 1 || signum > MAX_SIG || signum == SIGKILL)
		return -1;
    // 在信号的signaction结构中设置新的操作(动作)。如果oldaction指针不为空的话，则
    // 将原操作指针保存到oldaction所指的位置。
	tmp = task->sigaction[signum-1];
	get_new((char *) action,
		(char *) (signum-1+task->sigaction));
	if (oldaction)
		save_old((char *) &tmp,(char *) oldaction);
    // 如果允许信号在自己的信号句柄中收到，则令屏蔽码为0，否则设置屏蔽本信号。
	if (task->sigaction[signum-1].sa_flags & SA_NOMASK)
		task->sigaction[signum-1].sa_mask = 0;
	else
		task->sigaction[signum-1].sa_mask |= (1<<(signum-1));
	return 0;
}

void do_signal() {
	/* 
	* 这段代码将信号处理句柄插入到用户程序堆栈中，
	* 并在系统调用结束后立刻执行信号处理句柄程序，
	* 然后继续执行用户的程序
	*/
	/* TODO：这段程序有点麻烦，后面来实现(先完善syscall) */
	
}
