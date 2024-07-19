#include "abs_tformat.h"

// #if __ABS_ENC_TFORMAT_SW

#include "main.h"
#include "usart.h"

static void cmd_tx(u8* pau8TxBuf, u8 u8TxCnt)
{
    HAL_GPIO_WritePin(RS485_RTS2_GPIO_Port, RS485_RTS2_Pin, GPIO_PIN_SET);  // TX
    HAL_UART_Transmit(&UART_TFORMAT, pau8TxBuf, u8TxCnt, 0xFF);
    HAL_GPIO_WritePin(RS485_RTS2_GPIO_Port, RS485_RTS2_Pin, GPIO_PIN_RESET);  // RX
}

static void cmd_rx(u8* pau8RxBuf, u8 u8RxCnt)  // 异步读取
{
    HAL_UART_Receive_DMA(&UART_TFORMAT, pau8RxBuf, u8RxCnt);
}

/**
 * @brief update encoder position
 */
void TFormatRdPos(abs_drv_t* pEnc)
{
    abs_tformat_t* pArgs     = (abs_tformat_t*)(pEnc->pArgs);
    u8*            pau8RxBuf = pArgs->au8RxBuf;

    ///< @note: The result of bitwise XOR of the same number is 0
    u8 u8CrcSts = pau8RxBuf[0] ^ pau8RxBuf[1] ^ pau8RxBuf[2] ^ pau8RxBuf[3] ^ pau8RxBuf[4] ^ pau8RxBuf[5];

    u8CrcSts = 0;
    if (u8CrcSts == 0)  // success
    {
        pEnc->eCommSts = ABS_COMM_DRDY;
        pEnc->u32Pos   = (((u32)pau8RxBuf[4] << 16) + ((u32)pau8RxBuf[3] << 8) + (u32)pau8RxBuf[2]) >> 2;
    }
    else  // failure
    {
        pEnc->eCommSts = ABS_COMM_CRC_ERR;
        pEnc->u16CommErr++;  // 校验错误
    }
}

void TFormatTimeout(abs_drv_t* pEnc)
{
    pEnc->eCommSts = ABS_COMM_TIMEOUT;
    pEnc->u16CommErr++;  // 超时
}

void TFormatRdData(abs_drv_t* pEnc)
{
}

void TFormatWrCmd(abs_drv_t* pEnc, abs_cmd_e eCmd)
{
    static u8 sau8TxBuf[1];

    switch (eCmd)
    {
        case TFORMAT_CMD_READ_POS:  // 读取位置
        {
            u8* pau8RxBuf = ((abs_tformat_t*)(pEnc->pArgs))->au8RxBuf;
            sau8TxBuf[0]  = 0x02;
            cmd_tx(sau8TxBuf, 1);
            cmd_rx(pau8RxBuf, 6);
            break;
        }
        case TFORMAT_CMD_SET_ZERO_POS:  // 置零位
        {
            sau8TxBuf[0] = 0xAA;
            cmd_tx(sau8TxBuf, 1);
            break;
        }
        case TFORMAT_CMD_CLR_TURNS:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    pEnc->eCommSts = ABS_COMM_WAIT;
}

#if 0

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &UART_TFORMAT)
    {
        extern abs_enc_t AbsEnc;
        TFormatRdPos(&AbsEnc.absTFormat);
    }
}

#endif

// #endif
