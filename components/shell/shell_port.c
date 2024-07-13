#include "gsdk.h"

#ifdef CONFIG_DEMOS_SW

#include "platform.h"
#include "usart.h"
#include "shell_port.h"
#include "shell_core.h"

#include "stdarg.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LINE_BUFSIZE 64

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static bool _shell_getc(char* c);
static bool _shell_putc(char c);
static bool _shell_puts(char* s);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static uint8_t linebuf[LINE_BUFSIZE * (1 + CONFIG_SHELL_HISTORY_MAX_COUNT)] = {0};

static shell_t shell = {
    .szPrompt       = "uYanki@root: ",
    .getc           = _shell_getc,
    .putc           = _shell_putc,
    .puts           = _shell_puts,
    .eInputMode     = SHELL_INPUT_MODE_INSERT,
    .pu8Buffer      = linebuf,
    .u16LineMaxSize = LINE_BUFSIZE,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static bool _shell_getc(char* c)
{
    return HAL_UART_Receive(&huart1, (uint8_t*)c, 1, 0xFF) == HAL_OK;
}

static bool _shell_putc(char c)
{
    return HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, 0xFF) == HAL_OK;
}

static bool _shell_puts(char* s)
{
    return HAL_UART_Transmit(&huart1, (uint8_t*)s, strlen(s), 0xFF) == HAL_OK;
}

shell_t* Shell_GetInst(void)
{
    return &shell;
}

void Shell_Test(void)
{
    shell_t* pShell = Shell_GetInst();

    Shell_Init(pShell);

    // Shell_ExecCmd(pShell, "memdump 1233 -s123");
    // Shell_ExecCmd(pShell, "memdump 1233 -s 123");

    while (1)
    {
        Shell_Cycle(pShell);
    }
}

#endif
