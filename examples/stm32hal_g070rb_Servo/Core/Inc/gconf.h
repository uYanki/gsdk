#ifndef __GSDK_CONF_H__
#define __GSDK_CONF_H__

#include "gopts.h"

#define CONFIG_ARM_DWT_SW       0
#define CONFIG_ARM_TSG_SW       0

#define CONFIG_DEMOS_SW         0

#define CONFIG_SWUART_MODULE_SW 1
#define CONFIG_SWI2C_MODULE_SW  1
#define CONFIG_SWSPI_MODULE_SW  1

#define CONFIG_HWUART_MODULE_SW 1
#define CONFIG_HWI2C_MODULE_SW  1
#define CONFIG_HWSPI_MODULE_SW  0

#define CONFIG_HWCAN_MODULE_SW  1


//-----------------------------------------------------------------------------
// 串口配置
//

/**
 * @param CONFIG_UART_MODE
 */
#define UART_MODE_NORMAL 0
#define UART_MODE_RS485  1
#define UART_MODE_RS232  2
#define UART_MODE_RS422  3

// 串口类型
#define CONFIG_UART_MODE UART_MODE_NORMAL

// 使能重定向
#define CONFIG_REDIRECT_UART_PORT huart2
#define CONFIG_REDIRECT_PRINTF_SW 2

// MODBUS
#define CONFIG_MODBUS_UART_PORT  huart2
#define CONFIG_MODBUS_RXDMA_PORT hdma_usart2_rx

// 缓冲区大小
#define CONFIG_UART_TXBUF_SIZE 256
#define CONFIG_UART_RXBUF_SIZE 256

#if CONFIG_UART_MODE == UART_MODE_RS485
// RS485 换向时间
#define CONFIG_RS485_SWITCH_TIME_US 200
#endif

// 编码器类型
#define CONFIG_HALL_ENC_SW 1

#endif /*<! __GSDK_CONF_H__ */
