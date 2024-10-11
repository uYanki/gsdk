#include "./adconv.h"

#include "motdrv.h"

__IO u16 ADConv[6] = {0};

#define AD_CUR_C             (ADConv[0])
#define AD_CUR_A             (ADConv[1])
#define AD_NTC               (ADConv[2])
#define AD_VBUS              (ADConv[3])
#define AD_POT1              (ADConv[4])
#define AD_POT2              (ADConv[5])

//-----------------------------------------------------------------------------

#define SAMP_TYPE_NULL       0  // 无 null
#define SAMP_TYPE_SARADC_12B 1  // 运放 sar adc (q12)
#define SAMP_TYPE_SARADC_16B 2  // 运放 sar adc (q15)
#define SAMP_TYPE_SDADC_16B  3  // 调制器 sigma-delta adc

/**
 * @note
 *
 * 调制器时钟分主动输出和被动输入。
 *
 * 对于被动输入的调制器，主控使用PWM产生频率20M占空比50%的PWM，给到调制器的时钟引脚，同时给到自身SDFM模块的时钟引脚。
 *
 */

#define CONFIG_UCDC_SAMP_IF  SAMP_TYPE_NULL        // 控制电电压
#define CONFIG_UMDC_SAMP_IF  SAMP_TYPE_SARADC_12B  // 主回路电压
#define CONFIG_CUR_SAMP_IF   SAMP_TYPE_SARADC_12B  // 电流
#define CONFIG_UAI_SAMP_IF   SAMP_TYPE_SARADC_12B  // 模拟量输入

q15 GetCurPu(u16 u16Ind)
{
    u16 u16Data = ADConv[u16Ind];

    /**
     * @note
     *
     * ① SarAdc 只能采正电压，需减去运放的偏置电压 0x8000 (Pu)
     * ② SdAdc 采的是正负电压，不需要减 0x8000
     *
     *  满量程标幺到 [-32768,32767]
     *
     */

#if CONFIG_CUR_SAMP_IF == SAMP_TYPE_NULL
    return 0;
#elif CONFIG_CUR_SAMP_IF == SAMP_TYPE_SARADC_12B
    return (q15)(u16Data << 4) - 0x8000;
#elif CONFIG_CUR_SAMP_IF == SAMP_TYPE_SARADC_16B
    return (q15)u16Data - 0x8000;
#elif CONFIG_CUR_SAMP_IF == SAMP_TYPE_SDADC_16B
    return (q15)u16Data;
#endif
}

q15 GetUmdcPu(u16 u16Ind)
{
    u16 u16Data = ADConv[u16Ind];

#if CONFIG_UMDC_SAMP_IF == SAMP_TYPE_NULL
    return 0;
#elif CONFIG_UMDC_SAMP_IF == SAMP_TYPE_SARADC_12B
    return (q15)(u16Data << 3);
#elif CONFIG_UMDC_SAMP_IF == SAMP_TYPE_SDADC_16B
    return (q15)u16Data >> 1;
#endif
}

q15 GetUaiPu(u16 u16Ind)
{
    u16 u16Data = ADConv[u16Ind];

#if CONFIG_UMDC_SAMP_IF == SAMP_TYPE_SARADC_12B
    return u16Data << 3;
#endif
}

//-----------------------------------------------------------------------------

#if (CONFIG_EXT_AI_NUM >= 1)
s16 GetUai1(void)
{
    return GetUaiPu(4);
}
#endif
#if (CONFIG_EXT_AI_NUM >= 2)
s16 GetUai2(void)
{
    return GetUaiPu(5);
}
#endif
#if (CONFIG_EXT_AI_NUM >= 3)
s16 GetUai3(void)
{
    return 0;
}
#endif

//-----------------------------------------------------------------------------
//

u16 GetUmdc(void)
{
    return (u16)(AD_VBUS << 3);
}

#ifdef __UCDC_SW

u16 GetUcdc(void)
{
    return (u16)(AD_VBUS << 3);
}

#endif

//-----------------------------------------------------------------------------
// current sampler

#if __CUR_SAMP_SHUNT_1

s16 GetCurX(axis_e eAxisNo, u8 nSampIdx)  // Pu
{
    switch (eAxisNo)
    {
#if CONFIG_AXIS_NUM >= 1
        case AXIS_0: {
            if (nSampIdx == 0)
            {
                return (s16)(AD_CUR_0 << 4) - 0x8000;
            }
            else if (nSmapIdx == 1)
            {
                return (s16)(AD_CUR_1 << 4) - 0x8000;
            }
            else
            {
                return 0;
            }
        }
#endif
#if CONFIG_AXIS_NUM >= 2
        case AXIS_1:
            return 0;
#endif
        default:
            return 0;
    }
}

#endif

#if __CUR_SAMP_SHUNT_2 || __CUR_SAMP_SHUNT_3

s16 GetCurU(axis_e eAxisNo)  // Pu
{
    return GetCurPu(1);
}

s16 GetCurV(axis_e eAxisNo)  // Pu
{
    return 0;
}

s16 GetCurW(axis_e eAxisNo)  // Pu
{
    return GetCurPu(0);
}

#endif

//-----------------------------------------------------------------------------

#if (CONFIG_TEMP_SAMP_NUM >= 1)

s16 GetTs1(void)
{
    return 0;
}

#endif

u16 NTC_GetTempSi(u16 adconv)  // unit: 0.1℃
{
    int ntc_resistance = 1000 * adconv / (4096 - adconv);

    // B = 3380K, 10k , from 0 degree to 100 degree, table, the step is 5 degree
    static const u16 tbl[] = {2749, 2218, 1802, 1472, 1210, 1000, 831, 694, 583, 491, 416, 354, 302, 259, 223, 193, 167, 146, 127, 111, 98};

    u16 min = 0, max = sizeof(tbl) / 2 - 1, mid;  // index

    if (ntc_resistance > tbl[min])
    {
        return 100.f;
    }
    else if (ntc_resistance < tbl[max])
    {
        return 0.f;
    }

    while ((max - min) > 1)  // 二分法
    {
        mid = (max + min) >> 1;
        (tbl[mid] < ntc_resistance) ? (max = mid) : (min = mid);
    }

    return 50 * min + 50 * (tbl[min] - ntc_resistance) / (tbl[min] - tbl[min + 1]);
}
