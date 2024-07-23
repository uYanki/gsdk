#ifndef __ARM_DWT_H__
#define __ARM_DWT_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ARM_DWT_SW

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void     DWT_Init(void);
void     DWT_DelayTick(uint32_t u32Ticks);
void     DWT_DelayUS(uint32_t u32TimeUs);
uint32_t DWT_GetTick(void);
uint32_t DWT_TickToUs(uint32_t u32TickStart, uint32_t u32TickEnd);

#endif

#ifdef __cplusplus
}
#endif

#endif
