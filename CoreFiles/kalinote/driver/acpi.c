/* ACPI电源控制，实现关机 */
/* https://wiki.osdev.org/ACPI */
#include "../bootpack.h"
#include <stdio.h>
#include <string.h>

struct ACPI_RSDP *RSDP;
struct ACPI_RSDT *RSDT;
struct ACPI_FADT *FADT;

char checksum(unsigned char *addr, unsigned int length){
	/* 检查校验和(checksum)是否有效 */
    int i = 0;
    unsigned char sum = 0;
 
    for (i = 0; i < length; i++){
        sum += ((unsigned char *) addr)[i];
    }
 
    return sum == 0;
}

unsigned int *acpi_find_rsdp(void){
	/* https://wiki.osdev.org/RSDP */
	/* 查找RSDP(Root System Description Pointer) */
	/* RSDP一般位于EBDA(Extended BIOS Data Area)的前1K，或者0x000E0000到0x000FFFFF的内存区域 */
	/* 如果使用UEFI，则应该使用EFI_SYSTEM_TABLE查找 */
	unsigned int *addr;
	
	for (addr = (unsigned int *)0x000e0000; addr < (unsigned int *)0x00100000; addr++){
		/* 从内存0x000E0000到0x000FFFFF的位置找到"RSD PTR "字符串 */
		/* 该签名始终位于16字节的边界上 */
		if (memcmp(addr, "RSD PTR ", 8) == 0){
			/* 如果找到RSDP */
			if (checksum((unsigned char *)addr, ((struct ACPI_RSDP *)addr)->Length)){
				return addr;
			}
		}
	}
	return 0;
}

unsigned int acpi_find_table(char *Signature){
	int i, length = (RSDT->header.Length - sizeof(RSDT->header))/4;
	struct ACPISDTHeader *header;
	for (i = 0; i < length; i++){
		header = (struct ACPISDTHeader *)((unsigned int)RSDT->Entry + i*4), sizeof(struct ACPISDTHeader);
		if (memcmp(header->Signature, Signature, 4) == 0){
			return (unsigned int)header;
		}
	}
    return 0;
}

/*
*     由 ACPI 定义(机翻)	// https://wiki.osdev.org/RSDT
*   ACPI 规范 5.0 对以下系统描述表有直接定义，由其签名识别：
* "APIC"：多个 APIC 描述表（MADT）)
* "BERT"：引导错误记录表（BERT）)
* "CPEP"：更正平台错误投票表（CPEP）)
* "DSDT"：差异化系统描述表（DSDT）)
* "ECDT"：嵌入式控制器启动资源表（ECDT）)
* "EINJ"：错误注射表（EINJ）)
* "ERST"：错误记录序列化表（ERST）)
* "FACP"：固定的 ACPI 描述表（FADT）)
* "FACS"：固件 ACPI 控制结构（FACS）)
* "HEST"：硬件错误源表（HEST)
* "MSCT"：最大系统特征表（MSCT）)
* "MPST"：内存功率状态表（MPST）)
* "OEMx"：OEM 特定信息表（任何以"OEM"开头的签名表都属于此定义）
* "PMTT"：平台内存拓扑表 （PMTT)
* "PSDT"：持久系统描述表（PSDT）)
* "RASF"：ACPI拉斯功能表（RASF）)
* "RSDT"：根系统描述表（本维基页面;完整性包括在内）
* "SBST"：智能电池规格表（SBST）)
* "SLIT"：系统局部系统信息表（SLIT）)
* "SRAT"：系统资源亲和力表)
* "SSDT"：二级系统描述表（SSDT）)
* "XSDT"：扩展系统描述表（XSDT：64位版本的 RSDT）
*/

void init_acpi(void){
	/* ACPI功能初始化 */
	/* 检查RSDP */
	RSDP = (struct ACPI_RSDP *)acpi_find_rsdp();
	if (RSDP == 0) return;		/* 未找到RSDP，无法使用ACPI */
	
	/* 检查RSDT */
	RSDT = (struct ACPI_RSDT *)RSDP->RsdtAddress;
	if (!checksum((unsigned char *)RSDT, RSDT->header.Length)) return;		/* RSDT校验不通过 */
	
	/* 检查FADT(签名为"FACP") */
	FADT = (struct ACPI_FADT *)acpi_find_table("FACP");
	if (!checksum((unsigned char *)FADT, FADT->h.Length)) return;
	
	/* 切换到ACPI模式 */
	if (!(io_in16(FADT->PM1aControlBlock) & 1)){
		if (FADT->SMI_CommandPort && FADT->AcpiEnable){
			io_out8(FADT->SMI_CommandPort, FADT->AcpiEnable);
			int i, j;
			for (i = 0; i < 300; i++){
				if (io_in16(FADT->PM1aControlBlock) & 1) break;
                for (j = 0; j < 1000000; j++);
			}
			if (FADT->PM1bControlBlock){
				for (; i < 300; i++){
					if (io_in16(FADT->PM1bControlBlock) & 1) break;
                for (j = 0; j < 1000000; j++);
				}
			}
		}
	}
	return;
}

/*
 * \_S5 Object 
 * -----------------------------------
 * NameOP | \(可选) | _  | S  | 5  | _
 * 08     | 5A      | 5F | 53 | 35 | 5F
 * -----------------------------------
 * PackageOP | PkgLength | NumElements | prefix Num | prefix Num | prefix Num | prefix Num
 * 12        | 0A        | 04          | 0A     05  | 0A     05  | 0A     05  | 0A     05
 * -----------------------------------
 * PkgLength: bit6~7为长度的字节数-1;bit4~5保留;bit0~3为长度的低4位
 * prefix:	0A Byte
 * 			0B Word
 * 			0C DWord
 * 			0D String
 * 			0E Qword
*/

int acpi_shutdown(void){
	/* 通过ACPI实现关机 */
	/* https://wiki.osdev.org/Shutdown */
	int i;
	unsigned short SLP_TYPa = 0;
	unsigned short SLP_TYPb = 0;
	struct ACPISDTHeader *header = (struct ACPISDTHeader *)acpi_find_table("DSDT");
	char *S5Addr = (char *)header;
	int dsdtLength = (header->Length - sizeof(struct ACPISDTHeader))/4;
	
	for(i = 0; i < dsdtLength; i++){
		if (memcmp(S5Addr, "_S5_", 4) == 0) break;
		S5Addr++;
	}
	if (i < dsdtLength){
		if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 ){
			S5Addr+=5;
			S5Addr+=((*S5Addr&0xc0)>>6)+2;
			
			if (*S5Addr == 0x0a) S5Addr++;
			SLP_TYPa = *(S5Addr)<<10;
			S5Addr++;
			
			if (*S5Addr == 0x0a) S5Addr++;
			SLP_TYPb = *(S5Addr)<<10;
			S5Addr++;
		}
		// 关于PM1x_CNT_BLK的描述见 ACPI Specification Ver6.3 4.8.3.2.1
		io_out16(FADT->PM1aControlBlock, SLP_TYPa | 1<<13);
		if (FADT->PM1bControlBlock != 0){
			io_out16(FADT->PM1bControlBlock, SLP_TYPb | 1<<13);
		}
	}
    return 1;
}

int acpi_reboot(void) {
	/* 通过ACPI实现重启 */
	/*
	* (这里的重启是通过判断系统环境选择使用I/O总线还是PCI总线，
	* 但是因为PCI驱动还没有实现，所以暂时和acpi_reset功能相同，
	* 但在调用使用重启功能时，应该使用该函数)
	*/
	/* https://wiki.osdev.org/Reboot */
	/* https://elixir.bootlin.com/linux/latest/source/drivers/acpi/reboot.c */

	/* 判断ACPI版本 */
	debug_print("ACPI>ACPI version:%d\n",FADT->h.Revision);
	if(FADT->h.Revision < 2) {
		debug_print("ACPI>Unsupported ACPI version:%d\n",FADT->h.Revision);
		return 0;
	}

	/* Linux系统在这里验证了是否支持reset寄存器(通过检查位宽度和位偏移)，这里暂时跳过 */
	
	/* 
	* 复位寄存器只能存在于I/O、内存或PCI配置空间中，在总线0上的设备上
	*/
	
	/*
	* 这里选择是通过PCI总线配置空间还是I/O和内存实现重启
	* 关于PCI总线的驱动，只能后面有空再去做
	*/
	acpi_reset();
	return 1;
}

int acpi_reset(void) {
	/* 检查是否支持reset寄存器 */

	if (!(FADT->Flags & ACPI_FADT_RESET_REGISTER) || !(FADT->ResetReg.Address)) {
		/* 不支持则进行返回 */
		// debug_print("ACPI>Unsupported ResetReg\n");
		// debug_print("ACPI>FADT Flags = 0x%08x\n", FADT->Flags);
		// debug_print("ACPI>FADT ResetReg Address[0] = 0x%08x\n", FADT->ResetReg.Address[0]);
		// debug_print("ACPI>FADT ResetReg Address[1] = 0x%08x\n", FADT->ResetReg.Address[1]);
		// debug_print("ACPI>FADT ResetReg Address[2] = 0x%08x\n", FADT->ResetReg.Address[2]);
		return 0;
	}

	/* ACPI通过I/O总线实现reset */
	debug_print("APIC>reset by I/O\n");
	io_out8((unsigned int)(FADT->ResetReg.Address), FADT->ResetValue);

	return 1;
}
