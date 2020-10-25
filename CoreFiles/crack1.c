/* 系统安全性测试程序1 */

void KaliMain(void){
	*((char *) 0x00102600) = 0;
	return;
}
