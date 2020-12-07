/* 用于??日文半角字符?示 */
#include "apilib.h"

void KaliMain(void)
{
	static char s[9] = { 0xb2, 0xdb, 0xca, 0xc6, 0xce, 0xcd, 0xc4, 0x0a, 0x00 };
		/* 半角字符片假名イロハニホヘト的字符??+?行+0 */
	api_putstr0(s);
	api_end();
}
