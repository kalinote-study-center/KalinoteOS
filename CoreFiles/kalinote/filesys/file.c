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
	/* ����Ŀ¼ */
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

int dir_check(char *dir, int *fat) {
	/* ͨ������·���ж�·���Ƿ���� */
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);		/* ��Ŀ¼��Ϣ */
	char dirname_buf[8] = {0};
	char subdirinfo_buf[512];
	int i,j=0;
	
	for(i = 0;i <= strlen(dir);i++){
		// debug_print("%d/%d\n",i,strlen(dir));
		if(dir[i]=='/'||dir[i]==0){
			if(strlen(dirname_buf)==0){continue;}
			/* һ���ļ��������꣬Ѱ��Ŀ¼ */
			dirname_buf[j] = 0;
			finfo = dir_search(dirname_buf, finfo, 224);
			// debug_print("DIR_CHECK>check %s,finfo:%d\n",dirname_buf,finfo);
			if(finfo == 0) {
				/* û���ҵ� */
				return FALSE;
			} else {
				file_loadfile(finfo->clustno, 512, subdirinfo_buf, fat, (char *) (ADR_DISKIMG + 0x003e00));
				finfo = (struct FILEINFO *)subdirinfo_buf;
			}
			if(dir[i]=='/'&&dir[i+1]==0){
				/*
				* ������һ���ַ���'/'��������ȡ��'/'�ͻ�ִ���ж� 
				* Ĭ���ַ�����������󻹻�ִ��һ���ж�
				* �����ᵼ��'/'��β���ַ������д����ж�
				* ��������������if��֤������ַ��������һ���жϣ�����'/'��β
				* �����ֶ�break
				* ����������������û�и��õİ취���
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
	return TRUE;
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
