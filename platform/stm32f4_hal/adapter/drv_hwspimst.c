#include "spimst.h"

#if CONFIG_HWSPI_MODULE_SW

#include "spi.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_hwspi"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

#define SPI_TIMEOUT     0xFF

#define MakeError_(eStatus)               \
    do {                                  \
        switch (eStatus)                  \
        {                                 \
            case HAL_OK: return ERR_NONE; \
            case HAL_BUSY: return ERR_BUSY;  // busy          \
            case HAL_TIMEOUT: return ERR_TIMEOUT; // timeout \
            default: return ERR_FAIL; // generic error       \
        }                                                               \
    } while (0)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static err_t HWSPI_Master_Init(spi_mst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags);
static err_t HWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size);
static err_t HWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size);
static err_t HWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const spimst_ops_t g_hwSpiOps = {
    .Init                 = HWSPI_Master_Init,
    .TransmitBlock        = HWSPI_Master_TransmitBlock,
    .ReceiveBlock         = HWSPI_Master_ReceiveBlock,
    .TransmitReceiveBlock = HWSPI_Master_TransmitReceiveBlock,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

static err_t HWSPI_Master_Init(spi_mst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);

    if (
#ifdef SPI1
        hwspi->Instance == SPI1 ||
#endif
#ifdef SPI2
        hwspi->Instance == SPI2 ||
#endif
#ifdef SPI3
        hwspi->Instance == SPI3 ||
#endif
#ifdef SPI4
        hwspi->Instance == SPI4 ||
#endif
#ifdef SPI5
        hwspi->Instance == SPI5 ||
#endif
        0)
    {
        return ERR_NONE;
    }

    // invalid instance
    return ERR_INVALID_VALUE;  // unknown instance
}

static err_t HWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);
    MakeError_(HAL_SPI_Transmit(hwspi, (uint8_t*)cpu8TxData, u16Size, SPI_TIMEOUT));
}

static err_t HWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);
    MakeError_(HAL_SPI_Receive(hwspi, pu8RxData, u16Size, SPI_TIMEOUT));
}

static err_t HWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);
    MakeError_(HAL_SPI_TransmitReceive(hwspi, (uint8_t*)cpu8TxData, pu8RxData, u16Size, SPI_TIMEOUT));
}

#endif
