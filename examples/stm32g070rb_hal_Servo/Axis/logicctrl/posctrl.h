#ifndef __POS_CTRL_H__
#define __POS_CTRL_H__

#include "paratbl.h"

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
void PosCtrlInit(pos_ctrl_t* pPosCtrl);
void PosCtrlCycle(pos_ctrl_t* pPosCtrl);
void PosCtrlIsr(pos_ctrl_t* pPosCtrl);

#ifdef __cplusplus
}
#endif

#endif
