#ifndef __SPD_CTRL_H__
#define __SPD_CTRL_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
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
