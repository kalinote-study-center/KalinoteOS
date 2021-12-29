/* C语言标准函数库 stdio for KalinoteOS */

#if (!defined(STDIO_H))

#define STDIO_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>


#define EOF				-1			// End Of File
#define BUFSIZ			8192		// 这个宏是一个整数，该整数代表了 setbuf 函数使用的缓冲区大小。
#define FOPEN_MAX		16			// (暂定)这个宏是一个整数，该整数代表了系统可以同时打开的文件数量。
#define FILENAME_MAX	8			// (限制于FAT12)这个宏是一个整数，该整数代表了字符数组可以存储的文件名的最大长度。如果实现没有任何限制，则该值应为推荐的最大值。

/* 文件流结构体 */
typedef struct {
	/* 这个结构体基于《C标准库》P276 改编，以适应KalinoteOS操作系统 */
	unsigned short _Mode;		/* 流的状态位集合 */
	int _Handle;				/* 操作系统打开文件返回的句柄 int api_fopen(char *fname); */
	unsigned char *_Buf;		/* 指向缓冲流首地址的指针，如果没有分配缓冲区，则为空指针 */
	unsigned char *_Bend;		/* 超出缓冲区的第一个字符的指针，如果_Buf为空指针，则不定义 */
	unsigned char *_Next;		/* 指向读(写)操作的下一个字符的指针 */
	unsigned char *_Rend;		/* 指向超出读取数据范围的第一个字符的指针 */
	unsigned char *_Rsave;		/* 如果字符回退，则保存指针_Rend */
	unsigned char *_Wend;		/* 指向超出可写数据范围的第一个字符 */
	unsigned char _Back[2];		/* 保存回退字符的栈 */
	unsigned char _Cbuf;		/* 没有其他缓冲区可用时，能够使用的单字符缓冲区 */
	unsigned char _Nback;		/* 记录回退字符个数 */
	char *_Tmpnam;				/* 指向关闭文件时要删除的临时文件名的指针，或一个空指针 */
} FILE;

int sprintf(char *s, const char *format, ...);				// 发送格式化输出到字符串。
int vsprintf(char *s, const char *format, va_list arg);		// 使用参数列表发送格式化输出到字符串。
int putchar(int c);											// 把参数 char 指定的字符（一个无符号字符）写入到标准输出 stdout 中。
int printf(char *format, ...);								// 格式字符串到 str 中。
int scanf(const char *format, ...);							// 从标准输入 stdin 读取格式化输入。
int getchar(void);											// 从标准输入 stdin 获取一个字符（一个无符号字符）。
int puts(const char *str);									// 把一个字符串写入到标准输出 stdout，直到空字符，但不包括空字符。换行符会被追加到输出中。
char *gets(char *str);										// 从标准输入 stdin 读取一行，并把它存储在 str 所指向的字符串中。当读取到换行符时，或者到达文件末尾时，它会停止，具体视情况而定。
/* 文件的io函数暂时还没有做，等系统调用更完善再写 */


#if (defined(__cplusplus))
	}
#endif

#endif
