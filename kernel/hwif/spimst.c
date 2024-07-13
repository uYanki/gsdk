#include "spimst.h"
#include <string.h>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "spimst"
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

void SPI_Master_Select(spimst_t* pHandle)
{
    switch (pHandle->u16TimingConfig & SPI_FLAG_CS_MODE_Msk)
    {
        case SPI_FLAG_SOFT_CS:
        {
            switch (pHandle->u16TimingConfig & SPI_FLAG_CS_ACTIVE_LEVEL_Msk)
            {
                default:
                case SPI_FLAG_CS_ACTIVE_LOW:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_LOW);
                    break;
                }
                case SPI_FLAG_CS_ACTIVE_HIGH:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_HIGH);
                    break;
                }
            }

            break;
        }
        case SPI_FLAG_HADR_CS:
        case SPI_FLAG_NONE_CS:
        {
            break;
        }
    }
}

void SPI_Master_Deselect(spimst_t* pHandle)
{
    switch (pHandle->u16TimingConfig & SPI_FLAG_CS_MODE_Msk)
    {
        case SPI_FLAG_SOFT_CS:
        {
            switch (pHandle->u16TimingConfig & SPI_FLAG_CS_ACTIVE_LEVEL_Msk)
            {
                default:
                case SPI_FLAG_CS_ACTIVE_LOW:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_HIGH);
                    break;
                }
                case SPI_FLAG_CS_ACTIVE_HIGH:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_LOW);
                    break;
                }
            }

            break;
        }
        case SPI_FLAG_HADR_CS:
        case SPI_FLAG_NONE_CS:
        {
            break;
        }
    }
}

err_t SPI_Master_Init(spimst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags)
{
    pHandle->u16TimingConfig |= u16Flags & (SPI_FLAG_CS_MODE_Msk | SPI_FLAG_CS_ACTIVE_LEVEL_Msk);

    if (memcmp(&pHandle->MOSI, &pHandle->MISO, sizeof(pin_t)) == 0)
    {
        pHandle->u16TimingConfig |= SPI_FLAG_3WIRE;
    }
    else
    {
        pHandle->u16TimingConfig |= SPI_FLAG_4WIRE;
    }

    if (pHandle->SPIx == nullptr)
    {
#if CONFIG_SWI2C_MODULE_SW
        extern const spimst_ops_t g_swSpiOps;
        pHandle->pOps = &g_swSpiOps;
#else
        return ThrowError(ERR_NOT_EXIST, "swspi master module is disabled");
#endif
    }
    else
    {
#if CONFIG_HWI2C_MODULE_SW
        extern const spimst_ops_t g_hwSpiOps;
        pHandle->pOps = &g_hwSpiOps;
#else
        return ThrowError(ERR_NOT_EXIST, "hwspi master module is disabled");
#endif
    }

    if ((pHandle->u16TimingConfig & SPI_FLAG_CS_MODE_Msk) == SPI_FLAG_SOFT_CS)
    {
        if ((pHandle->u16TimingConfig & SPI_FLAG_CS_ACTIVE_LEVEL_Msk) == SPI_FLAG_CS_ACTIVE_LOW)
        {
            PIN_SetMode(&pHandle->CS, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
        }
        else  // SPI_FLAG_CS_ACTIVE_HIGH
        {
            PIN_SetMode(&pHandle->CS, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_DOWN);
        }
    }

    return pHandle->pOps->Init(pHandle, u32ClockSpeedHz, eDutyCycle, u16Flags);
}

err_t SPI_Master_TransmitBlock(spimst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size)
{
    return pHandle->pOps->TransmitBlock(pHandle, cpu8TxData, u16Size);
}

err_t SPI_Master_ReceiveBlock(spimst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size)
{
    return pHandle->pOps->ReceiveBlock(pHandle, pu8RxData, u16Size);
}

err_t SPI_Master_TransmitReceiveBlock(spimst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size)
{
    return pHandle->pOps->TransmitReceiveBlock(pHandle, cpu8TxData, pu8RxData, u16Size);
}

err_t SPI_Master_TransmitByte(spimst_t* pHandle, uint8_t u8TxData)
{
    return SPI_Master_TransmitBlock(pHandle, (const uint8_t*)&u8TxData, 1);
}

err_t SPI_Master_ReceiveByte(spimst_t* pHandle, uint8_t* pu8RxData)
{
    return SPI_Master_ReceiveBlock(pHandle, pu8RxData, 1);
}

err_t SPI_Master_TransmitReceiveByte(spimst_t* pHandle, uint8_t u8TxData, uint8_t* pu8RxData)
{
    return SPI_Master_TransmitReceiveBlock(pHandle, (const uint8_t*)&u8TxData, pu8RxData, 1);
}
