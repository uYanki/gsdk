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

err_t SPI_Master_Init(spimst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags);

err_t SPI_Master_TransmitBlock(spimst_t* pHandle, const uint8_t* pu8TxData, uint16_t u16Size);
err_t SPI_Master_ReceiveBlock(spimst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size);

err_t SPI_Master_TransmitReceiveByte(spimst_t* pHandle, uint8_t u8TxData, uint8_t* pu8RxData);
err_t SPI_Master_TransmitReceiveBlock(spimst_t* pHandle, const uint8_t* pu8TxData, uint8_t* pu8RxData, uint16_t u16Size);

err_t SPI_Master_TransmitByte(spimst_t* pHandle, uint8_t u8TxData);
err_t SPI_Master_ReceiveByte(spimst_t* pHandle, uint8_t* pu8RxData);

void SPI_Master_Select(spimst_t* pHandle);
void SPI_Master_Deselect(spimst_t* pHandle);

#ifdef __cplusplus
}
#endif

#endif
