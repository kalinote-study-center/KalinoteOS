//naskfunc中的函数
void io_hlt(void);		//hlt功能
void write_mem8(int addr, int data);		//写入内存


void KaliMain(void){
	/*这里是主程序*/
	int i;
	for(i = 0xa0000; i <= 0xaffff; i++){
		//向i地址循环写入15，15是白色，所以会是全屏白色
		write_mem8(i, 15); /* MOV BYTE [i],15 */
	}
	
	for(;;){
		//停止CPU
		io_hlt();
	}
}

void HariMain(void){
	KaliMain();
}