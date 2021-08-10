/* PCI设备驱动 */
#include "../bootpack.h"

void pci_configaddress (unsigned int Bus,unsigned int f,unsigned int equipment,unsigned int addr){
	/* 写cf8寄存器 */
	unsigned int cmd = 0;
	cmd = 0x80000000 + (unsigned int)addr + ((unsigned int)f << 8) + ((unsigned int)equipment << 11) + ((unsigned int)Bus << 16);
	//cmd = cmd | 0x01;
	io_out32(0xcf8,cmd);
}

struct PCI_INFO pci_init(void){
	/* 枚举PCI设备，并在内存中建立一张设备表(参数adder_Base表的起始地址) */
	unsigned int i,BUS,Equipment,F,ADDER,*i1;
	unsigned char *PCI_DATA = PCI_DEVICE_INFO_ADDR;
	unsigned char *PCI_DATA1;
	struct PCI_INFO pci_info;
	pci_info.start_addr = PCI_DEVICE_INFO_ADDR;
	for (BUS = 0;BUS < 256;BUS++) {//查询总线
		for (Equipment = 0;Equipment < 32;Equipment++) {//查询设备
			for (F = 0;F < 8;F++) {//查询功能
				pci_configaddress (BUS,F,Equipment,0);
				if (io_in32 (0xcfc) != 0xFFFFFFFF) {
					//当前插槽有设备
					//把当前设备信息映射到PCI数据区
					while (1) {
						//if (*PCI_DATA == 0x00) {
							//此配置表为空
							PCI_DATA1 = PCI_DATA;
							*PCI_DATA1 = 0xFF;//表占用标志
							PCI_DATA1++;
							*PCI_DATA1 = BUS;//总线号
							PCI_DATA1++;
							*PCI_DATA1 = Equipment;//设备号
							PCI_DATA1++;
							*PCI_DATA1 = F;//功能号
							PCI_DATA1++;
							PCI_DATA1 = PCI_DATA1 + 12;
							//写入寄存器配置
							for (ADDER = 0;ADDER < 256;ADDER = ADDER + 4) {
								pci_configaddress (BUS,F,Equipment,ADDER);
								i = io_in32 (0xcfc);
								i1 = i;
								*i1 = PCI_DATA1;
								PCI_DATA1 = PCI_DATA1 + 4;
							}
						//}
						PCI_DATA = PCI_DATA + 0x110;
						break;
					}
				}
			}
		}
	}
	// 函数执行完PCI_DATA就是PCI设备表的结束地址
	// 所有设备所在表地址位adder_Base - PCI_DATA
	pci_info.end_addr = PCI_DATA;
	return pci_info;
}
