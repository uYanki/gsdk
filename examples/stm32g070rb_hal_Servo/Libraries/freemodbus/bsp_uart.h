#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include <stdio.h>

#include "stm32g0xx_hal.h"
#include "usart.h"
#include "gsdk.h"

typedef enum {
    UART_DIR_TX,
    UART_DIR_RX,
} uart_dir_e;

void Uart_SetWorkDir(uart_dir_e dir);

#endif
