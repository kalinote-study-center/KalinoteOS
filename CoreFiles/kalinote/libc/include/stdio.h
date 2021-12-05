/* C语言标准函数库 stdio for KalinoteOS */

#if (!defined(STDIO_H))

#define STDIO_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

/* golibc */
#define EOF				-1			// End Of File
#define BUFSIZ			8192		// 这个宏是一个整数，该整数代表了 setbuf 函数使用的缓冲区大小。
#define FOPEN_MAX		16			// (暂定)这个宏是一个整数，该整数代表了系统可以同时打开的文件数量。
#define FILENAME_MAX	8			// (限制于FAT12)这个宏是一个整数，该整数代表了字符数组可以存储的文件名的最大长度。如果实现没有任何限制，则该值应为推荐的最大值。

struct _iobuf {  
        char *_ptr;  			// 文件输入的下一个位置
        int   _cnt;  			// 当前缓冲区的相对位置
        char *_base;  			// 基础位置(文件起始位置)
        int   _flag;  			// 文件标志
        int   _file;  			// 文件有效性验证
        int   _charbuf;  		// 检查缓冲区状况，如果无缓冲区则不读取
        int   _bufsiz;  		// 文件大小
        char *_tmpfname;  		// 临时文件名
        };  
typedef struct _iobuf FILE;			//FILE *stream 文件流

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
