/* KalinoteOS syscall */
#if (!defined(APILIB_H))
#define APILIB_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

/* apilib */
void api_putchar(int c);                                                        // 在命令行中打印字符
void api_putstr0(char *s);                                                      // 在命令行中打印字符串，并以\0结尾
void api_putstr1(char *s, int l);                                               // 在命令行中打印字符串，并指定字符串长度
void api_end(void);                                                             // 结束应用程序
int api_openwin(int *buf, int xsiz, int ysiz, int col_inv, char *title);        // 打开窗口，返回窗口句柄
void api_putstrwin(int win, int x, int y, int col, int len, char *str);         // 在窗口指定未知打印字符串
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);           // 在窗口指定位置画一个指定大小的方块
void api_initmalloc(void);                                                      // 初始化内存管理结构
int *api_malloc(unsigned int size);                                             // 分配内存，并返回一个执行可用内存地址的指针
void api_free(char *addr, int size);                                            // 释放已分配的内存
void api_point(int win, int x, int y, int col);                                 // 在窗口指定位置绘制一个点
void api_refreshwin(int win, int x0, int y0, int x1, int y1);                   // 刷新窗口指定区域图层
void api_linewin(int win, int x0, int y0, int x1, int y1, int col);             // 在窗口指定位置绘制直线
void api_closewin(int win);                                                     // 关闭指定窗口
int api_getkey(int mode);                                                       // 获取键盘按键
int api_alloctimer(void);                                                       // 分配一个计时器
void api_inittimer(int timer, int data);                                        // 初始化计时器管理
void api_settimer(int timer, int time);                                         // 设置指定计时器的定时
void api_freetimer(int timer);                                                  // 释放指定计时器
void api_beep(int tone);                                                        // 控制蜂鸣器发生
int api_fopen(char *fname);                                                     // 打开一个文件，并返回文件句柄
void api_fclose(int fhandle);                                                   // 关闭一个文件
void api_fseek(int fhandle, int offset, int mode);                              // 寻找文件
int api_fsize(int fhandle, int mode);                                           // 获取文件大小
int api_fread(char *buf, int maxsize, int fhandle);                             // 读取文件
int api_cmdline(char *buf, int maxsize);                                        // 获取命令行参数
int api_getlang(void);                                                          // 获取语言模式，返回语言模式代码
void api_cls(void);                                                             // 清空命令行
unsigned int api_get_year(void);                                                // 获取系统年
unsigned int api_get_mon_hex(void);                                             // 获取系统月
unsigned int api_get_day_of_week(void);                                         // 获取系统日
unsigned int api_get_day_of_month(void);                                        // 获取系统星期
unsigned int api_get_hour_hex(void);                                            // 获取系统小时
unsigned int api_get_min_hex(void);                                             // 获取系统分
unsigned int api_get_sec_hex(void);                                             // 获取系统秒
unsigned int api_get_system_runtime(void);                                      // 获取系统运行时间
void api_showjpeg(char *filename, int x, int y, int win);                       // 在指定窗口的指定位置显示一张图片
int api_send_key_to_cmd(char *);                                                // 向命令行发送键值

#if (defined(__cplusplus))
	}
#endif

#endif
