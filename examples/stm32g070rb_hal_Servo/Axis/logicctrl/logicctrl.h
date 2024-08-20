#ifndef __LOGIC_CTRL_H__
#define __LOGIC_CTRL_H__

#include "paratbl.h"

#include "posctrl.h"
#include "spdctrl.h"
#include "trqctrl.h"
#include "cmdctrl.h"
#include "homing.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    pos_ctrl_t sPosCtrl;
    trq_ctrl_t sTrqCtrl;
    spd_ctrl_t sSpdCtrl;
    cmd_ctrl_t sCmdCtrl;
    homing_t   sHoming;
} logic_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void LogicCtrlCreat(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo);
void LogicCtrlInit(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo);
void LogicCtrlCycle(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo);
void LogicCtrlIsr(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
