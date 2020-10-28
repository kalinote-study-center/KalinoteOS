/* 性能测试 */
#include <stdio.h>

int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_putstrwin(int win, int x, int y, int col, int len, char *str);
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
void api_initmalloc(void);
char *api_malloc(int size);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
void api_closewin(int win);
int api_getkey(int mode);
void api_end(void);

void KaliMain(void){
	char *buf, s[10];
	int counter = 0;
	int win;
	api_initmalloc();
	buf = api_malloc(160*100);
	win = api_openwin(buf,160,100,-1,"counter");
	for(;;){
		counter += 1;
		api_boxfilwin(win,5,25,150,95,7);
		sprintf(s,"%d",counter);
		api_putstrwin(win,15,50,0,8,s);
		api_refreshwin(win,0,0,160,100);
	}
	api_end();
}
