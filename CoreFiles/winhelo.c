/* 生成一个窗口的应用程序的演示代码 */
int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_end(void);

char buf[150 * 50];

void KaliMain(void){
	int win;
	win = api_openwin(buf, 150, 50, -1, "hello");
	api_end();
}
