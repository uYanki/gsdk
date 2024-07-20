#ifndef __AXIS_H__
#define __AXIS_H__

#include "paratbl.h"

#include "logicctrl/logicctrl.h"
#include "pathplan/pathplan.h"
#include "motdrv/motdrv.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    logic_ctrl_t sLogicCtrl;
    path_plan_t  sPathPlan;
    motdrv_t     sMotDrv;
} axis_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void AxisCreat(axis_t* pAxis, axis_e eAxisNo);
void AxisInit(axis_t* pAxis, axis_e eAxisNo);
void AxisCycle(axis_t* pAxis, axis_e eAxisNo);
void AxisIsr(axis_t* pAxis, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
