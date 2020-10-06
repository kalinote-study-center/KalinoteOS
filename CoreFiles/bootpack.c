/*茅鮫中才嶄僅翌議凪麿侃尖*/
#include <stdio.h>
#include "bootpack.h"

extern struct FIFO8 keyfifo;
void enable_mouse(void);			//爾試報炎
void init_keyboard(void);			//兜兵晒囚徒陣崙窮揃

void KaliMain(void){
	/*宸戦頁麼殻會*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;		//尼強佚連(BOOTINFO潤更悶)
	char s[40], mcursor[256], keybuf[32];
	int mx, my, i;
	
	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC兜兵晒潤崩��盾茅CPU議嶄僅鋤峭 */
	
	fifo8_init(&keyfifo, 32, keybuf);							//兜兵晒fifo産喝曝
	io_out8(PIC0_IMR, 0xf9); /* 塋俯PIC1才囚徒(11111001) */
	io_out8(PIC1_IMR, 0xef); /* 塋俯報炎(11101111) */
	
	init_keyboard();		//兜兵晒囚徒
	
	init_palette();												//兜兵晒距弼医
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);		//兜兵晒徳鳥
	/*
	* 廣�砕眄鏥� binfo->vram 吉勺噐(*binfo).vram
	* 凪麿議揖尖
	*/
	mx = (binfo->scrnx - 16) / 2; 								//報炎峺寞了崔��潮範葎徳鳥嶄伉
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL_LDBLUE);					//兜兵晒報炎峺寞
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16,
		mx, my, mcursor, 16);									//紙崙報炎
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL_WHITE, s);
	/*
	* 廣�佐吠欝鎚鷄蟇馨慰残槓�COL_LDBLUE議歓弼��侭參卞強頁氏顕固渠和中議坪否
	* 音狛朔中氏盾畳宸倖諒籾
	*/
	
	putfonts8_asc(binfo->vram, binfo->scrnx, 236, 181, COL_DRED, "KalinoteOS");
	putfonts8_asc(binfo->vram, binfo->scrnx, 235, 180, COL_BRED, "KalinoteOS");
	/*
	* 峪勣枠紙崙匯演圧弼忖悶��隼朔壅紙崙匯演疏弼忖悶��隼朔曾倖忖憲堪餓1倖�駛悖�祥辛參屁竃羨悶湖栖
	* 朔中距屁痕方猟周潤更朔辛參紗倖痕方栖廨壇紙崙羨悶忖憲堪
	*/
	
	//延楚�犢慊敞檻�圻猟壓及98匈
	//sprintf(s, "scrnx = %d", binfo->scrnx);
	//putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL_WHITE, s);
	
	enable_mouse();			//爾試報炎
	
	for(;;){
		//唯峭CPU
		io_cli();
		if (fifo8_status(&keyfifo) == 0) {
			io_stihlt();
		} else {
			i = fifo8_get(&keyfifo);
			io_sti();
			sprintf(s, "%02X", i);
			boxfill8(binfo->vram, binfo->scrnx, COL_LDBLUE, 0, 16, 15, 31);
			putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL_WHITE, s);
		}
	}
}


#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* 吉棋囚徒陣崙窮揃彈姥頼穎 - 緩侃圻坪否壓及140匈 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(void)
{
	/* 兜兵晒囚徒陣崙窮揃 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(void)
{
	/* 爾試報炎 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return; /* うまくいくとACK(0xfa)が僕佚されてくる */
}


void HariMain(void){
	/*狼由尼強秘笥*/
	KaliMain();
}
