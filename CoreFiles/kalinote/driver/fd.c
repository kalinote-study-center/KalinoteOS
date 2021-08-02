/* 软盘驱动程序 */
#include "../bootpack.h"
#include <string.h>

/* 一些自定义的数据类型 */
typedef unsigned char	u_int8_t;			/* 8位无符号 */
typedef unsigned int	u_int32_t;			/* 32位无符号 */

static u_int32_t fdc_interrupt = 0; // 这个变量用于储存0x26(软盘控制器)中断状态

/* 软盘控制器操作 */

void inthandler26(int *esp){
	/* 软盘中断程序 */
	++fdc_interrupt;
	io_out8(PIC0_OCW2, 0x66);		/* 把IRQ-00信号接收完了的信息通知给中断(PIC)，6(软盘控制器)+0x60号端口 */
	return;
}

/*******************************************************************
*                   以下函数均仅限于本文件内使用                   *
*******************************************************************/

static int fdc_chk_interrupt() {
	/* 获取中断状态 */
	return fdc_interrupt;
}

static u_int32_t fdc_wait_interrupt() {
	/* 等待中断 */
	while(!fdc_chk_interrupt());
	return TRUE;
}

static void fdc_clear_interrupt() {
	/* 重置中断状态 */
	fdc_interrupt = 0;
}

static unsigned char in8(int addr) { return io_in8(addr);}				/* 从某个端口(端口地址)读取8bit数据 */
static void out8(int addr, unsigned char data) { io_out8(addr, data);}		/* 向某个端口(端口地址)写入8bit数据 */
static void disableInterrupt()  { io_cli(); }								/* 禁止中断 */
static void enableInterrupt()  { io_sti(); }								/* 开启中断 */

static void sysPrintc(int c) {
	/* 向当前任务窗口打印字符 */
	struct CONSOLE* cons = task_now()->cons;
	cons_printf(cons, "%c", c);
}

static void sysPrints(char* str) {
	/* 向当前任务窗口打印字符串 */
	struct CONSOLE* cons = task_now()->cons;
	cons_putstr0(cons, str);
}

static void sysPrintBin(int x) {
	/* 向当前任务窗口打印二进制数据 */
	struct CONSOLE* cons = task_now()->cons;
	cons_printf(cons, "%02x", x);
}

static u_int8_t dma_databuf[DMA_DATABUF];			/* 申请一个DMA数据缓冲区 */

static struct _dma_trans {
	u_int32_t count;
	u_int32_t addr;
} dma_trans;					/* 暂时还不知道是干什么用的 */

static struct FDC_RESULTS {
	u_int8_t gets;
	u_int8_t req_sense;
	u_int32_t status_count;
	u_int8_t status[10];
} fdc_results;					/* 结构体变量fdc_results为软盘控制器数据结果结构体 */

static void fdc_dma_start() {
	/* DMA开始 */
	out8(DMA_SGL_MSK_SEC, 0x02);
}

static void fdc_dma_stop() {
	/* DMA停止 */
	out8(DMA_SGL_MSK_SEC, 0x06);
}

void init_dma() {
	/* 初始化DMA */
	/* 重置 DMAC */
	out8(DMA_MSR_CLR_PRI, 0x00);
	out8(DMA_MSR_CLR_SEC, 0x00);
	
	out8(DMA_CMD_PRI, 0x00);
	out8(DMA_CMD_SEC, 0x00);
	
	/* 设置DMAC模式寄存器 */
	out8(DMA_MOD_PRI, 0xc0);
	out8(DMA_MOD_SEC, 0x46);
	
	out8(DMA_SGL_MSK_PRI, 0x00);
}

static void init_dma_r() {
	/* 初始化DMAC读取 */
	fdc_dma_stop();
	
	out8(DMA_MSR_CLR_SEC, 0x00);
	out8(DMA_CLR_FLP_SEC, 0);
	
	out8(DMA_MOD_SEC, 0x46);
	disableInterrupt();
	out8(DMA_ADD_SEC, dma_trans.addr >> 0);
	out8(DMA_ADD_SEC, dma_trans.addr >> 8);
	out8(DMA_TOP, dma_trans.addr >> 16);
	out8(DMA_CNT_SEC, dma_trans.count >> 0);
	out8(DMA_CNT_SEC, dma_trans.count >> 8);
	enableInterrupt();
	fdc_dma_start();
}

static void init_dma_w() {
	/* 初始化DMAC写入 */
	fdc_dma_stop();
	
	out8(DMA_MSR_CLR_SEC, 0x00);
	out8(DMA_CLR_FLP_SEC, 0);
	
	out8(DMA_MOD_SEC, 0x4a);  // memory >> I/O
	disableInterrupt();
	out8(DMA_ADD_SEC, dma_trans.addr >> 0);
	out8(DMA_ADD_SEC, dma_trans.addr >> 8);
	out8(DMA_TOP, dma_trans.addr >> 16);
	out8(DMA_CNT_SEC, dma_trans.count >> 0);
	out8(DMA_CNT_SEC, dma_trans.count >> 8);
	enableInterrupt();
	fdc_dma_start();
}

static int fdc_wait_msrStatus(u_int8_t mask, u_int8_t expected) {
	/* 等待MSR状态 */
	int count;
	for (count = 0; count < FDC_RESULT_MAXCOUNT; ++count) {
		u_int8_t status = in8(FDC_MSR);
		if ((status & mask) == expected)
			return status;
	}
	return -1;	/* 失败 */
}

static int fdc_cmd(const u_int8_t *cmd, const int length){
	/* 指令软盘控制器指令 */
	int i;
	debug_print("FDC>cmd busy check.\n");
	if (!fdc_wait_msrStatus(MSR_BUSY, MSR_READY)) {
		sysPrints("[FDC] cmd busy check error.\n");
		return FALSE;
	}
	debug_print("FDC>cmd busy check [OK]\n");
	
	debug_print("FDC>cmd out and msr check.\n");
	for (i = 0; i < length; ++i) {
		if (!fdc_wait_msrStatus(MSR_RQM | MSR_DIO, MSR_RQM)) {
			sysPrints("[FDC] msr RQM|DIO error\n");
			return FALSE;
		}
		out8(FDC_DAT, cmd[i]);
	}
	debug_print("FDC>cmd out and msr check [OK]\n");
	
	return TRUE;
}

static int fdc_read_results() {
	/* FDC读取结果 */
	int i;
	fdc_results.status_count = 0;

	if (fdc_wait_msrStatus(MSR_RQM | MSR_DIO, MSR_RQM | MSR_DIO) < 0) {
		sysPrints("fdc result phase error 1\n");
		return FALSE;
	}

	u_int8_t* msr = &fdc_results.status[0];
	for (i = 0; ; ++i) {
		*msr++ = in8(FDC_DAT);
		++fdc_results.status_count;

		int status = fdc_wait_msrStatus(MSR_RQM, MSR_RQM);
		if (status < 0) {
			sysPrints("fdc result phase error 2\n");
			return FALSE;
		}
		if (!(status & MSR_DIO))
			return TRUE;
	}
}

static int fdc_sense_interrupt() {
	/* FDC检测中断	*/
	static const u_int8_t cmd[] = { CMD_SENSE_INT_STS };

	fdc_clear_interrupt();
	if (fdc_cmd(cmd, sizeof(cmd)) != TRUE) {
		sysPrints("[FDC] sense interrupt status cmd error\n");
		return FALSE;
	}
	fdc_read_results();
	return TRUE;
}

static void fdc_motor_on() {
	/* 启动软盘电机 */
	out8(FDC_DOR, 0x1c);
}

static void fdc_motor_off() {
	/* 关闭软盘电机 */
	out8(FDC_DOR, 0x0c);
}

static int fdc_recalibrate() {
	/* 重新校准 */
	static const u_int8_t cmd[] = { CMD_RECALIBRATE, CMD_SUB };

	fdc_clear_interrupt();

	debug_print("FDC>Recalibrate Cmd.\n");
	if (!fdc_cmd(cmd, sizeof(cmd))) {
		sysPrints("[FDC] Recalibrate Cmd error\n");
		return FALSE;
	}
	debug_print("FDC>Recalibrate Cmd [OK]\n");

	debug_print("FDC>wait interrupt\n");
	if (!fdc_wait_interrupt())
		sysPrints("[FDC] wait interrupt error\n");
	debug_print("FDC>wait interrupt [OK]\n");
	
	/* get result */
	fdc_sense_interrupt();
	
	debug_print("FDC>Recalibrate  wait [OK]\n");
	if (!fdc_wait_msrStatus(MSR_BUSY, MSR_READY)) {
		sysPrints("[FDC] Recalibrate  wait fail\n");
		return FALSE;
	}
	debug_print("FDC>Recalibrate  wait [OK]\n");
	
	return TRUE;
}

static void fdc_specify() {
	/* 指定FDC */
	fdc_motor_on();

	static const u_int8_t specify_cmd[] = {
		CMD_SPECIFY,
		0xc1,  // ?
		0x10   // ?
	};
	fdc_clear_interrupt();
	fdc_cmd(specify_cmd, sizeof(specify_cmd));

	fdc_motor_off();
}

void init_fdc() {
	/* 初始化FDC */
	init_dma();
	dma_trans.addr = (u_int32_t)&dma_databuf[0];
	dma_trans.count = 512;

	out8(FDC_DOR, 0x0);
	out8(FDC_CCR, 0x0);
	out8(FDC_DOR, 0xc);

	fdc_specify();
}

static int fdc_seek(u_int8_t cyl) {
	/* 寻找 */
	int drive = 0, head = 0;
	u_int8_t cmd[] = {
		CMD_SEEK,       // 0x0f
		(head << 2) | drive,
		cyl
	};

	fdc_clear_interrupt();

	debug_print("FDC>seek cmd check.\n");
	if (!fdc_cmd(cmd, sizeof(cmd))) {
		sysPrints("[FDC] seek cmd error\n");
		return FALSE;
	}
	debug_print("FDC>seek cmd check [OK]\n");

	if (!fdc_wait_interrupt()) {
		sysPrints("[FDC][SEEK] wait interrupt error\n");
		return FALSE;
	}

	/* get result */
	if (!fdc_sense_interrupt()) {
		sysPrints("[FDC][SEEK] SIS error\n");
		return FALSE;
	}

	return TRUE;
}

void *fdc_read(int head, int track, int sector) {
	/* 读取 */
	int i,j;
	init_dma_r();
	
	fdc_motor_on();
	
	if (!fdc_recalibrate()) {
		sysPrints("[FDC][READ] recalibrate error\n");
		return NULL;
	}
	
	if (!fdc_seek(track)) {
		sysPrints("[FDC][READ] seek error\n");
		return NULL;
	}
	
	u_int8_t cmd[] = {
		CMD_READ,
		head << 2,   // head
		track,       // track
		head,        // head
		sector,      // sector
		0x2,         // sector length (0x2 = 512byte)
		0x12,        // end of track (EOT)
		0x1b,        // dummy GSR
		0            // dummy STP
	};
	
	fdc_cmd(cmd, sizeof(cmd));
	fdc_dma_stop();
	fdc_read_results();
	
	fdc_motor_off();
	
	// write the binary which we get from DMA
	sysPrints("READ DATA:\n");
	for (j = 0; j < 1; ++j) {
		for (i = 0; i < 16; ++i) {
			sysPrintBin(dma_databuf[j * 16 + i]);
			sysPrintc(' ');
		}
		sysPrintc('\n');
	}
	
	return NULL;
}

int fdc_write(void* buf, int head, int cyl, int sector) {
	/* 写入 */
	init_dma_w();
	fdc_motor_on();
	
	sysPrints("FDC_WRITE\n");
	if (!fdc_recalibrate()) {
		sysPrints("[FDC][WRITE] recalibrate error\n");
		return FALSE;
	}
	
	sysPrints("SEEK\n");
	if (!fdc_seek(cyl)) {
		sysPrints("[FDC][WRITE] seek error\n");
		return FALSE;
	}
	
	memcpy(dma_databuf, buf, 512);
	
	u_int8_t cmd[] = {
		CMD_WRITE,
		head << 2,      // head
		cyl,            // cylinder
		head,           // head
		sector,         // sector
		0x2,            // sector length (0x2 = 512byte)
		0x12,           // end of track (EOT)
		0x1b,           // dummy GSR
		0               // dummy STP
	};
	
	fdc_clear_interrupt();
	
	sysPrints("WRITE\n");
	if (!fdc_cmd(cmd, sizeof(cmd))) {
		sysPrints("[FDC][WRITE] cmd error\n");
		return FALSE;
	}
	
	if (!fdc_wait_interrupt()) {
		sysPrints("[FDC][WRITE] wait interrupt error\n");
		return FALSE;
	}
	
	if (!fdc_read_results()) {
		sysPrints("[FDC][WRITE] read result error\n");
		return FALSE;
	}
	sysPrints("WRITE OK\n");
	
	fdc_motor_off();
	return TRUE;
}

static int writeSector(int sector) {
	/* 执行将数据写入软盘1扇区（从内存映像） */
	const int SECTOR_COUNT = 18, HEAD_COUNT = 2;
	int cyl = sector / SECTOR_COUNT / HEAD_COUNT;
	int head = (sector / SECTOR_COUNT) & 1;
	int sec = (sector % SECTOR_COUNT) + 1;

#if 0
	sysPrints("SEEK\n");
	if (!fdc_seek(cyl)) {
		sysPrints("[FDC][WRITE] seek error\n");
		return FALSE;
  }

	unsigned char* buf = (unsigned char*)(ADR_DISKIMG + sector * CLUSTER_SIZE);
	memcpy(dma_databuf, buf, 512);
	
sprintf(s, "%p: %02x %02x %02x %02x\n", buf, dma_databuf[0], dma_databuf[1], dma_databuf[2], dma_databuf[3]);
sysPrints(s);

	u_int8_t cmd[] = {
		CMD_WRITE,
		head << 2,      // head
		cyl,            // cylinder
		head,           // head
		sec,            // sector
		0x2,            // sector length (0x2 = 512byte)
		0x1,            // end of track (EOT)
		0x1b,           // dummy GSR
		0xff,           // dummy STP
	};

	fdc_clear_interrupt();

	if (!fdc_cmd(cmd, sizeof(cmd))) {
		sysPrints("[FDC][WRITE] cmd error\n");
		return FALSE;
	}

	if (!fdc_wait_interrupt()) {
		sysPrints("[FDC][WRITE] wait interrupt error\n");
		return FALSE;
	}

	if (!fdc_read_results()) {
		sysPrints("[FDC][WRITE] read result error\n");
		return FALSE;
	}

	return TRUE;
#else
	unsigned char* buf = (unsigned char*)(ADR_DISKIMG + sector * CLUSTER_SIZE);
	return fdc_write(buf, head, cyl, sec);
#endif
}

short get_next_cluster(short cluster) {
	/* 获取下一个簇 */
	const unsigned char* p = DISK_FAT + (cluster >> 1) * 3;
	if ((cluster & 1) == 0) {
		return (p[0] | p[1] << 8) & 0xfff;
	} else {
		return (p[1] >> 4 | p[2] << 4) & 0xfff;
	}
}

static void set_next_cluster(short cluster, short next) {
	/* 设置下一个簇 */
	unsigned char* p = DISK_FAT + (cluster >> 1) * 3;
	if ((cluster & 1) == 0) {
		p[0] = next;
		p[1] = (p[1] & 0xf0) | ((next >> 8) & 0x0f);
	} else {
		p[1] = (p[1] & 0x0f) | ((next << 4) & 0xf0);
		p[2] = next >> 4;
	}
}

static int deleteFatClusters(short startCluster) {
	/* 返回更新的簇（以9位表示，9来自（0x1200-0x200）/512） */
	short cluster;
	int updatedClusterBits = 0;
	for (cluster = startCluster; cluster < 0xff0; ) {
		short next = get_next_cluster(cluster);
		set_next_cluster(cluster, 0x000);  // Free
		updatedClusterBits |= 1 << (cluster / CLUSTER_SIZE);
		cluster = next;
	}
	return updatedClusterBits;
}

static int writeDirTable(FDINFO* finfo) {
	/* 写入目录表 */
	int filePos = finfo - FINFO_TOP;  // 文件在目录表中的位置
	int offset = filePos * sizeof(FDINFO) / CLUSTER_SIZE;
	return writeSector(offset + CLUSTNO_ROOTDIR);
}

static int writeFat(int fatBits) {
	/* 写FAT */
	int i;
	const int FAT_CLUSTERS = 9;
	for (i = 0; i < FAT_CLUSTERS; ++i) {
		if (!(fatBits & (1 << i)))
			continue;
		if (!writeSector(i + CLUSTNO_FAT1))
			return FALSE;
	}
	return TRUE;
}

static int writeBack(FDHANDLE* fh, int fatBits) {
	/* 将文件项从内存写回软盘 */
	/* 写文件体 */
	short cluster;
	for (cluster = fh->finfo->clustno; ;) {
		if (!writeSector(cluster + CLUSTNO_ENTITY))
			return FALSE;
			cluster = get_next_cluster(cluster);
		if (cluster >= 0xff0)
			break;
  }

	// 写入目录项
	writeDirTable(fh->finfo);

	// 检测FAT中的哪个区域已更新
	for (cluster = fh->finfo->clustno; ; cluster = get_next_cluster(cluster)) {
		int p = cluster / 2 * 3 + (cluster & 1);
		fatBits |= (1 << (p / CLUSTER_SIZE));
		fatBits |= (1 << ((p + 1) / CLUSTER_SIZE));
		if (cluster >= 0xff0)
			break;
	}

	if (!writeFat(fatBits))
		return FALSE;

	sysPrints("WriteBack succeeded\n");
	return TRUE;
}

void fd_close(FDHANDLE* fh) {
	/* 关闭软盘 */
	if (fh->modified) {
		int fatBits = 0;
		fh->modified = FALSE;
		if (fh->cluster > 0) {
			fatBits = deleteFatClusters(fh->cluster);
			set_next_cluster(fh->cluster, 0xfff);  // End mark.
		}
		fh->finfo->size = fh->pos;

		// Update timestamp.
		unsigned char t[5];
		int year = read_rtc(t);
		fh->finfo->date = ((year - 1980) << 9) | ((t[0] - 1) << 5) | (t[1] - 1);
		fh->finfo->time = (t[2] << 11) | (t[3] << 5) | (t[4] / 2);
		
		// Write back to floppy disk.
		{
			init_dma_w();
			fdc_motor_on();

			sysPrints("FDC_WRITE\n");
			if (!fdc_recalibrate()) {
				sysPrints("[FDC][WRITE] recalibrate error\n");
		} else {
			if (!writeBack(fh, fatBits)) {
				sysPrints("WRITE_BACK failed\n");
			}
		}

			fdc_motor_off();
		}
	}
	fh->finfo = NULL;
}

static void make_file_name83(char s[12], const char* name) {
	/* 创建文件名 */
	memset(s, ' ', 11);
	int i;
	int j = 0;
	for (i = 0; j < 11 && name[i] != '\0'; ++i) {
		if (name[i] == '.') {
			j = 8;
		} else {
			s[j++] = name[i];
		}
	}
}

/*******************************************************************
*                   以上函数均仅限于本文件内使用                   *
*******************************************************************/

int fd_writeopen(FDHANDLE* fh, const char* filename) {
	/* FD写文件 */
	fh->finfo = NULL;
	fh->pos = 0;
	fh->cluster = 0;
	fh->modified = FALSE;

	int i;
	char s[12];
	make_file_name83(s, filename);
	FDINFO* finfo;
	for (i = 0; i < FINFO_MAX; ++i) {
		finfo = &FINFO_TOP[i];
		if (finfo->name[0] == 0x00 || finfo->name[0] == 0xe5) {  // End of table, or deleted entry.
			memset(finfo, 0x00, sizeof(FDINFO));
			memcpy(finfo->name, s, 11);
			finfo->type = 0x20;  // Normal file.
			goto found;
		}
		if ((finfo->type & 0x18) == 0) {
			if (strncmp((char*)finfo->name, s, 11) == 0)
				goto found;
		}
	}
	return FALSE;

found:
	fh->finfo = finfo;
	return TRUE;
}

static FDINFO* fd_search(const char* filename) {
	/* 搜索文件 */
	char s[12];
	int i;
	make_file_name83(s, filename);
	for (i = 0; i < FINFO_MAX; ++i) {
		FDINFO* finfo = &FINFO_TOP[i];
		if (finfo->name[0] == 0x00)  // End of table.
			return NULL;
		if ((finfo->type & 0x18) == 0) {
			if (strncmp((char*)finfo->name, s, 11) == 0)
				return finfo;
		}
	}
	return NULL;
}

int fd_delete(const char* filename) {
	/* 删除文件 */
	FDINFO* finfo = fd_search(filename);
	if (finfo == NULL)
		return FALSE;

	finfo->name[0] = 0xe5;  // 删除标记
	int deletedClusterBits = deleteFatClusters(finfo->clustno);

	// 写到软盘
	writeDirTable(finfo);
	writeFat(deletedClusterBits);

	return TRUE;
}

int fd_open(FDHANDLE* fh, const char* name) {
	/* 打开文件 */
	fh->pos = 0;
	fh->cluster = 0;
	fh->modified = FALSE;

	fh->finfo = fd_search(name);
	if (fh->finfo == NULL)
		return FALSE;
	fh->cluster = fh->finfo->clustno;
	return TRUE;
}

static unsigned char* clusterData(int cluster) {
	/* 簇数据 */
	return DISK_CLUSTER_DATA + cluster * CLUSTER_SIZE;
}

int fd_read(FDHANDLE* fh, void* dst, int requestSize) {
	/* 读取软盘 */
	int readSize = 0;
	unsigned char* p = dst;
	while (requestSize > 0) {
		if (fh->pos >= (int)fh->finfo->size)
			break;
		char forward = TRUE;
		int nextClusterPos = (fh->pos + CLUSTER_SIZE) & -CLUSTER_SIZE;
		if (nextClusterPos > (int)fh->finfo->size)
			nextClusterPos = fh->finfo->size;
		int blockBytes = nextClusterPos - fh->pos;
		if (blockBytes > requestSize) {
			blockBytes = requestSize;
			forward = FALSE;
		}
		const unsigned char* src = clusterData(fh->cluster) + (fh->pos % CLUSTER_SIZE);
		memcpy(p, src, blockBytes);
		p += blockBytes;
		fh->pos += blockBytes;
		if (forward)
			fh->cluster = get_next_cluster(fh->cluster);
		readSize += blockBytes;
		requestSize -= blockBytes;
	}
	return readSize;
}

short allocate_cluster(void) {
	/* 分配簇 */
	int i;
	for (i = 0; i < MAX_CLUSTER; ++i) {
		if (get_next_cluster(i) == 0) {
			set_next_cluster(i, 0xfff);  // Write end mark.
			return i;
		}
	}
	return -1;
}

int fd_write(FDHANDLE* fh, const void* srcData, int requestSize) {
	/* 写入软盘 */
	if (requestSize <= 0)
		return 0;

	fh->modified = TRUE;
	int writeSize = 0;
	unsigned char* src = (unsigned char*)srcData;
	while (requestSize > 0) {
		if (fh->pos == 0) {  // First write.
			if (fh->finfo->clustno > 0) {  // Exist old file: overwrite.
				fh->cluster = fh->finfo->clustno;
			} else {  // Not exist: allocate new cluster.
				fh->cluster = fh->finfo->clustno = allocate_cluster();
				// TODO: Error check.
			}
		} else if ((fh->pos % CLUSTER_SIZE) == 0) {  // Forward next cluster.
			short nextCluster = get_next_cluster(fh->cluster);
			if (nextCluster < 0xff0) {  // Valid: use it.
				fh->cluster = nextCluster;
			} else {
				nextCluster = allocate_cluster();
				// TODO: Error check.
				set_next_cluster(fh->cluster, nextCluster);
				fh->cluster = nextCluster;
			}
		}

		int size = CLUSTER_SIZE - (fh->pos % CLUSTER_SIZE);
		if (requestSize < size)
			size = requestSize;
		unsigned char* dst = clusterData(fh->cluster) + (fh->pos % CLUSTER_SIZE);

		memcpy(dst, src, size);
		fh->pos += size;
		writeSize += size;
		dst += size;
		requestSize -= size;
	}
	return writeSize;
}

static int calc_cluster(FDHANDLE* fh, int newpos) {
	/* 计算簇 */
	int clusterCount = newpos / CLUSTER_SIZE - fh->pos / CLUSTER_SIZE;
	int cluster = fh->cluster;
	int i;
	if (newpos < fh->pos) {  // If the new position is backward,
		// Then search target cluster from the top.
		cluster = fh->finfo->clustno;
		clusterCount = newpos / CLUSTER_SIZE;
	}

	for (i = 0; i < clusterCount; ++i)
		cluster = get_next_cluster(cluster);
	return cluster;
}

void fd_seek(FDHANDLE* fh, int offset, int origin) {
	/* 选择软盘 */
	int newpos = fh->pos;
	switch (origin) {
	case 0:  newpos = offset; break;
	case 1:  newpos += offset; break;
	case 2:  newpos = fh->finfo->size + offset; break;
	}
	if (newpos < 0)
		newpos = 0;
	else if (newpos > (int)fh->finfo->size)
		newpos = fh->finfo->size;
	fh->cluster = calc_cluster(fh, newpos);
	fh->pos = newpos;
}
