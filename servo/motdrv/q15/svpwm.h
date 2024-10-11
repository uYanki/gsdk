#ifndef __SVPWM_H__
#define __SVPWM_H__

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef enum {
    SECTOR_1,
    SECTOR_2,
    SECTOR_3,
    SECTOR_4,
    SECTOR_5,
    SECTOR_6,
} sector_e;

typedef struct {
    q15* q15Ua_i;   // [Pu] Q15
    q15* q15Ub_i;   // [Pu] Q15
    q15* q15Uc_i;   // [Pu] Q15
    q15* q15Udc_i;  // [Pu] Q15

    sector_e* eSector_o;

    u16* u16PwmPeriod;
    u16* u16PwmCmpA_o;  // 0 ~ u16PwmPeriod-1
    u16* u16PwmCmpB_o;  // 0 ~ u16PwmPeriod-1
    u16* u16PwmCmpC_o;  // 0 ~ u16PwmPeriod-1
} svpwm_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline void Svpwm(svpwm_t* pSvpwm)
{
    // 扇区判断

    q15 *q15UphMin_i, *q15UphMid_i, *q15UphMax_i;
    u16 *u16PwmCmpMin_o, *u16PwmCmpMid_o, *u16PwmCmpMax_o;

    if (*pSvpwm->q15Ua_i >= *pSvpwm->q15Ub_i)
    {
        if (*pSvpwm->q15Ua_i >= *pSvpwm->q15Uc_i)
        {
            if (*pSvpwm->q15Ub_i >= *pSvpwm->q15Uc_i)
            {
                *pSvpwm->eSector_o = SECTOR_1;

                q15UphMax_i = pSvpwm->q15Ua_i;
                q15UphMid_i = pSvpwm->q15Ub_i;
                q15UphMin_i = pSvpwm->q15Uc_i;

                u16PwmCmpMax_o = pSvpwm->u16PwmCmpA_o;
                u16PwmCmpMid_o = pSvpwm->u16PwmCmpB_o;
                u16PwmCmpMin_o = pSvpwm->u16PwmCmpC_o;
            }
            else
            {
                *pSvpwm->eSector_o = SECTOR_6;

                q15UphMax_i = pSvpwm->q15Ua_i;
                q15UphMid_i = pSvpwm->q15Uc_i;
                q15UphMin_i = pSvpwm->q15Ub_i;

                u16PwmCmpMax_o = pSvpwm->u16PwmCmpA_o;
                u16PwmCmpMid_o = pSvpwm->u16PwmCmpC_o;
                u16PwmCmpMin_o = pSvpwm->u16PwmCmpB_o;
            }
        }
        else
        {
            *pSvpwm->eSector_o = SECTOR_5;

            q15UphMax_i = pSvpwm->q15Uc_i;
            q15UphMid_i = pSvpwm->q15Ua_i;
            q15UphMin_i = pSvpwm->q15Ub_i;

            u16PwmCmpMax_o = pSvpwm->u16PwmCmpC_o;
            u16PwmCmpMid_o = pSvpwm->u16PwmCmpA_o;
            u16PwmCmpMin_o = pSvpwm->u16PwmCmpB_o;
        }
    }
    else
    {
        if (*pSvpwm->q15Ua_i < *pSvpwm->q15Uc_i)
        {
            if (*pSvpwm->q15Ub_i < *pSvpwm->q15Uc_i)
            {
                *pSvpwm->eSector_o = SECTOR_4;

                q15UphMax_i = pSvpwm->q15Uc_i;
                q15UphMid_i = pSvpwm->q15Ub_i;
                q15UphMin_i = pSvpwm->q15Ua_i;

                u16PwmCmpMax_o = pSvpwm->u16PwmCmpC_o;
                u16PwmCmpMid_o = pSvpwm->u16PwmCmpB_o;
                u16PwmCmpMin_o = pSvpwm->u16PwmCmpA_o;
            }
            else
            {
                *pSvpwm->eSector_o = SECTOR_3;

                q15UphMax_i = pSvpwm->q15Ub_i;
                q15UphMid_i = pSvpwm->q15Uc_i;
                q15UphMin_i = pSvpwm->q15Ua_i;

                u16PwmCmpMax_o = pSvpwm->u16PwmCmpB_o;
                u16PwmCmpMid_o = pSvpwm->u16PwmCmpC_o;
                u16PwmCmpMin_o = pSvpwm->u16PwmCmpA_o;
            }
        }
        else
        {
            *pSvpwm->eSector_o = SECTOR_2;

            q15UphMax_i = pSvpwm->q15Ub_i;
            q15UphMid_i = pSvpwm->q15Ua_i;
            q15UphMin_i = pSvpwm->q15Uc_i;

            u16PwmCmpMax_o = pSvpwm->u16PwmCmpB_o;
            u16PwmCmpMid_o = pSvpwm->u16PwmCmpA_o;
            u16PwmCmpMin_o = pSvpwm->u16PwmCmpC_o;
        }
    }

    // 计算矢量作用时长

    u16 u16HalfPrd = *pSvpwm->u16PwmPeriod >> 1;
    s16 s16Tmp1    = (s16)((s32)u16HalfPrd * (s32)(*q15UphMax_i - *q15UphMin_i) / (s32)*pSvpwm->q15Udc_i);
    s16 s16Tmp2    = (s16)((s32)u16HalfPrd * (s32)(*q15UphMid_i * 3) / (s32)*pSvpwm->q15Udc_i);

    *u16PwmCmpMax_o = (u16)(u16HalfPrd + s16Tmp1);
    *u16PwmCmpMid_o = (u16)(u16HalfPrd + s16Tmp2);
    *u16PwmCmpMin_o = (u16)(u16HalfPrd - s16Tmp1);
}

#ifdef __cplusplus
}
#endif

#endif
