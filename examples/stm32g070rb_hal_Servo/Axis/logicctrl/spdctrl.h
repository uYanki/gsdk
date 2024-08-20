#ifndef __SPD_CTRL_H__
#define __SPD_CTRL_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    bool bJogFlag;  ///< 点动标志
} spd_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdCtrlCreat(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo);
void SpdCtrlInit(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo);
void SpdCtrlCycle(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo);
void SpdCtrlIsr(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
