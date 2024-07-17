#include "./adconv.h"

#define AD_POT1  (ADConv[0])
#define AD_POT2  (ADConv[1])
#define AD_NTC   (ADConv[2])
#define AD_VBUS  (ADConv[3])
#define AD_CUR_C (ADConv[4])
#define AD_CUR_A (ADConv[5])

__IO u16 ADConv[7] = {0};

#if (CONFIG_EXT_AI_NUM >= 1)
s16 GetUai1(void)
{
    return AD_POT1 << 3;  // Q15
}
#endif
#if (CONFIG_EXT_AI_NUM >= 2)
s16 GetUai2(void)
{
    return AD_POT2 << 3;  // Q15
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

s16 GetCurX(axis_e eAxisNo, u8 nSampIdx)  // Q15
{
    switch (eAxisNo)
    {
#if CONFIG_AXIS_NUM >= 1
        case AXIS_0:
        {
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
        case AXIS_1: return 0;
#endif
        default: return 0;
    }
}

#endif

#if __CUR_SAMP_SHUNT_2 || __CUR_SAMP_SHUNT_3

s16 GetCurU(axis_e eAxisNo)  // Q15
{
    switch (eAxisNo)
    {
#if CONFIG_AXIS_NUM >= 1
        case AXIS_0: return (s16)(AD_CUR_A << 4) - 0x8000;
#endif
#if CONFIG_AXIS_NUM >= 2
        case AXIS_1: return 0;
#endif
        default: return 0;
    }
}

s16 GetCurV(axis_e eAxisNo)  // Q15
{
    switch (eAxisNo)
    {
#if CONFIG_AXIS_NUM >= 1
        case AXIS_0: return 0;
#endif
#if CONFIG_AXIS_NUM >= 2
        case AXIS_1: return 0;
#endif
        default: return 0;
    }
}

s16 GetCurW(axis_e eAxisNo)  // Q15
{
    switch (eAxisNo)
    {
#if CONFIG_AXIS_NUM >= 1
        case AXIS_0: return (s16)(AD_CUR_C << 4) - 0x8000;
#endif
#if CONFIG_AXIS_NUM >= 2
        case AXIS_1: return 0;
#endif
        default: return 0;
    }
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
