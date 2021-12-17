void KaliMain(void);

void KaliStartup(void) {
	/* 系统(C语言部分)真正的入口其实在这里 */
	/* 将来在KaliMain执行之前想做什么的话，就在这里写上 */

	KaliMain();

	/* 将来KaliMain结束后想让他这理做点什么的话，就在这里写上 */

	return;
}
