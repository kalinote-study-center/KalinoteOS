//naskfunc中的函数
void io_hlt(void);		//hlt功能
void write_mem8(int addr, int data);		//写入内存


void KaliMain(void){
	/*这里是主程序*/
	int i;
	char *p;
	
	for(i = 0xa0000; i <= 0xaffff; i++){
		p = (char *) i;
		*p = i & 0x0f;
	}
	
	for(;;){
		//停止CPU
		io_hlt();
	}
}

void HariMain(void){
	KaliMain();
}