#ifndef __SPI_MST_H__
#define __SPI_MST_H__

#include "spidefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

err_t SPI_Master_Init(spi_mst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags);

err_t SPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* pu8TxData, uint16_t u16Size);
err_t SPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size);

err_t SPI_Master_TransmitReceiveByte(spi_mst_t* pHandle, uint8_t u8TxData, uint8_t* pu8RxData);
err_t SPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* pu8TxData, uint8_t* pu8RxData, uint16_t u16Size);

err_t SPI_Master_TransmitByte(spi_mst_t* pHandle, uint8_t u8TxData);
err_t SPI_Master_ReceiveByte(spi_mst_t* pHandle, uint8_t* pu8RxData);

void SPI_Master_Select(spi_mst_t* pHandle);
void SPI_Master_Deselect(spi_mst_t* pHandle);

#ifdef __cplusplus
}
#endif

#endif
