#ifndef __SPI_MFRC522_H__
#define __SPI_MFRC522_H__

#include "spimst.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define RC522_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPOL_LOW | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW)

typedef struct {
    __IN spi_mst_t*  hSPI;
    __IN const pin_t RST;
} spi_rc522_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#define MI_NOTAGERR 1
#define MI_ERR      2

void RC522_AntennaOn(spi_rc522_t* pHandle);   // 开启天线
void RC522_AntennaOff(spi_rc522_t* pHandle);  // 关闭天线

uint8_t RC522_Request(spi_rc522_t* pHandle, uint8_t req_code, uint8_t* pTagType);                              // 寻卡
uint8_t RC522_Anticoll(spi_rc522_t* pHandle, uint8_t* pSnr);                                                   // 防冲撞
uint8_t RC522_Select(spi_rc522_t* pHandle, uint8_t* pSnr);                                                     // 选卡
uint8_t RC522_AuthState(spi_rc522_t* pHandle, uint8_t auth_mode, uint8_t addr, uint8_t* pKey, uint8_t* pSnr);  // 密码校验

uint8_t RC522_Read(spi_rc522_t* pHandle, uint8_t addr, uint8_t* pData);   // 读数据
uint8_t RC522_Write(spi_rc522_t* pHandle, uint8_t addr, uint8_t* pData);  // 写数据
uint8_t RC522_Value(spi_rc522_t* pHandle, uint8_t dd_mode, uint8_t addr, uint8_t* pValue);
uint8_t RC522_BakValue(spi_rc522_t* pHandle, uint8_t sourceaddr, uint8_t goaladdr);

err_t RC522_Halt(spi_rc522_t* pHandle);  // 休眠

uint8_t RC522_ComMF522(spi_rc522_t* pHandle, uint8_t Command, uint8_t* pInData, uint8_t InLenByte, uint8_t* pOutData, uint16_t* pOutLenBit);

bool RC522_IsDataBlock(spi_rc522_t* pHandle, uint8_t u8Addr);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void RC522_Test(void);
#endif

#endif
