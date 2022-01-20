/* 性能测试 */
#include <stdio.h>
#include <apilib.h>

int main(){
	int *buf;
	char s[10];
	int counter = 0;
	int win;
	api_initmalloc();
	buf = api_malloc(160*100*4);
	win = api_openwin(buf,160,100,-1,"counter");
	for(;;){
		counter += 1;
		api_boxfilwin(win,5,25,150,95,0xffffff);
		sprintf(s,"%d",counter);
		api_putstrwin(win,15,50,0,8,s);
		api_refreshwin(win,0,0,160,100);
	}
	return 0;
}
