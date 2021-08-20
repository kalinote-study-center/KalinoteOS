/* C语言标准函数库 string for KalinoteOS  */

#if (!defined(STRING_H))

#define STRING_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#include <stddef.h>		/* size_t */

char *strcpy(char *s, const char *ct);							// 把 s 所指向的字符串复制到 ct。
char *strncpy(char *s, const char *ct, size_t n);				// 把 s 所指向的字符串复制到 ct，最多复制 n 个字符。
char *strcat(char *s, const char *ct);							// 把 s 所指向的字符串追加到 ct 所指向的字符串的结尾。
char *strncat(char *s, const char *ct, size_t n);				// 把 s 所指向的字符串追加到 ct 所指向的字符串的结尾，直到 n 字符长度为止。
int strcmp(const char *cs, const char *ct);						// 把 cs 所指向的字符串和 ct 所指向的字符串进行比较。
int strncmp(const char *cs, const char *ct, size_t n);			// 把 cs 和 ct 进行比较，最多比较前 n 个字节。
int strcoll(const char *str1, const char *str2);				// 把 str1 和 str2 进行比较，结果取决于 LC_COLLATE 的位置设置。
char *strchr(const char *cs, int c);							// 在参数 str 所指向的字符串中搜索第一次出现字符 c（一个无符号字符）的位置。
char *strrchr(const char *cs, int c);							// 在参数 str 所指向的字符串中搜索最后一次出现字符 c（一个无符号字符）的位置。
size_t strspn(const char *s, const char *accept);				// 检索字符串 s 中第一个不在字符串 accept 中出现的字符下标。
size_t strcspn(const char *s, const char *reject);				// 检索字符串 s 开头连续有几个字符都不含字符串 reject 中的字符。
char *strpbrk(const char *s, const char *accept);				// 检索字符串 s 中第一个匹配字符串 accept 中字符的字符，不包含空结束字符。也就是说，依次检验字符串 s 中的字符，当被检验字符在字符串 accept 中也包含时，则停止检验，并返回该字符位置。
char *strstr(const char *cs, const char *ct);					// 在字符串 cs 中查找第一次出现字符串 ct（不包含空结束字符）的位置。
size_t strlen(const char *cs);									// 计算字符串 str 的长度，直到空结束字符，但不包括空结束字符。
// char *strerror(int errnum);									// (这个后面再做)从内部数组中搜索错误号 errnum，并返回一个指向错误消息字符串的指针。
// char *strtok(char *str, const char *delim);					// (这个后面再做)分解字符串 str 为一组字符串，delim 为分隔符。
// size_t strxfrm(char *dest, const char *src, size_t n);		// (这个后面再做)根据程序当前的区域选项中的 LC_COLLATE 来转换字符串 src 的前 n 个字符，并把它们放置在字符串 dest 中。

void *memcpy(void *s, const void *ct, size_t n);				// 从 ct 复制 n 个字符到 s。
void *memmove(void *s, const void *ct, size_t n);				// 另一个用于从 s 复制 n 个字符到 ct 的函数。
int memcmp(const void *cs, const void *ct, size_t n);			// 把 cs 和 str2 的前 n 个字节进行比较。
void *memchr(const void *cs, int c, size_t n);					// 在参数 cs 所指向的字符串的前 n 个字节中搜索第一次出现字符 c（一个无符号字符）的位置。
void *memset(void *s, int c, size_t n);							// 复制字符 c（一个无符号字符）到参数 s 所指向的字符串的前 n 个字符。
char *strdup(const char *s);

#if (defined(__cplusplus))
	}
#endif

#endif
