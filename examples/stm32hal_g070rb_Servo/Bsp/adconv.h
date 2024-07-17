
#ifndef __ADCONV_H__
#define __ADCONV_H__

// #include "defs.h"
#include "gsdk.h"
#include "paratbl.h"
// #include "cursamp.h"

#include "stm32g0xx_hal.h"

extern u16 GetUmdc(void);

#ifdef __UCDC_SW
extern u16 GetUcdc(void);
#endif

#if (CONFIG_EXT_AI_NUM >= 1)
extern s16 GetUai1(void);
#endif
#if (CONFIG_EXT_AI_NUM >= 2)
extern s16 GetUai2(void);
#endif
#if (CONFIG_EXT_AI_NUM >= 3)
extern s16 GetUai3(void);
#endif

#if __CUR_SAMP_SHUNT_1
extern s16 GetCurX(axis_e eAxisNo, u8 nSampIdx);
#endif
#if __CUR_SAMP_SHUNT_2 || __CUR_SAMP_SHUNT_3
extern s16 GetCurU(axis_e eAxisNo);
extern s16 GetCurV(axis_e eAxisNo);
extern s16 GetCurW(axis_e eAxisNo);
#endif

#if (CONFIG_TEMP_SAMP_NUM >= 1)
extern s16 GetTs1(void);
extern u16 NTC_GetTempSi(u16 adconv);
#endif

extern __IO u16 ADConv[7];

#endif
