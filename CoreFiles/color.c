/* 多色彩测试[该程序有问题] */
int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_initmalloc(void);
char *api_malloc(int size);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
int api_getkey(int mode);
void api_end(void);
void api_putstr0(char *s);
void api_point(int win, int x, int y, int col);

unsigned char rgb2pal(int r, int g, int b, int x, int y);

void KaliMain(void)
{
	char *buf;
	int win, x, y;
	api_initmalloc();
	buf = api_malloc(144 * 164);
	win = api_openwin(buf, 144, 164, -1, "full color");
	api_putstr0("enjoy full color :)");
	for (y = 0; y < 128; y++) {
		for (x = 0; x < 128; x++) {
			api_point(win, x + 8, y + 28, rgb2pal(x * 2, y * 2, 0, x, y));
			//buf[(x + 8) + (y + 28) * 144] = rgb2pal(x * 2, y * 2, 0, x, y);
		}
	}
	api_refreshwin(win, 8, 28, 136, 156);
	api_getkey(1); /* 等待按下任意键 */
	api_end();
}

unsigned char rgb2pal(int r, int g, int b, int x, int y)
{
	static int table[4] = { 3, 1, 0, 2 };
	int i;
	x &= 1; /* 判断是偶数还是奇数 */
	y &= 1;
	i = table[x + y * 2];	/* 用来生成中间色的常量 */
	r = (r * 21) / 256;	/* r 为 0-20 */
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;	/* r 为 0-5 */
	g = (g + i) / 4;
	b = (b + i) / 4;
	return 16 + r + g * 6 + b * 36;
}
