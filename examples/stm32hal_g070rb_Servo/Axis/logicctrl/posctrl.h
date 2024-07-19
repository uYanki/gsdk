#ifndef __POS_CTRL_H__
#define __POS_CTRL_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} pos_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void PosCtrlCreat(pos_ctrl_t* pPosCtrl, axis_e eAxisNo);
void PosCtrlInit(pos_ctrl_t* pPosCtrl, axis_e eAxisNo);
void PosCtrlCycle(pos_ctrl_t* pPosCtrl, axis_e eAxisNo);
void PosCtrlIsr(pos_ctrl_t* pPosCtrl, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
