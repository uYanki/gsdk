#ifndef __SHELL_CONF_H__
#define __SHELL_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define CONFIG_SHELL_PARAMETER_MAX_COUNT 4
#define CONFIG_SHELL_HISTORY_MAX_COUNT   4
#define CONFIG_SHELL_PRINTF_BUFSIZE      128

#define CONFIG_SHELL_HASH_MATCH_SW       0  // 哈希匹配
#define CONFIG_SHELL_CMD_HASH_SEED       0xB433E5C

#define CONFIG_SHELL_FILE_SYSTEM_SW      0  // 文件系统

//

#define SHELL_HISTORY_SW (CONFIG_SHELL_HISTORY_MAX_COUNT > 0)

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
