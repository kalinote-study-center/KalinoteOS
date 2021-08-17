/* C语言标准函数 stdlib.h system */
#include <stdlib.h>
#include <string.h>
#include "../apilib.h"

int system(const char *command) {
	/* 注意：该函数可用性未知 */
	/* 注意：该函数功能尚不完善 */
	char *cmd = malloc(strlen(command) + 1);
	strcpy(cmd, command);
	api_send_key_to_cmd(cmd);
	free(cmd);
	return 0;
}
