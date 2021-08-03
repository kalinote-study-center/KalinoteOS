/* 关机 */
#include "../bootpack.h"

void cmd_shutdown(void){
	/* 关机 */
	acpi_shutdown();
}
