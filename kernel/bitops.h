#ifndef __BITOPS_H__
#define __BITOPS_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_REVBIT_TABLE_SW 0  // 位反转表

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

uint8_t  revbit8(uint8_t u8Data);
uint16_t revbit16(uint16_t u16Data);
uint32_t revbit32(uint32_t u32Data);

void    setbits(uint8_t* pu8Data, uint16_t u16StartBit, uint8_t u8BitWidth, uint8_t u8Value);
uint8_t getbits(uint8_t* pu8Data, uint16_t u16StartBit, uint8_t u8BitWidth);

#ifdef __cplusplus
}
#endif

#endif
