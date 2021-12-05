/* C语言标准函数库 signal for KalinoteOS */

#if (!defined(SIGNAL_H))

#define SIGNAL_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* signal.h 头文件定义了一个变量类型 sig_atomic_t、两个函数调用和一些宏来处理程序执行期间报告的不同信号。 */
/* 由于KalinoteOS暂时还没有做进程间通信，所以暂时用不上 */

#if (defined(__cplusplus))
	}
#endif

#endif
