/* �ļ�������س��� */
#include "../bootpack.h"
#include <string.h>

void file_readfat(int *fat, unsigned char *img){
	/* ������ӳ���е�FAT���� */
	/* 
	* ����FAT�ı�ʾ������
	* FAT(�ļ������)�ӵڶ�������ʼ(��һ������MBR����0x55aa��β)���������9������(Ҳ����2��10����ΪFAT��)
	* FAT�е������ڶ�ȡǰ��Ҫ�Ƚ��н�ѹ��
	* �����ʼ�����ֽ�Ϊ 0F F4 34����ѹ�������40F 34F�������������ֽ�Ϊһ�飬
	* ���м��ֽڵĸ�λ�ŵ���ǰ�棬��λ�ŵ������
	* ��0x1400-0x25ff��λ����FAT���ݣ���ֹFAT�𻵵������ݶ�ʧ
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
	/* �����ļ� */
	int i;
	for (;;) {
		if (size <= 512) {
			/* ����FAT12��֪������ļ�С��512�ֽھͲ���FAT����� */
			for (i = 0; i < size; i++) {
				/* ֱ�Ӱ��ļ�����ȫ�����ص������ڴ� */
				buf[i] = img[clustno * 512 + i];
			}
			break;
		}
		/* ����512�ֽ� */
		/* ͨ��FAT����벢�ֶζ��� */
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
	/* ����FAT12��ʽԭ����ഢ���ļ�����Ϊ224��(max) */
	/* TODO:����·������ */
	int i, j;
	char s[12];
	
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {
		if (j >= 11) { return 0; /* û���ҵ� */ }
		if (name[i] == '.' && j <= 8) {
			/* ����ļ������ҵ��㣬��ֱ�Ӷ���׺ */
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				/* ��Сд��ĸת��Ϊ��д��ĸ */
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for (i = 0; i < max; ) {
		if (finfo[i].name[0] == 0x00) {
			/* �ļ�����һ������0(ʮ�����ƣ������ַ�)�������û���ļ� */
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i; /* �ҵ��ļ� */
		}
next:
		i++;
	}
	return 0; /* û���ҵ� */
}

struct FILEINFO *dir_search(char *name, struct FILEINFO *finfo, int max){
	/* ����(��ǰĿ¼�µ�)��Ŀ¼ */
	/* ���ļ���ͬ���ǣ�Ŀ¼û�к�׺�� */
	/* ����FAT12��ʽԭ����ഢ���ļ�����Ϊ224��(max) */
	int i, j;
	char s[9];
	for (j = 0; j < 8; j++) {
		s[j] = ' ';
	}
	j = 0;
	/* �����ļ��� */
	for (i = 0; name[i] != 0; i++) {
		if (j >= 8) { return 0; /* û���ҵ� */ }
		s[j] = name[i];
		if ('a' <= s[j] && s[j] <= 'z') {
			/* ��Сд��ĸת��Ϊ��д��ĸ */
			s[j] -= 0x20;
		} 
		j++;
	}
	/* ���� */
	for (i = 0; i < max; ) {
		if (finfo[i].name[0] == 0x00) {
			/* �ļ�����һ������0(ʮ�����ƣ������ַ�)�������û���ļ� */
			break;
		}
		if ((finfo[i].type & 0xef) == 0) {
			for (j = 0; j < 8; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i; /* �ҵ�Ŀ¼ */
		}
next:
		i++;
	}
	return 0; /* û���ҵ� */
}

struct FILEINFO *dir_check(char *dir, int *fat) {
	/* ͨ������·����ȡĿ¼��Ϣ */
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);		/* ��Ŀ¼��Ϣ */
	char dirname_buf[8] = {0};
	int i,j=0;
	
	if(strcmp(dir, "/") == 0||strcmp(dir, "/.") == 0){
		/* ����ֻ��һ����Ŀ¼ */
		return finfo;
	}

	for(i = 0;i <= strlen(dir);i++){
		// debug_print("%d/%d\n",i,strlen(dir));
		if(j>8)return 0;	/* �ļ������Ƴ��ȳ���8 */
		if(dir[i]=='/'||dir[i]==0){
			if(strlen(dirname_buf)==0){continue;}	/* ������Ŀ¼��'/' */
			/* һ���ļ��������꣬Ѱ��Ŀ¼ */
			dirname_buf[j] = 0;
			finfo = dir_search(dirname_buf, finfo, 224);
			// debug_print("DIR_CHECK>check %s,finfo:%d\n",dirname_buf,finfo);
			if(finfo == 0) {
				/* û���ҵ� */
				return 0;
			} else {
				/* 
				// ���ڴ���
				file_loadfile(finfo->clustno, 512, subdirinfo_buf, fat, (char *) (ADR_DISKIMG + 0x003e00));
				finfo = (struct FILEINFO *)subdirinfo_buf;
				*/
				finfo = (struct FILEINFO *)((finfo->clustno)*512 + ADR_DISKIMG + 0x003e00);
				debug_print("%d",finfo);
			}
			if(dir[i]=='/'&&dir[i+1]==0){
				/*
				* ������һ���ַ���'/'��������ȡ��'/'�ͻ�ִ���ж� 
				* Ĭ���ַ�����������󻹻�ִ��һ���ж�
				* �����ᵼ��'/'��β���ַ������д����ж�
				* ��������������if��֤������ַ��������һ���жϣ�����'/'��β
				* �����ֶ�break
				* TODO������������������û�и��õİ취���
				*/
				break;
			}
			j=0;
		} else {
			/* ���������ļ��� */
			dirname_buf[j] = dir[i];
			// debug_print("DIR_CHECK>copy %s,finfo:%d\n",dirname_buf,finfo);
			j++;
		}
	}
	return finfo;
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

/***************************************************
*  ������Щ���뻹û�о���������������С��ʹ�ã���  *
***************************************************/

// struct FILEINFO *file_rename(char *filename, char *newname) {
	// /*
	// * TODO��
	// * ���ж�·��(�ļ���)�Ǿ���·���������·��
	// * Ȼ��ͨ��·��(�ļ���)��ȡfinfo�����ַ
	// * �ж�newname�Ƿ���Ϲ���
	// * �޸�finfo���ļ�����������finfo
	// */
	
	// return finfo;
// }

// void file_writefat(int *fat, int no, int data) {
	// unsigned char *img = (unsigned char *) ADR_FAT;
	// int i = no / 2, j, k;

	// /* ��imgд������ */
	// fat[no] = data;

	// if((no & 1) == 0) {	/* ���no��ż���Ļ��ͺ�֮ǰ��һ����� */
		// j = fat[no - 1];
		// k = fat[no];
	// } else {	/* �������Ļ����ͺͺ���Ķ���һ����� */
		// j = fat[no];
		// k = fat[no + 1];
	// }

	// /* �������ļ� */
	// img[i * 3] = (unsigned char) (j & 0x0ff);
	// img[i * 3 + 1] = (unsigned char) ((k & 0x00f) << 4 | (j & 0xf00) >> 8);
	// img[i * 3 + 2] = (unsigned char) ((k & 0xff0) >> 4);

	// /* ���´��豸 */
	// img[2880 + i * 3] = img[i * 3];
	// img[2880 + i * 3 + 1] = img[i * 3 + 1];
	// img[2880 + i * 3 + 2] = img[i * 3 + 1];
	// return;
// }

// int fat_flush(void) {
	// int i, j;

	// /* ��I/O������ */
	// for(i = 1; i <= 18; i++) {
		// io_req(IO_WRITE, i);
	// }

	// /* ��I/O��ȡ����״̬ */
	// for(i = 1; i <= 18; i++) {
		// j = io_stat(IO_WRITE, i);
		// if(j == 1) {
			// return 1;
		// }
	// }
	// return 0;
// }

// int file_delete(int *fat, struct FILEINFO *file) {
	// int clustno, ncno, i;

	// /* ��Ŀ¼����Ϊɾ�� */
	// file->name[0] = 0xe5;

	// /* ȡ��FAT��� */
	// ncno = file->clustno;
	// for(; fat[ncno] >= 0x0ff8;) {
		// clustno = ncno;
		// ncno = fat[clustno];
		// file_writefat(fat, clustno, 0x0000);
	// }
	// i = fat_flush();
	// if(i != 0) {
		// return 1;
	// }
	// return 0;
// }
