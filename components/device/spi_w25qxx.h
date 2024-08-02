#ifndef __SPI_W25QXX_H__
#define __SPI_W25QXX_H__

#include "spimst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define W25QXX_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_CPOL_LOW | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW | SPI_FLAG_FAST_CLOCK_ENABLE)

/**
 * @brief W25QXX Device ID
 */
#define W25Q80_ID  0xEF13
#define W25Q16_ID  0xEF14
#define W25Q32_ID  0xEF15
#define W25Q64_ID  0xEF16
#define W25Q128_ID 0xEF17
#define W25Q256_ID 0xEF18
#define W25Q512_ID 0xEF19

typedef struct {
    __IN spi_mst_t* hSPI;

    uint16_t u16ManDevID;  // 0:24bit, 1:32bit
} spi_w25qxx_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

err_t W25Qxx_Init(spi_w25qxx_t* pHandle);
err_t W25Qxx_ReadDeviceID(spi_w25qxx_t* pHandle, __OUT uint8_t ID[2]);
err_t W25Qxx_ReadUniqueID(spi_w25qxx_t* pHandle, __OUT uint8_t ID[8]);
err_t W25Qxx_PowerDown(spi_w25qxx_t* pHandle);
err_t W25Qxx_WakeUp(spi_w25qxx_t* pHandle);
err_t W25Qxx_ReadData(spi_w25qxx_t* pHandle, uint32_t u32ReadAddr, uint32_t u32Size, uint8_t* pu8Data);
err_t W25Qxx_WriteData(spi_w25qxx_t* pHandle, uint32_t u32WriteAddr, uint32_t u32Size, const uint8_t* cpu8Data);
err_t W25Qxx_EraseSector(spi_w25qxx_t* pHandle, uint32_t u32Address);
err_t W25Qxx_EraseChip(spi_w25qxx_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void W25Qxx_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
