#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#include "gdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool hexdump(const uint8_t* cpu8Buffer, uint32_t u32BytesDumped, uint8_t u8BytesPerLine, uint8_t u8BytesOncePrint, bool bShowAscii, const char* szPrefixFormat, uint32_t u32ShownStartingAddress);

#ifdef __cplusplus
}
#endif

#endif
