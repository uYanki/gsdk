#ifndef __SHELL_CORE_H__
#define __SHELL_CORE_H__

#include "gsdk.h"
#include "shell_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define __SECTION(x) __attribute__((section(x)))

#if defined(__CC_ARM) || defined(__CLANG_ARM) || defined(__GNUC__)
#define __SECTION(x) __attribute__((section(x)))
#define __USED       __attribute__((used))
#elif defined(__IAR_SYSTEMS_ICC__)
#define __SECTION(x) @x
#define __USED       __root
#else
#error "unsupported toolchain"
#endif

#define CMD_ATTR_TYPE_Len   5
#define CMD_ATTR_ACCESS_Len 3

#define CMD_ATTR_TYPE_Pos   0
#define CMD_ATTR_ACCESS_Pos (CMD_ATTR_TYPE_Pos + CMD_ATTR_TYPE_Len)
#define CMD_ATTR_EXT_Pos    (CMD_ATTR_ACCESS_Len + CMD_ATTR_ACCESS_Pos)

#define CMD_ACCESS(n)       ((uint16_t)(n) << CMD_ATTR_ACCESS_Pos)
#define CMD_TYPE(n)         ((uint16_t)(n) << CMD_ATTR_TYPE_Pos)
#define CMD_FUNC_ARGC(n)    ((uint16_t)(n) << CMD_ATTR_EXT_Pos)
#define CMD_VAR_RW(n)       ((uint16_t)(n) << CMD_ATTR_EXT_Pos)

#define CMD_EXPORT_FUNC(_attr, _name, _cbk, _desc)  \
    static const char _cmd_name_##_name[] = #_name; \
    static const char _cmd_desc_##_name[] = #_desc; \
    __SECTION("CMDS")                               \
    EXTERN_C __USED static cmd_t _cmd_##_name = {   \
        .Func.szName  = _cmd_name_##_name,          \
        .Func.pfnCbk  = (cmd_cbk_t)_cbk,            \
        .szDesc       = _cmd_desc_##_name,          \
        .uAttr.u16All = _attr,                      \
    };

#define CMD_EXPORT_VAR(_attr, _name, _addr, _desc)  \
    static const char _cmd_name_##_name[] = #_name; \
    static const char _cmd_desc_##_name[] = #_desc; \
    __SECTION("CMDS")                               \
    EXTERN_C __USED static cmd_t _cmd_##_name = {   \
        .Var.szName   = _cmd_name_##_name,          \
        .Var.pAddr    = (void*)_addr,               \
        .szDesc       = _cmd_desc_##_name,          \
        .uAttr.u16All = _attr,                      \
    };

// hotkey
#define CMD_EXPORT_KEY(_attr, _key, _cbk, _desc)   \
    static const char _cmd_desc_##_key[] = #_desc; \
    __SECTION("KEYS")                              \
    __USED static cmd_t _cmd_##_key = {            \
        .Key.u32Code  = _key,                      \
        .Key.pfnCbk   = (key_cbk_t)_cbk,           \
        .szDesc       = _cmd_desc_##_key,          \
        .uAttr.u16All = _attr,                     \
    };

typedef enum {
    CMD_FUNC_STYLE_C,
    CMD_FUNC_STYLE_MAIN,
    CMD_VAR_TYPE_STRING,
    CMD_VAR_TYPE_INT8,
    CMD_VAR_TYPE_INT16,
    CMD_VAR_TYPE_INT32,
    CMD_VAR_TYPE_INT64,
    CMD_VAR_TYPE_UINT8,
    CMD_VAR_TYPE_UINT16,
    CMD_VAR_TYPE_UINT32,
    CMD_VAR_TYPE_UINT64,
    CMD_VAR_TYPE_FLOAT32,
    CMD_VAR_TYPE_FLOAT64,
    CMD_HOTKEY,
} cmd_type_e;

typedef enum {
    CMD_VAR_RO,
    CMD_VAR_WO,
    CMD_VAR_RW,
} cmd_var_rw_e;

typedef enum {
    CMD_ACCESS_USER,
    CMD_ACCESS_ADMIN,
    CMD_ACCESS_SUPER,
} cmd_access_e;

typedef enum {
    SHELL_INPUT_MODE_INSERT,
    SHELL_INPUT_MODE_REPLACE,
} shell_input_mode_e;

typedef enum {
    SHELL_CURSOR_PREV_CHAR,   // 光标前
    SHELL_CURSOR_CURR_CHAR,   // 光标处
    SHELL_CURSOR_NEXT_CHAR,   // 光标后
    SHELL_CURSOR_LINE_BEGIN,  // 行首
    SHELL_CURSOR_LINE_END,    // 行尾
} shell_cursor_e;

typedef struct shell shell_t;
// typedef int (*cmd_cbk_t)(shell_t* shell, uint8_t argc, char** argv);
typedef int (*cmd_cbk_t)();
typedef void (*key_cbk_t)(shell_t* shell);

typedef struct {
#if CONFIG_HASH_MATCH_SW
    volatile uint32_t u32Hash;  // !! 必须用 volatile 修饰
#endif

    const union {
        struct {
            const char* szName;
            cmd_cbk_t   pfnCbk;
        } Func;

        struct {
            const char* szName;
            void*       pAddr;
        } Var;

        struct {
            uint32_t  u32Code;
            key_cbk_t pfnCbk;
        } Key;
    };

    const char* szDesc;

    const union {
        uint16_t u16All;

        struct {
            cmd_type_e   eType : CMD_ATTR_TYPE_Len;
            cmd_access_e eAccess : CMD_ATTR_ACCESS_Len;

            union {
                cmd_var_rw_e eRW;
                uint8_t      argc;
            };

        } Bit;
    } uAttr;

} cmd_t;

struct shell {
    __IN shell_input_mode_e eInputMode;

    __IN uint8_t* pu8Buffer;
    __IN uint16_t u16LineMaxSize;  // unit: bytes

    __IN const char* szPrompt;

    uint16_t u16CmdSize;  // current line size, unit: bytes
    uint16_t u16CursorPos;

    uint32_t u32KeySeq;  // hotkey

    struct {
        cmd_t*   pTable;
        uint16_t u16Count;
    } aCmds, aKeys;

    /**
     * argv[0]: cmd szName
     * argv[1...]: args
     */
    uint8_t argc;
    char*   argv[CONFIG_SHELL_PARAMETER_MAX_COUNT + 1];

#if SHELL_HISTORY_SW

    char*    pu8HistoryBuff[CONFIG_SHELL_HISTORY_MAX_COUNT];
    uint16_t u16HistoryRecord;  // writer
    int16_t  s16HistoryOffset;  // reader
    uint16_t u16HistoryCount;

#endif

    __IMPL bool (*getc)(char* ch);
    __IMPL bool (*putc)(char ch);
    __IMPL bool (*puts)(char* str);
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

extern void Shell_Init(shell_t* pShell);
extern void Shell_Cycle(shell_t* pShell);
extern int  Shell_Printf(shell_t* pShell, const char* format, ...);
extern void Shell_ExecCmd(shell_t* pShell, const char* szCmd);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
