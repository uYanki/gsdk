#include "gsdk.h"

#if CONFIG_HWUART_MODULE_SW

#include "usart.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_hwuart"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @note UART1 has IdleIsr (used in modbus), UART2 does't have IdleIsr
 */

#define UART_HANDLE huart2

int fputc(int ch, FILE* f)
{
    if (ch == '\n')
    {
        HAL_UART_Transmit(&UART_HANDLE, (uint8_t*)"\r", 1, HAL_MAX_DELAY);
    }

    HAL_UART_Transmit(&UART_HANDLE, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

    return (ch);
}

int fgetc(FILE* f)
{
    uint8_t ch = '\0';
    HAL_UART_Receive(&UART_HANDLE, &ch, 1, HAL_MAX_DELAY);
    return ch;
}

#if defined(__CC_ARM) && 0

// auto switch uart direction when call printf

#include <stdarg.h>

// precall
int $Sub$$__2printf(const char* fmt, ...)
{
    extern int $Super$$__2printf(const char* fmt, ...);

    int len;

    // set tx dir
    Uart_SetWorkDir(UART_DIR_TX);

    // $Super$$__2printf("<*>");

    va_list args;
    va_start(args, fmt);
    len = vprintf(fmt, args);  // print
    va_end(args);

    // set rx dir
    Uart_SetWorkDir(UART_DIR_RX);

    return len;
}

#endif

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#endif
