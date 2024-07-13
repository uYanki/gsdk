#include "tformat.h"
#include "hexdump.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG                       "tformat"
#define LOG_LOCAL_LEVEL                     LOG_LEVEL_INFO

#define TFORMAT_FifoIsEnable(UARTx)         LL_USART_IsEnabledFIFO(USARTx)

#define TFORMAT_TxFifoNotFull(UARTx)        LL_USART_IsActiveFlag_TFNF(UARTx)  // tx fifo not full
#define TFORMAT_TxFifoEmpty(UARTx)          LL_USART_IsActiveFlag_TXE(UARTx)   // tx fifo empty, the stopbit of last byte have been transmit
#define TFORMAT_TransmitByte(UARTx, u8Data) UARTx->TDR = u8Data

#define TFORMAT_RxDataReady(UARTx)          LL_USART_IsActiveFlag_RFNE(UARTx)  // rx fifo is empty
#define TFORMAT_ReceiveByte(UARTx)          UARTx->RDR

#define TFORMAT_ResetModule(UARTx)          LL_USART_SoftReset(UARTx)

#define TFORMAT_GetRxErr(UARTx)             UARTx->LSR&(USART_LSR_PE_Msk | USART_LSR_OE_Msk | USART_LSR_FE_Msk)
#define TFORMAT_ClearRxErr(UARTx)           // Reading the LSR clears the XE bit

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static uint8_t TFORMAT_CRC8(uint8_t* data, uint32_t byte_len);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief P(x) = x8 + 1; LSB first
 */
static uint8_t TFORMAT_CRC8(uint8_t* data, uint32_t byte_len)
{
    uint8_t crc = 0x0; /* Filled with 0 in advance. */
    uint8_t reg_data;
    uint8_t b0;

    for (int32_t i = byte_len - 1; i >= 0; i--)
    {
        reg_data = data[i];
        for (int32_t j = 7; j >= 0; j--)
        {
            b0 = ((reg_data >> j) ^ (crc >> 7u)) & 1u;
            crc <<= 1u;
            crc &= 0xFEu; /* Clear bit0 */
            crc |= b0;
        }
    }

    return crc;
}

void TFORMAT_SetupCmd(tformat_t* pTformat, tformat_cf_did_e cf)
{
    pTformat->cf = cf;

    switch (pTformat->cf)
    {
        case TFORMAT_CF_DID0: /*Data readout: ABS */
        case TFORMAT_CF_DID1: /*Data readout: ABM */
        {
            pTformat->u8TxSize = 1u;
            pTformat->u8RxSize = 6u;
            break;
        }
        case TFORMAT_CF_DID2: /*Data readout: ENID */
        {
            pTformat->u8TxSize = 1u;
            pTformat->u8RxSize = 4u;
            break;
        }
        case TFORMAT_CF_DID3: /*Data readout: ABS, ENID, ABM, ALMC */
        {
            pTformat->u8TxSize = 1u;
            pTformat->u8RxSize = 11u;
            break;
        }
        case TFORMAT_CF_DID6: /* Writing to EEPROM*/
        {
            pTformat->u8TxSize     = 4u;
            pTformat->u8RxSize     = 4u;
            pTformat->au8TxData[1] = pTformat->adf;
            pTformat->au8TxData[2] = pTformat->edf_wr;
            pTformat->au8TxData[3] = TFORMAT_CRC8(pTformat->au8TxData, pTformat->u8TxSize - 1u);
            break;
        }
        case TFORMAT_CF_DIDD: /* Readout from EEPROM */
        {
            pTformat->u8TxSize     = 3u;
            pTformat->u8RxSize     = 4u;
            pTformat->au8TxData[1] = pTformat->adf;
            pTformat->au8TxData[2] = TFORMAT_CRC8(pTformat->au8TxData, pTformat->u8TxSize - 1u);
            break;
        }
        default:
        {
            return;
        }
    }

    pTformat->au8TxData[0] = pTformat->cf;
}

void TFORMAT_ProcData(tformat_t* pTformat)
{
    uint8_t* pu8Data = (uint8_t*)pTformat->au8RxData;

    pTformat->cf_match = (pTformat->cf == pu8Data[0]);

    switch (pTformat->cf)
    {
        case TFORMAT_CF_DID0:
        {
            pTformat->status = pu8Data[1];
            pTformat->st     = *(uint32_t*)&pu8Data[2] & 0x00FFFFFF;
            break;
        }
        case TFORMAT_CF_DID1:
        {
            pTformat->status = pu8Data[1];
            pTformat->mt     = *(uint32_t*)&pu8Data[2] & 0x00FFFFFF;
            break;
        }
        case TFORMAT_CF_DID2:
        {
            pTformat->status = pu8Data[1];
            pTformat->id     = pu8Data[2];
            break;
        }
        case TFORMAT_CF_DID3:
        {
            pTformat->status = pu8Data[1];
            pTformat->st     = *(uint32_t*)&pu8Data[2] & 0x00FFFFFF;
            pTformat->id     = pu8Data[5];
            pTformat->mt     = *(uint32_t*)&pu8Data[6] & 0x00FFFFFF;
            pTformat->almc   = pu8Data[9];
            break;
        }
        case TFORMAT_CF_DID6: /* Writing to EEPROM*/
        {
            pTformat->adf_match = (pTformat->adf == pu8Data[1]);
            pTformat->edf_match = (pTformat->edf_wr == pu8Data[2]);
            pTformat->abs_busy  = pu8Data[1] & 0x80;
            break;
        }
        case TFORMAT_CF_DIDD: /* Readout from EEPROM */
        {
            pTformat->adf_match = (pTformat->adf == pu8Data[1]);
            pTformat->edf_rd    = pu8Data[2];
            pTformat->abs_busy  = pu8Data[1] & 0x80;
            break;
        }
        default:
        {
            return;
        }
    }

    pTformat->crc_match = (TFORMAT_CRC8(pu8Data, pTformat->u8RxSize - 1u) == pu8Data[pTformat->u8RxSize - 1u]);

    if (pTformat->crc_match == false)
    {
        LOGW("crc check fail");
    }
}

bool Tformat_Init(tformat_t* pTformat)
{
    if (pTformat->huart->Instance != USART1)
    {
        return false;
    }

    static const PinMux_t aPinMux[] = {
        {GPIOF, GPIO_PIN_2,  GPIO_PULL_NO, GPIO_MODE_ALTERNATE, GPIO_AF_3}, // UART1_RX
        {GPIOI, GPIO_PIN_12, GPIO_PULL_NO, GPIO_MODE_ALTERNATE, GPIO_AF_0}, // UART1_TX
        {GPIOI, GPIO_PIN_13, GPIO_PULL_NO, GPIO_MODE_ALTERNATE, GPIO_AF_0}, // UART1_RTS
    };

    IOMUX(aPinMux, ARRAY_SIZE(aPinMux));

    USART_HandleType USART_InitHandle = {
        .Instance = USART1,
        .FifoMode = true,
    };

    LL_USART_InitType USART_InitType = {
        .BaudRate     = 2500000UL,
        .WordLength   = USART_WORDLENGTH_8B,
        .Parity       = USART_PARITY_NONE,
        .StopBits     = USART_STOPBITS_1,
        .Mode         = USART_MODE_RS485,
        .OverSampling = USART_OVERSAMPLING_8,
    };

    LL_USART_RS485InitType RS485_InitType = {
        .Polarity        = USART_DE_POLARITY_HIGH,
        .AssertionTime   = 0,
        .DeassertionTime = 0,
    };

    HAL_USART_RS485_Init(&USART_InitHandle, &USART_InitType, &RS485_InitType);
    HAL_USART_EnableFifoMode(&USART_InitHandle);

    return true;
}

void TFORMAT_Transfer(tformat_t* pTformat)
{
    uint8_t u8ByteIndex;

    USART_Type* UARTx = pTformat->huart->Instance;

    // check if tx fifo is empty
    while (TFORMAT_TxFifoEmpty(UARTx) == false)
    {
    }

    for (u8ByteIndex = 0; u8ByteIndex < pTformat->u8TxSize; u8ByteIndex++)
    {
        TFORMAT_TransmitByte(UARTx, pTformat->au8TxData[u8ByteIndex]);
    }

    // wait for tx fifo empty
    while (TFORMAT_TxFifoEmpty(UARTx) == false)
    {
    }
}

bool TFORMAT_Receive(tformat_t* pTformat)
{
    uint8_t u8ByteIndex;

    USART_Type* UARTx = pTformat->huart->Instance;

    for (u8ByteIndex = 0; TFORMAT_RxDataReady(UARTx); u8ByteIndex++)
    {
        pTformat->au8RxData[u8ByteIndex] = TFORMAT_ReceiveByte(UARTx);
    }

    if (pTformat->u8RxSize != u8ByteIndex)
    {
        LOGW("fail to receive");
        return false;
    }

#if 1
    hexdump(pTformat->au8RxData, pTformat->u8RxSize, 16, 1, false, nullptr, 0);
#endif

    return true;
}

bool TFORMAT_Test()
{
    tick_t t = 0;

    USART_HandleType tformat_uart = {
        .Instance = USART1,
    };

    tformat_t tformat = {
        .huart = &tformat_uart,
    };

    Tformat_Init(&tformat);

    while (1)
    {
        if (DelayNonBlockMs(&t, 300))
        {
            TFORMAT_SetupCmd(&tformat, TFORMAT_CF_DID0);
            TFORMAT_Transfer(&tformat);

            DelayBlockUs(100);

            if (TFORMAT_Receive(&tformat) == true)
            {
                TFORMAT_ProcData(&tformat);

                if (tformat.crc_match)
                {
                    LOGI("mt = %d, st = %d", tformat.mt, tformat.st);
                }
            }

            t = GetTickUs();
        }
    }
}
