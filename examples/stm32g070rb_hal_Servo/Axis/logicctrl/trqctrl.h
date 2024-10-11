#ifndef __TRQ_CTRL_H__
#define __TRQ_CTRL_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16* pu16StopCmd_i;
    u16* pu16StopMode_i;
    u16* pu16StopPlanMode_i;
    u16* pu16EmStopTrqDigRef_i;

    u16* pu16TrqRefSrc_i;
    s16* ps16TrqDigRef_i;
    u16* pu16MultMotionEn_i;
    s16* ps16TrqDigRefs_i;
    u16* pu16TrqMulRefSel_i;

    u16* pu16TrqLimSrc_i;
    u16* pu16TrqLimFwd_i;
    u16* pu16TrqLimRev_i;

    s16* ps16UaiSi0_i;
    s16* ps16UaiSi1_i;
    u16* pu16AiTrqCoeff0_i;
    u16* pu16AiTrqCoeff1_i;

    s16* ps16LogicTrqRef_o;
    s16* ps16LogicTrqLimFwd_o;
    s16* ps16LogicTrqLimRev_o;

    s32* ps32DrvTrqFb_i;
} trq_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void TrqCtrlCreat(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo);
void TrqCtrlInit(trq_ctrl_t* pTrqCtrl);
void TrqCtrlCycle(trq_ctrl_t* pTrqCtrl);
void TrqCtrlIsr(trq_ctrl_t* pTrqCtrl);

#ifdef __cplusplus
}
#endif

#endif
