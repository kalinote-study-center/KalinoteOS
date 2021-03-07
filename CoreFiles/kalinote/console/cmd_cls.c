/* ÇåÆÁ */
#include "../bootpack.h"

void cmd_cls(struct CONSOLE *cons){
	/* ÇåÆÁ */
	int x, y;
	struct SHEET *sheet = cons->sht;
	debug_print("command>run clear at %d\n",cons);
	//for (y = 28; y < 28 + 128; y++) {
	for (y = 28; y < 28 + 448; y++) {
		//for (x = 8; x < 8 + 240; x++) {
		for (x = 8; x < 8 + 512; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL_BLACK;
		}
	}
	//sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	sheet_refresh(sheet, 8, 28, 8 + 512, 28 + 448);
	cons->cur_y = 28;
	return;
}
