#include <stdarg.h>
#include <string.h>

#include "endian.h"

#include "shell_core.h"
#include "shell_keys.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "shell"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#ifndef LINEEND
#define LINEEND "\r\n"
#endif

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static inline bool        _IsFunc(cmd_t* pCmd);
static inline bool        _IsVar(cmd_t* pCmd);
static inline const char* _GetCmdName(cmd_t* pCmd);
static inline const char* _GetCmdDesc(cmd_t* pCmd);

static uint32_t _GenerateHash(const char* szCmd);

static bool   _Shell_KeyScan(shell_t* pShell, uint8_t u8Data);
static void   _Shell_InsertChar(shell_t* pShell, uint8_t u8Data);
static cmd_t* _Shell_SeekCmd(shell_t* pShell, const char* szCmd);
static bool   _Shell_ParseCmd(shell_t* pShell);
static void   _Shell_ExecCmd(shell_t* pShell);

static void _Shell_ShowPrompt(shell_t* pShell);
static void _Shell_ClearLine(shell_t* pShell);
static void _Shell_ClearScreen(shell_t* pShell);

#if SHELL_HISTORY_SW
static void _Shell_HistoryAppend(shell_t* pShell);
static void _Shell_HistorySwitch(shell_t* pShell, bool bDir);
static void _Shell_ShowHistory(shell_t* pShell);
#endif

static void _Shell_ShowCmd(shell_t* pShell, cmd_t* pCmd);
static void _Shell_ShowVar(shell_t* pShell, cmd_t* pCmd);

void Shell_ListAll(shell_t* pShell);
void Shell_ListCmds(shell_t* pShell);
void Shell_ListVars(shell_t* pShell);

static void _ShellKey_Delete(shell_t* pShell);
static void _ShellKey_BackSpace(shell_t* pShell);
static void _ShellKey_ExecCmd(shell_t* pShell);
static void _ShellKey_MoveCursorToPrevChar(shell_t* pShell);
static void _ShellKey_MoveCursorToNextChar(shell_t* pShell);
static void _ShellKey_MoveCursorToLineBegin(shell_t* pShell);
static void _ShellKey_MoveCursorToLineEnd(shell_t* pShell);
static void _ShellKey_TabComplete(shell_t* pShell);
static void _ShellKey_DeleteToEnd(shell_t* pShell);

static void _ShellCmd_ShowHelp(shell_t* pShell, int argc, char* argv[]);
static void _ShellCmd_Echo(shell_t* pShell, int argc, char* argv[]);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline bool _IsFunc(cmd_t* pCmd)
{
    return (CMD_FUNC_STYLE_C <= pCmd->uAttr.Bit.eType) && (pCmd->uAttr.Bit.eType <= CMD_FUNC_STYLE_MAIN);
}

static inline bool _IsVar(cmd_t* pCmd)
{
    return (CMD_VAR_TYPE_STRING <= pCmd->uAttr.Bit.eType) && (pCmd->uAttr.Bit.eType <= CMD_VAR_TYPE_FLOAT64);
}

static inline const char* _GetCmdName(cmd_t* pCmd)
{
    if (_IsFunc(pCmd))
    {
        if (pCmd->Func.szName != nullptr)
        {
            return pCmd->Func.szName;
        }
    }
    else if (_IsVar(pCmd))
    {
        if (pCmd->Var.szName != nullptr)
        {
            return pCmd->Var.szName;
        }
    }

    return "";
}

static inline const char* _GetCmdDesc(cmd_t* pCmd)
{
    return pCmd->szDesc == nullptr ? "" : pCmd->szDesc;
}

#if CONFIG_SHELL_HASH_MATCH_SW

static inline uint32_t _GenerateHash(const char* szCmd)
{
    uint32_t u32Seed = CONFIG_SHELL_CMD_HASH_SEED;
    uint32_t u32Hash = 0;

    while (*szCmd)
    {
        u32Hash ^= u32Seed;
        u32Hash += *szCmd++;
    }

    return u32Hash;
}

#endif

//-------------------------------------

static int _Shell_CallFuncCbk(shell_t* pShell, cmd_t* pCmd)
{
    if (pCmd->uAttr.Bit.eType == CMD_FUNC_STYLE_MAIN)
    {
        int ret = pCmd->Func.pfnCbk(pShell, pShell->argc, pShell->argv);
    }
    else if (pCmd->uAttr.Bit.eType == CMD_FUNC_STYLE_C)
    {
        uint8_t argc = pCmd->uAttr.Bit.argc;

        if (argc != (pShell->argc - 1))
        {
            pShell->puts("Too few or too many arguments. ");
            Shell_Printf(pShell, "Only %d parameters are needed.\r\n", argc);
            goto _error;
        }

        uint32_t args[CONFIG_SHELL_PARAMETER_MAX_COUNT] = {0};

        for (uint8_t i = 0; i < argc; i++)
        {
            // cmd_parse_arg(pShell->argv[i + 1], &args[i]);
        }

        switch (argc)
        {
            case 0 : return pCmd->Func.pfnCbk(pShell);
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 0
            case 1 : return pCmd->Func.pfnCbk(pShell, args[0]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 1
            case 2 : return pCmd->Func.pfnCbk(pShell, args[0], args[1]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 2
            case 3 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 3
            case 4 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 4
            case 5 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 5
            case 6 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4], args[5]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 6
            case 7 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 7
            case 8 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 8
            case 9 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 9
            case 10 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 10
            case 11 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 11
            case 12 : return pCmd->Func.pfnCbk(pShell, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11]);
#endif
#if CONFIG_SHELL_PARAMETER_MAX_COUNT > 12
#warning "too many parameters"
#endif
            default : break;
        }
    }

_error:
    return -1;
}

static bool _Shell_KeyScan(shell_t* pShell, uint8_t u8Data)
{
    uint32_t u32KeyMask;
    uint8_t  u8BitShift;

    if (pShell->u32KeySeq & 0x0000FF00)
    {
        u8BitShift = 0;
        u32KeyMask = 0xFFFFFF00;
    }
    else if (pShell->u32KeySeq & 0x00FF0000)
    {
        u8BitShift = 8;
        u32KeyMask = 0xFFFF0000;
    }
    else if (pShell->u32KeySeq & 0xFF000000)
    {
        u8BitShift = 16;
        u32KeyMask = 0xFF000000;
    }
    else
    {
        u8BitShift = 24;
        u32KeyMask = 0x00000000;
    }

    for (uint16_t i = 0; i < pShell->aKeys.u16Count; ++i)
    {
        uint32_t u32HotKey = pShell->aKeys.pTable[i].Key.u32Code;

        if ((u32HotKey & u32KeyMask) == pShell->u32KeySeq)
        {
            if (u8Data > 0)
            {
                if (GETBITS(u32HotKey, u8BitShift, 8) == u8Data)
                {
                    pShell->u32KeySeq |= u8Data << u8BitShift;
                    u8Data = 0x00;
                }
                else
                {
                    continue;
                }
            }

            // check if matched (or):
            // 1. current byte the last byte
            // 2. the next byte is dummy
            if ((u8BitShift == 0) || (GETBITS(u32HotKey, u8BitShift - 8, 8) == 0x00))
            {
                // matched !!!
                pShell->u32KeySeq = 0x00000000;

                // call key handler
                pShell->aKeys.pTable[i].Key.pfnCbk(pShell);

                LOGD("matched Key! 0x%08X", u32HotKey);

                break;
            }
        }
    }

    return u8Data == 0x00;
}

static void _Shell_InsertChar(shell_t* pShell, uint8_t u8Data)
{
    if (pShell->u16CmdSize >= (pShell->u16LineMaxSize - 1))
    {
        pShell->puts("\r\nWarning: Command is too long\r\n");
        _Shell_ShowPrompt(pShell);
        pShell->puts(pShell->pu8Buffer);
        return;
    }

    if (pShell->u16CursorPos == pShell->u16CmdSize)
    {
        // clang-format off
        pShell->pu8Buffer[pShell->u16CursorPos++] = u8Data;
        pShell->u16CmdSize = pShell->u16CursorPos;
        pShell->pu8Buffer[pShell->u16CmdSize]   = '\0';
        // clang-format on
        pShell->putc(u8Data);
    }
    else if (pShell->u16CursorPos < pShell->u16CmdSize)
    {
        // insert mode

        if (pShell->eInputMode == SHELL_INPUT_MODE_INSERT)
        {
            uint16_t i;

            for (i = pShell->u16CmdSize - pShell->u16CursorPos; i > 0; i--)
            {
                // move back
                pShell->pu8Buffer[pShell->u16CursorPos + i] = pShell->pu8Buffer[pShell->u16CursorPos + i - 1];
            }

            pShell->pu8Buffer[pShell->u16CursorPos++] = u8Data;
            pShell->pu8Buffer[++pShell->u16CmdSize]   = 0;

            pShell->puts(&pShell->pu8Buffer[pShell->u16CursorPos - 1]);

            for (i = pShell->u16CmdSize - pShell->u16CursorPos; i > 0; i--)
            {
                pShell->putc('\b');
            }
        }
    }
}

static cmd_t* _Shell_SeekCmd(shell_t* pShell, const char* szCmd)
{
    cmd_t* pTable = pShell->aCmds.pTable;

#if CONFIG_SHELL_HASH_MATCH_SW
    uint32_t u32Hash = _GenerateHash(szCmd);
#endif

    for (uint16_t i = 0; i < pShell->aCmds.u16Count; i++)
    {
#if CONFIG_SHELL_HASH_MATCH_SW
        if (u32Hash == pTable[i].u32Hash)
#endif
        {
            if (strncmp(szCmd, _GetCmdName(&pTable[i]), pShell->u16LineMaxSize) == 0)
            {
                return &pTable[i];
            }
        }
    }

    return nullptr;
}

static bool _Shell_ParseCmd(shell_t* pShell)
{
    struct {
        uint8_t inArg   : 1;
        uint8_t inQuote : 1;
        uint8_t done    : 1;
    } state = {0};

    uint16_t i = 0;

    char*    pBuff  = pShell->pu8Buffer;
    uint16_t u16Len = pShell->u16CmdSize;

    uint16_t argc = 0;
    char**   argv = pShell->argv;

    for (; !state.done & (argc <= CONFIG_SHELL_PARAMETER_MAX_COUNT); ++i)
    {
        switch (pBuff[i])
        {
            case '\0' :  // end
            {
                if (state.inQuote)
                {
                    goto __error;
                }
                state.done = 1;
                break;
            }

            case '"' :  // str
            {
                if (state.inQuote)
                {
                    if (state.inArg)
                    {
                        // end of string
                        state.inQuote = 0;
                        goto __end_arg;
                    }
                    else
                    {
                        goto __error;
                    }
                }
                else if (!state.inArg)
                {
                    // beginning of string arg
                    state.inArg = state.inQuote = 1;
                    argv[argc++]                = &pBuff[i + 1];
                }

                break;
            }

            case ' ' :
            {
                if (state.inArg)
                {
                    if (!state.inQuote)
                    {
__end_arg:
                        state.inArg = 0;
                        pBuff[i]    = '\0';  // end of arg
                    }
                }
                break;
            }

            case '\\' :
            {
                switch (pBuff[i + 1])
                {
                    case '"' :
                        // next char isn't a delimiter
                        pBuff[i] = '"';
                        break;
                    case ' ' :
                        // next char isn't a delimiter
                        pBuff[i] = ' ';
                        break;
                    case 'a' : pBuff[i] = '\a'; break;
                    case 'b' : pBuff[i] = '\b'; break;
                    case 'r' : pBuff[i] = '\r'; break;
                    case 'n' : pBuff[i] = '\n'; break;
                    case 't' : pBuff[i] = '\t'; break;
                    case '\\' : pBuff[i] = '\\'; break;
                    case '0' :
                    {
                        pBuff[i--] = '\0';
                        continue;
                    }
                    case '\0' :
                    default :
                    {
                        continue;
                    }
                }

                strcpy(&pBuff[i + 1], &pBuff[i + 2]);

                break;
            }
            default :
            {
                if (!state.inArg)
                {
                    // beginning of arg
                    state.inArg  = 1;
                    argv[argc++] = &pBuff[i];
                }
                break;
            }
        }
    }

    if (state.inQuote)
    {
        goto __error;
    }

    pShell->argc = argc;

    return true;

__error:

    pShell->argc = 0;  // ERR_SYNTAX

    for (uint16_t i = pShell->argc; i <= CONFIG_SHELL_PARAMETER_MAX_COUNT; i++)
    {
        pShell->argv[i] = nullptr;
    }

    pShell->puts("syntax error\r\n");

    return false;
}

static void _Shell_ExecCmd(shell_t* pShell)
{
    pShell->puts(LINEEND);

    if (pShell->u16CmdSize == 0)
    {
        return;
    }

#if SHELL_HISTORY_SW
    _Shell_HistoryAppend(pShell);
#endif

    if (_Shell_ParseCmd(pShell) == false)
    {
        return;
    }

    cmd_t* pCmd = _Shell_SeekCmd(pShell, pShell->argv[0]);

    LOGV("%s", pShell->argv[0]);

    if (pCmd == nullptr)
    {
        pShell->puts("Command not Found: ");
        pShell->putc('\"');
        pShell->puts(pShell->pu8Buffer);
        pShell->putc('\"');
        pShell->puts(LINEEND);
    }
    else
    {
        if (_IsFunc(pCmd))
        {
            _Shell_CallFuncCbk(pShell, pCmd);
        }
        else if (_IsVar(pCmd))
        {
            _Shell_ShowVar(pShell, pCmd);
        }
    }

    pShell->u16CmdSize = pShell->u16CursorPos = 0;
}

static void _Shell_MoveCursor(shell_t* pShell, shell_cursor_e ePostion)
{
    switch (ePostion)
    {
        case SHELL_CURSOR_PREV_CHAR :
        {
            if (pShell->u16CursorPos > 0)
            {
                pShell->u16CursorPos--;
                pShell->putc('\b');
            }

            break;
        }

        case SHELL_CURSOR_CURR_CHAR :
        {
            break;
        }

        case SHELL_CURSOR_NEXT_CHAR :
        {
            if (pShell->u16CursorPos < pShell->u16CmdSize)
            {
                pShell->putc(pShell->pu8Buffer[pShell->u16CursorPos++]);
            }

            break;
        }

        case SHELL_CURSOR_LINE_BEGIN :
        {
            // move cursor to start of the line

            while (pShell->u16CursorPos)
            {
                pShell->putc('\b');
                pShell->u16CursorPos--;
            }

            break;
        }

        case SHELL_CURSOR_LINE_END :
        {
            // move cursor to end of the line

            if (pShell->u16CursorPos < pShell->u16CmdSize)
            {
                pShell->puts(&pShell->pu8Buffer[pShell->u16CursorPos]);
                pShell->u16CursorPos = pShell->u16CmdSize;
            }

            break;
        }

        default :
        {
            break;
        }
    }
}

static void _Shell_RemoveChar(shell_t* pShell, shell_cursor_e ePostion)
{
    switch (ePostion)
    {
        case SHELL_CURSOR_PREV_CHAR :
        {
            if (pShell->u16CursorPos == 0)
            {
                return;
            }

            break;
        }

        case SHELL_CURSOR_CURR_CHAR :
        {
            if (pShell->u16CursorPos == pShell->u16CmdSize)
            {
                return;
            }

            break;
        }

        case SHELL_CURSOR_NEXT_CHAR :
        case SHELL_CURSOR_LINE_BEGIN :
        case SHELL_CURSOR_LINE_END :
        {
            return;
        }

        default :
        {
            return;
        }
    }

    if (pShell->u16CursorPos == pShell->u16CmdSize)
    {
        pShell->u16CmdSize--;
        pShell->u16CursorPos--;
        pShell->pu8Buffer[pShell->u16CmdSize] = '\0';
        pShell->puts("\b \b");
    }
    else
    {
        uint8_t i, offset;

        if (pShell->eInputMode == SHELL_INPUT_MODE_INSERT)
        {
            offset = (ePostion == SHELL_CURSOR_CURR_CHAR) ? 1 : 0;
        }
        else if (pShell->eInputMode == SHELL_INPUT_MODE_REPLACE)
        {
            offset = (ePostion == SHELL_CURSOR_PREV_CHAR) ? 1 : 0;
        }

        for (i = offset + pShell->u16CursorPos; i < pShell->u16CmdSize; i++)
        {
            // move forward
            pShell->pu8Buffer[i - 1] = pShell->pu8Buffer[i];
        }

        pShell->u16CmdSize--;

        if (!offset)
        {
            pShell->u16CursorPos--;
            pShell->putc('\b');
        }

        pShell->pu8Buffer[pShell->u16CmdSize] = '\0';

        pShell->puts(&pShell->pu8Buffer[pShell->u16CursorPos]);

        // the last char
        pShell->putc(' ');

        for (i = pShell->u16CmdSize - pShell->u16CursorPos + 1; i > 0; i--)
        {
            pShell->putc('\b');
        }
    }
}

static void _Shell_ShowPrompt(shell_t* pShell)
{
    pShell->puts(pShell->szPrompt);
}

static void _Shell_ClearLine(shell_t* pShell)
{
    pShell->puts("\33[2K\r");  // delete the entire line
    pShell->pu8Buffer[pShell->u16CmdSize = pShell->u16CursorPos = 0] = '\0';
}

static void _Shell_ClearScreen(shell_t* pShell)
{
    pShell->puts("\e[2J\e[1H");
}

//-------------------------------------

void Shell_Init(shell_t* pShell)
{
    pShell->u16CmdSize   = 0U;
    pShell->u16CursorPos = 0U;
    pShell->u32KeySeq    = 0U;

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)

    extern const uint32_t CMDS$$Base;
    extern const uint32_t CMDS$$Limit;

    pShell->aCmds.pTable   = (cmd_t*) (&CMDS$$Base);
    pShell->aCmds.u16Count = ((u32) &CMDS$$Limit - (u32) &CMDS$$Base) / sizeof(cmd_t);

    extern const uint32_t KEYS$$Base;
    extern const uint32_t KEYS$$Limit;

    pShell->aKeys.pTable   = (cmd_t*) (&KEYS$$Base);
    pShell->aKeys.u16Count = ((u32) &KEYS$$Limit - (u32) &KEYS$$Base) / sizeof(cmd_t);

#elif defined(__ICCARM__) || defined(__ICCRX__)

    pShell->aCmds.pTable   = (cmd_t*) (__section_begin("CMDS"));
    pShell->aCmds.u16Count = ((u32) (__section_end("CMDS")) - (u32) (__section_begin("CMDS"))) / sizeof(cmd_t);

    pShell->aCmds.pTable   = (cmd_t*) (__section_begin("KEYS"));
    pShell->aCmds.u16Count = ((u32) (__section_end("KEYS")) - (u32) (__section_begin("KEYS"))) / sizeof(cmd_t);

#elif defined(__GNUC__)

    aCmds.pTable   = (cmd_t*) (&_shell_command_start);
    aCmds.u16Count = ((u32) (&_shell_command_end) - (u32) (&_shell_command_start)) / sizeof(cmd_t);

#else

#error not supported compiler, please use command table mode.

#endif

#if SHELL_HISTORY_SW

    pShell->s16HistoryOffset = 0;
    pShell->u16HistoryCount  = 0;
    pShell->u16HistoryRecord = 0;

    char* p = pShell->pu8Buffer;

    for (uint16_t i = 0; i < CONFIG_SHELL_HISTORY_MAX_COUNT; i++)
    {
        p += pShell->u16LineMaxSize;
        pShell->pu8HistoryBuff[i] = p;
    }

#endif

#if CONFIG_SHELL_HASH_MATCH_SW

    cmd_t* pTable = pShell->aCmds.pTable;

    for (uint16_t i = 0; i < pShell->aCmds.u16Count; i++)
    {
        pTable[i].u32Hash = _GenerateHash(_GetCmdName(&pTable[i]));

        LOGD(" hash = %d", pTable[i].u32Hash);
    }

#endif

    _Shell_ClearScreen(pShell);
    _Shell_ShowPrompt(pShell);
}

void Shell_Cycle(shell_t* pShell)
{
    char u8Data;

    if (pShell->getc(&u8Data))
    {
        if (_Shell_KeyScan(pShell, u8Data) == true)
        {
            return;
        }

        _Shell_InsertChar(pShell, u8Data);
    }
}

int Shell_Printf(shell_t* pShell, const char* format, ...)
{
    static char aBuffer[CONFIG_SHELL_PRINTF_BUFSIZE];

    va_list vargs;

    va_start(vargs, format);
    uint16_t u16Length = vsnprintf(aBuffer, CONFIG_SHELL_PRINTF_BUFSIZE - 2, format, vargs);
    va_end(vargs);

    aBuffer[CONFIG_SHELL_PRINTF_BUFSIZE - 1] = '\0';

    pShell->puts(aBuffer);

    return u16Length;
}

void Shell_ExecCmd(shell_t* pShell, const char* szCmd)
{
    char* pSrc = szCmd;
    char* pDst = pShell->pu8Buffer;

    uint16_t u16Count;

    for (u16Count = 0; u16Count < pShell->u16LineMaxSize && *pSrc; ++u16Count)  // strncpy
    {
        *pDst++ == *pSrc++;
    }

    pShell->u16CursorPos = pShell->u16CmdSize = u16Count;

    _Shell_ExecCmd(pShell);
}

#if SHELL_HISTORY_SW

static void _Shell_HistoryAppend(shell_t* pShell)
{
    pShell->s16HistoryOffset = 0;

    if (pShell->u16HistoryCount > 0)
    {
        if (strcmp(pShell->pu8HistoryBuff[(CONFIG_SHELL_HISTORY_MAX_COUNT + pShell->u16HistoryRecord - 1) % CONFIG_SHELL_HISTORY_MAX_COUNT], pShell->pu8Buffer) == 0)
        {
            // same as the last record
            return;
        }
    }

    strcpy(pShell->pu8HistoryBuff[pShell->u16HistoryRecord++], pShell->pu8Buffer);

    if (pShell->u16HistoryCount < CONFIG_SHELL_HISTORY_MAX_COUNT)
    {
        pShell->u16HistoryCount++;
    }

    if (pShell->u16HistoryRecord == CONFIG_SHELL_HISTORY_MAX_COUNT)
    {
        pShell->u16HistoryRecord = 0;
    }
}

static void _Shell_HistorySwitch(shell_t* pShell, bool bDir)
{
    if (bDir)  // true: prev
    {
        if (pShell->s16HistoryOffset == -pShell->u16HistoryCount)
        {
            return;
        }

        pShell->s16HistoryOffset--;
    }
    else  // false: next
    {
        if (pShell->s16HistoryOffset == 0)
        {
            return;
        }

        pShell->s16HistoryOffset++;
    }

    _Shell_ClearLine(pShell);
    _Shell_ShowPrompt(pShell);

    if (pShell->s16HistoryOffset == 0)
    {
        pShell->u16CursorPos = pShell->u16CmdSize = 0;
    }
    else
    {
        strcpy(pShell->pu8Buffer, pShell->pu8HistoryBuff[(CONFIG_SHELL_HISTORY_MAX_COUNT + pShell->u16HistoryRecord + pShell->s16HistoryOffset) % CONFIG_SHELL_HISTORY_MAX_COUNT]);
        pShell->u16CursorPos = pShell->u16CmdSize = strlen(pShell->pu8Buffer);
        pShell->puts(pShell->pu8Buffer);
    }
}

static void _Shell_ShowHistory(shell_t* pShell)
{
    for (uint16_t i = 0; i < pShell->u16HistoryCount; ++i)
    {
        pShell->puts(pShell->pu8HistoryBuff[i]);
        pShell->puts(LINEEND);
    }
}

#endif

static void _Shell_ShowCmd(shell_t* pShell, cmd_t* pCmd)
{
    Shell_Printf(pShell, "  %-20s\t%s", _GetCmdName(pCmd), _GetCmdDesc(pCmd));
    pShell->puts(LINEEND);
}

static void _Shell_ShowVar(shell_t* pShell, cmd_t* pCmd)
{
    if (pCmd->uAttr.Bit.eRW != CMD_VAR_WO)
    {
        void* pAddr = pCmd->Var.pAddr;

        pShell->puts(_GetCmdName(pCmd));

        // clang-format off
        switch (pCmd->uAttr.Bit.eType)
        {
            case CMD_VAR_TYPE_STRING:  { const char* pszStr = (const char*) pAddr; Shell_Printf(pShell, "\"%s\"", pszStr); break; }
            case CMD_VAR_TYPE_INT8:    { s16 s8Data  = he8s(pAddr);  Shell_Printf(pShell, "%d, 0x%X", s8Data,  s8Data);    break; }
            case CMD_VAR_TYPE_UINT8:   { u16 u8Data  = he8u(pAddr);  Shell_Printf(pShell, "%d, 0x%X", u8Data,  u8Data);    break; }
            case CMD_VAR_TYPE_INT16:   { s16 s16Data = he16s(pAddr); Shell_Printf(pShell, "%d, 0x%X", s16Data, s16Data);   break; }
            case CMD_VAR_TYPE_UINT16:  { u16 u16Data = he16u(pAddr); Shell_Printf(pShell, "%d, 0x%X", u16Data, u16Data);   break; }
            case CMD_VAR_TYPE_INT32:   { s32 s32Data = he32s(pAddr); Shell_Printf(pShell, "%d, 0x%X", s32Data, s32Data);   break; }
            case CMD_VAR_TYPE_UINT32:  { u32 u32Data = he32u(pAddr); Shell_Printf(pShell, "%d, 0x%X", u32Data, u32Data);   break; }
            case CMD_VAR_TYPE_INT64:   { s64 s64Data = he64s(pAddr); Shell_Printf(pShell, "%d, 0x%X", s64Data, s64Data);   break; }
            case CMD_VAR_TYPE_UINT64:  { u64 u64Data = he64u(pAddr); Shell_Printf(pShell, "%d, 0x%X", u64Data, u64Data);   break; }
            case CMD_VAR_TYPE_FLOAT32: { s32 s32Data = he32s(pAddr); Shell_Printf(pShell, "%f", *(f32*)(&s32Data));        break; }
            case CMD_VAR_TYPE_FLOAT64: { s64 s64Data = he64s(pAddr); Shell_Printf(pShell, "%lf", *(f64*)(&s64Data));       break; }
            default: { pShell->puts("unsupported type"); break; }
        }
        // clang-format on

        pShell->puts(LINEEND);
    }
}

void Shell_ListAll(shell_t* pShell)
{
    Shell_ListCmds(pShell);
    Shell_ListVars(pShell);
}

void Shell_ListCmds(shell_t* pShell)
{
    pShell->puts("Command List:");
    pShell->puts(LINEEND);

    for (uint16_t i = 0; i < pShell->aCmds.u16Count; i++)
    {
        if (_IsFunc(&pShell->aCmds.pTable[i]))
        {
            _Shell_ShowCmd(pShell, &pShell->aCmds.pTable[i]);
        }
    }
}

void Shell_ListVars(shell_t* pShell)
{
    pShell->puts("Variable List:\r\n");

    for (uint16_t i = 0; i < pShell->aCmds.u16Count; i++)
    {
        if (_IsVar(&pShell->aCmds.pTable[i]))
        {
            _Shell_ShowVar(pShell, &pShell->aCmds.pTable[i]);
        }
    }
}

//---------------------------------------------------------------------------

static void _ShellKey_Delete(shell_t* pShell)
{
    _Shell_RemoveChar(pShell, SHELL_CURSOR_CURR_CHAR);
}

static void _ShellKey_BackSpace(shell_t* pShell)
{
    _Shell_RemoveChar(pShell, SHELL_CURSOR_PREV_CHAR);
}

static void _ShellKey_ExecCmd(shell_t* pShell)
{
    _Shell_ExecCmd(pShell);
    _Shell_ShowPrompt(pShell);
}

static void _ShellKey_MoveCursorToPrevChar(shell_t* pShell)
{
    _Shell_MoveCursor(pShell, SHELL_CURSOR_PREV_CHAR);
}

static void _ShellKey_MoveCursorToNextChar(shell_t* pShell)
{
    _Shell_MoveCursor(pShell, SHELL_CURSOR_NEXT_CHAR);
}

static void _ShellKey_MoveCursorToLineBegin(shell_t* pShell)
{
    _Shell_MoveCursor(pShell, SHELL_CURSOR_LINE_BEGIN);
}

static void _ShellKey_MoveCursorToLineEnd(shell_t* pShell)
{
    _Shell_MoveCursor(pShell, SHELL_CURSOR_LINE_END);
}

static void _ShellKey_TabComplete(shell_t* pShell)
{
    if (pShell->u16CmdSize == 0)
    {
        pShell->puts(LINEEND);
        Shell_ListAll(pShell);
        _Shell_ShowPrompt(pShell);
    }
    else  // length > 0
    {
        uint16_t u16LastMacthedIndex = 0;
        uint16_t u16MatchedCount     = 0;

        cmd_t* pTable = pShell->aCmds.pTable;

        for (uint16_t i = 0; i < pShell->aCmds.u16Count; i++)
        {
            if (strncmp(_GetCmdName(&pTable[i]), (const char*) (pShell->pu8Buffer), pShell->u16CmdSize) == 0)
            {
                if (++u16MatchedCount == 1)
                {
                    pShell->puts(LINEEND);
                }

                u16LastMacthedIndex = i;

                _Shell_ShowCmd(pShell, &pTable[i]);
            }
        }

        if (u16MatchedCount == 0)
        {
            return;
        }
        else if (u16MatchedCount == 1)
        {
            strcpy((char*) (pShell->pu8Buffer), _GetCmdName(&pTable[u16LastMacthedIndex]));
            pShell->u16CursorPos = pShell->u16CmdSize = strlen((char*) (pShell->pu8Buffer));
        }

        _Shell_ShowPrompt(pShell);
        pShell->puts(pShell->pu8Buffer);
    }
}

static void _ShellKey_ClearLine(shell_t* pShell)
{
    _Shell_ClearLine(pShell);
    _Shell_ShowPrompt(pShell);
}

static void _ShellKey_DeleteToEnd(shell_t* pShell)
{
    // Delete to the end of the line

    if (pShell->u16CursorPos < pShell->u16CmdSize)
    {
        uint16_t i;

        for (i = pShell->u16CursorPos; i < pShell->u16CmdSize; i++)
        {
            pShell->putc(' ');
        }

        for (i = pShell->u16CursorPos; i < pShell->u16CmdSize; i++)
        {
            pShell->putc('\b');
        }

        pShell->pu8Buffer[pShell->u16CmdSize = pShell->u16CursorPos] = '\0';
    }
}

#if SHELL_HISTORY_SW

static void _ShellKey_SwitchPrevHistroy(shell_t* pShell)
{
    _Shell_HistorySwitch(pShell, true);
}

static void _ShellKey_SwitchNextHistroy(shell_t* pShell)
{
    _Shell_HistorySwitch(pShell, false);
}

#endif

static void _ShellCmd_ShowHelp(shell_t* pShell, int argc, char* argv[])
{
    if (argc == 1)
    {
        Shell_ListAll(pShell);
    }
    else if (argc == 2)
    {
        cmd_t* pCmd = _Shell_SeekCmd(pShell, argv[1]);

        if (pCmd != nullptr)
        {
            if (pCmd->szDesc != nullptr)
            {
                pShell->puts(pCmd->szDesc);
                pShell->puts(LINEEND);
            }
        }
    }
}

static void _ShellCmd_Echo(shell_t* pShell, int argc, char* argv[])
{
    if (argc >= 2)
    {
        pShell->puts(argv[1]);
        pShell->puts(LINEEND);
    }
}

static void _ShellCmd_ClearScreen(shell_t* pShell)
{
    pShell->puts("\e[2J\e[1H");
}

static void _SysCmd_Reboot(shell_t* pShell)
{
    NVIC_SystemReset();
}

//---------------------------------------------------------------------------

CMD_EXPORT_KEY(0, KEY_TAB, _ShellKey_TabComplete, )      // '\t'
CMD_EXPORT_KEY(0, KEY_DELETE, _ShellKey_Delete, )        // Delete
CMD_EXPORT_KEY(0, KEY_BACKSPACE, _ShellKey_BackSpace, )  // Backspace

CMD_EXPORT_KEY(0, KEY_CTRL_H, _ShellKey_BackSpace, )              // Ctrl+H
CMD_EXPORT_KEY(0, KEY_CTRL_K, _ShellKey_DeleteToEnd, )            // Ctrl+K
CMD_EXPORT_KEY(0, KEY_CTRL_U, _ShellKey_ClearLine, )              // Ctrl+U
CMD_EXPORT_KEY(0, KEY_CTRL_A, _ShellKey_MoveCursorToLineBegin, )  // Ctrl+A
CMD_EXPORT_KEY(0, KEY_CTRL_B, _ShellKey_MoveCursorToPrevChar, )   // Ctrl+B
CMD_EXPORT_KEY(0, KEY_CTRL_E, _ShellKey_MoveCursorToLineEnd, )    // Ctrl+E
CMD_EXPORT_KEY(0, KEY_CTRL_D, _ShellKey_Delete, )                 // Ctrl+D
CMD_EXPORT_KEY(0, KEY_CTRL_F, _ShellKey_MoveCursorToNextChar, )   // Ctrl+F

CMD_EXPORT_KEY(0, KEY_ENTER_LF, _ShellKey_ExecCmd, )  // ENTER_LF, '\r'
CMD_EXPORT_KEY(0, KEY_ENTER_CR, _ShellKey_ExecCmd, )  // ENTER_CR, '\n'
// CMD_EXPORT_KEY(0, KEY_ENTER_CRLF, _ShellKey_ExecCmd, )  // ENTER_CRLF, '\r\n'

CMD_EXPORT_KEY(0, KEY_LEFT_ARROW, _ShellKey_MoveCursorToPrevChar, )   // '\e[D'
CMD_EXPORT_KEY(0, KEY_RIGHT_ARROW, _ShellKey_MoveCursorToNextChar, )  // '\e[C'

#if SHELL_HISTORY_SW
CMD_EXPORT_KEY(0, KEY_UP_ARROW, _ShellKey_SwitchPrevHistroy, up)      // '\e[A'
CMD_EXPORT_KEY(0, KEY_DOWN_ARROW, _ShellKey_SwitchNextHistroy, down)  // '\e[B'
CMD_EXPORT_FUNC(CMD_TYPE(CMD_FUNC_STYLE_C) | CMD_FUNC_ARGC(0), history, _Shell_ShowHistory, show History)
#endif

CMD_EXPORT_FUNC(CMD_TYPE(CMD_FUNC_STYLE_C) | CMD_FUNC_ARGC(0), cls, _ShellCmd_ClearScreen, clear console)

CMD_EXPORT_FUNC(CMD_TYPE(CMD_FUNC_STYLE_C) | CMD_FUNC_ARGC(0), cmds, Shell_ListCmds, list all cmd)
CMD_EXPORT_FUNC(CMD_TYPE(CMD_FUNC_STYLE_C) | CMD_FUNC_ARGC(0), vars, Shell_ListVars, list all var)
CMD_EXPORT_FUNC(CMD_TYPE(CMD_FUNC_STYLE_MAIN), help, _ShellCmd_ShowHelp, show command info)

CMD_EXPORT_FUNC(CMD_TYPE(CMD_FUNC_STYLE_MAIN), echo, _ShellCmd_Echo, echo)
CMD_EXPORT_FUNC(CMD_TYPE(CMD_FUNC_STYLE_C) | CMD_FUNC_ARGC(0), reboot, _SysCmd_Reboot, reset system)
