#ifndef __TRQ_CTRL_H__
#define __TRQ_CTRL_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} trq_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void TrqCtrlCreat(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo);
void TrqCtrlInit(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo);
void TrqCtrlCycle(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo);
void TrqCtrlIsr(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
