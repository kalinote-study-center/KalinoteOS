/* I/O相关 */
#include "../bootpack.h"

int *iomem;				/* 缓存状态 */
int *iorreq, *iowreq;	/* 请求状态 */

void io_init(void) {
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct MEMMAN *mem = (struct MEMMAN *) MEMMAN_ADDR;
	int i;

	iomem = (int *) memman_alloc(mem, 640);
	iorreq = (int *) memman_alloc(mem, 640);
	iowreq = (int *) memman_alloc(mem, 640);
	for(i = 0; i < binfo->cyls * 2; i++) {	/* 把读过的地方标上记号 */
		iomem[i] = 0x3ffff;
		iorreq[i] = 0;
		iowreq[i] = 0;
	}
	for(; i < 160; i++) {
		iomem[i] = 0;
		iorreq[i] = 0;
		iowreq[i] = 0;
	}
	return;
}

void io_runreq(char mode, int cyl, int head, int *req, int *mem) {
	int i = 1, j;

	for(;;) {
		if((*req & (1 << i)) != 0) {
			j = io_runreqsub(mode, cyl, head, i, 1);
			if(j == 0) {
				*mem |= 1 << i;
			} else {
				*mem &= ~(1 << i);
			}
			*req &= ~(1 << i);
		}
		i++;
		if(i > 18) {
			return;
		}
	}/*
	int sst, se, ss, i;

	se = 1;
	for(; se <= 18;) {
		sst = se;
		for(ss = 0; se <= 18; se++) {
			if((*req & (1 << se)) != 0) {
				ss++;
			} else {
				break;
			}
		}
		if(ss != 0) {
			i = io_runreqsub(mode, cyl, head, sst, ss);
			if(i == 0) {
				for(i = 0; i < ss; i++) {
					*mem |= 1 << (sst + i);
					*req &= ~(1 << (sst + i));
				}
			} else {
				for(i = 0; i < ss; i++) {
					*mem &= ~(1 << (sst + i));
					*req &= ~(1 << (sst + i));
				}
			}
		} else {
			se++;
		}
	}
	return;*/
}

int io_runreqsub(char mode, int cyl, int head, int sect, int sects)
{
	int i;

	fdc_req(mode, (char) cyl, (char) head, (char) sect, (char) sects);
	i = fdc_reqstat();
	return i;
}

void io_req(char mode, int s)
{
	int rno;
	char sect;

	rno = s / 18;
	sect = (char) (s % 18 + 1);

	if(mode == IO_READ) {
		if((iomem[rno] & (1 << sect)) == 0) {
			iorreq[rno] |= 1 << sect;
		}
	} else if(mode == IO_WRITE) {
		iowreq[rno] |= 1 << sect;
		iomem[rno] &= ~(1 << sect);
	}
	return;
}

int io_stat(char mode, int s) {
	int *req, rno, sect;

	rno = s / 18;
	sect = s % 18 + 1;
	if(mode == IO_READ) {
		req = &iorreq[rno];
		io_runreq(IO_READ, rno / 2, rno % 2, req, &iomem[rno]);
	} else if(mode == IO_WRITE) {
		req = &iowreq[rno];
		io_runreq(IO_WRITE, rno / 2, rno % 2, req, &iomem[rno]);
	} else {
		return 1;
	}

	for(;;) {
		if((iomem[rno] & (1 << sect)) != 0) {
			if((*req & (1 << sect)) == 0) {
				return 0;
			} else {
				return 1;
			}
		} else {
			timer_sleep(1);			
		}
	}
}
