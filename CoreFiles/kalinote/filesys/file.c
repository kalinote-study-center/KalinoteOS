/* 文件处理相关程序 */

#include "../bootpack.h"

void file_readfat(int *fat, unsigned char *img){
	/* 将磁盘映像中的FAT解码 */
	/* 
	* 关于FAT的表示方法：
	* FAT(文件分配表)从第二扇区开始(第一扇区是MBR，以0x55aa结尾)，到后面的9个扇区(也就是2到10扇区为FAT表)
	* FAT中的内容在读取前需要先进行解压缩
	* 比如最开始几个字节为 0F F4 34，解压结果就是40F 34F，就是以三个字节为一组，
	* 把中间字节的高位放到最前面，低位放到最后面
	* 在0x1400-0x25ff的位置有FAT备份，防止FAT损坏导致数据丢失
	*/
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {
		fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
	return;
}

void file_loadfile(int clustno, int size, char *buf, int *fat, char *img){
	/* 加载文件 */
	int i;
	for (;;) {
		if (size <= 512) {
			/* 根据FAT12可知，如果文件小于512字节就不用FAT表解码 */
			for (i = 0; i < size; i++) {
				/* 直接把文件内容全部加载到缓存内存 */
				buf[i] = img[clustno * 512 + i];
			}
			break;
		}
		/* 超过512字节 */
		/* 通过FAT表解码并分段读入 */
		for (i = 0; i < 512; i++) {
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}
	return;
}

struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max){
	/* 由于FAT12格式原因，最多储存文件数量为224个(max) */
	int i, j;
	char s[12];
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {
		if (j >= 11) { return 0; /* 没有找到 */ }
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				/* 将小写字母转换为大写字母 */
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for (i = 0; i < max; ) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i; /* 找到文件 */
		}
next:
		i++;
	}
	return 0; /* 没有找到 */
}

char *file_loadfile2(int clustno, int *psize, int *fat)
{
	int size = *psize, size2;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char *buf, *buf2;
	buf = (char *) memman_alloc_4k(memman, size);
	file_loadfile(clustno, size, buf, fat, (char *) (ADR_DISKIMG + 0x003e00));
	if (size >= 17) {
		size2 = kca_getsize(buf);
		if (size2 > 0) {
			buf2 = (char *) memman_alloc_4k(memman, size2);
			kca_decomp(buf, buf2, size2);
			memman_free_4k(memman, (int) buf, size);
			buf = buf2;
			*psize = size2;
		}
	}
	return buf;
}
