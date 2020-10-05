
void io_hlt(void);

void KaliMain(void)
{
	/*
	* 这里是主程序
	*/
	/*给一个无限循环*/
fin:
	io_hlt();
	goto fin;
}

void HariMain(void)
{
	KaliMain();
}