#ifndef __I2C_MST_H__
#define __I2C_MST_H__

#include "i2cdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

err_t I2C_Master_Init(i2c_mst_t* pHandle, uint32_t u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle);

/**
 * @brief block access
 */

err_t I2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags);
err_t I2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags);

err_t I2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8TxData, uint16_t u16Size, uint16_t u16Flags);
err_t I2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8RxData, uint16_t u16Size, uint16_t u16Flags);

/**
 * @brief byte access
 */

err_t I2C_Master_WriteByte(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t u8Data, uint16_t u16Flags);
err_t I2C_Master_ReadByte(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Flags);

err_t I2C_Master_TransmitByte(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t u8TxData, uint16_t u16Flags);
err_t I2C_Master_ReceiveByte(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8RxData, uint16_t u16Flags);

/**
 * @brief word access
 */

err_t I2C_Master_TransmitWord(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16Data, uint16_t u16Flags);
err_t I2C_Master_ReceiveWord(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t* pu16Data, uint16_t u16Flags);

err_t I2C_Master_ReadWord(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint16_t* pu16Data, uint16_t u16Flags);
err_t I2C_Master_WriteWord(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint16_t u16Data, uint16_t u16Flags);

/**
 * @brief bits access
 */

err_t I2C_Master_ReadByteBits(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t* pu8BitsValue, uint16_t u16Flags);
err_t I2C_Master_WriteByteBits(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t u8BitsValue, uint16_t u16Flags);

err_t I2C_Master_ReadWordBits(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint16_t* pu16BitsValue, uint16_t u16Flags);
err_t I2C_Master_WriteWordBits(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint16_t u16BitsValue, uint16_t u16Flags);

/**
 * @brief tools
 */

bool    I2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags);
uint8_t I2C_Master_ScanAddress(i2c_mst_t* pHandle);
err_t   I2C_Master_Hexdump(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16Flags);

#ifdef __cplusplus
}
#endif

#endif
