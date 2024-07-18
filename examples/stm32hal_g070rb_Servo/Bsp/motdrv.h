#ifndef __MOTDRV_H__
#define __MOTDRV_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

#define CONFIG_DOUBLE_SAMP_UPDATE_SW 0  // 双采双更（波峰&波谷）

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Marcos
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

typedef s16 q15;
typedef s32 q31;

#define Q15_MAX 32768
#define Q15(f)  ((q15)(f * 32768))

typedef struct {
    float PP;
    float Rs;
    float Ls;
    float Ld;
    float Lq;
    float Kemf;
    float FluxM;
} motor_attr_t;

typedef enum {
    SECTOR_0,
    SECTOR_1,
    SECTOR_2,
    SECTOR_3,
    SECTOR_4,
    SECTOR_5,
    SECTOR_6,
    SECTOR_7,
} sector_e;

/**
 * @brief 相电流采样类型 (相线上属于高边采样，下桥属于低边采样)
 * @note  ① 双电阻低边采样时刻：在上桥全关，下桥全开的时候进行采样。
 *        ② 相线顺序只影响旋转方向
 */
typedef enum {
    CUR_SAMP_NULL,         /* no current sample */
    CUR_SAMP_UV_LINE,      /* sample UV-phase line current */
    CUR_SAMP_VW_LINE,      /* sample VW-phase line current */
    CUR_SAMP_UVW_LINE,     /* sample UVW-phase line current */
    CUR_SAMP_S_DOWN_BRG,   /* sample single down bridge current */
    CUR_SAMP_UV_DOWN_BRG,  /* sample UV-phase down bridge line current */
    CUR_SAMP_VW_DOWN_BRG,  /* sample VW-phase down bridge line current */
    CUR_SAMP_UW_DOWN_BRG,  /* sample UW-phase down bridge line current */
    CUR_SAMP_UVW_DOWN_BRG, /* sample UVW-phase down bridge line current */
} cur_samp_e;

typedef struct foc {
    cur_samp_e eCurSamp;

    sector_e eSector;  // space vector sector

    q15 IxZero;
    q15 IaZero;
    q15 IbZero;
    q15 IcZero;

    u16 u16ElecAngle;  // rotating angle (pu)

    q15 Sin;  // sine term
    q15 Cos;  // cosine term

    q15 Id;  // stationary d-axis stator variable
    q15 Iq;  // stationary q-axis stator variable

    q15 Ialpha;
    q15 Ibeta;

    q15 Ia;  // phase-a stator
    q15 Ib;  // phase-b stator
    q15 Ic;  // phase-c stator

    q15 Uq;
    q15 Ud;

    q15 Ualpha;  // alpha-axis phase voltage
    q15 Ubeta;   // beta-axis phase voltage

    q15 Ua;
    q15 Ub;
    q15 Uc;

    q15 Umax;
    q15 Umid;
    q15 Umin;

    u16 DutyMax;

    s16 Ta;  // Ton, phase-a switching function
    s16 Tb;  // Ton, phase-b switching function
    s16 Tc;  // Ton, phase-c switching function

} motdrv_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MC_Park(motdrv_t* p);
void MC_InvPark(motdrv_t* p);
void MC_Clark(motdrv_t* p);
void MC_InvClark(motdrv_t* p);
void MC_RebuildCurrent(motdrv_t* p, q15 Ix[]);
void MC_SinCos(motdrv_t* p);
void MC_MinMaxU(motdrv_t* p);
void MC_OverMod(motdrv_t* p);
void MC_CirLim(motdrv_t* p);
void MC_ZeroSeqInj(motdrv_t* p);
void MC_SVPWM5(motdrv_t* p);

void SVPWMx(motdrv_t* p);
void svm(motdrv_t* pfoc);
void SVGEN_run(motdrv_t* p, uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif
