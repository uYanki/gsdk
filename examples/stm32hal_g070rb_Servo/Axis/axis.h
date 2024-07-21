#ifndef __AXIS_H__
#define __AXIS_H__

#include "paratbl.h"

#include "application/appmgr.h"
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
    axis_e       eAxisNo;
    app_mgr_t    sAppMgr;
    logic_ctrl_t sLogicCtrl;
    path_plan_t  sPathPlan;
    motdrv_t     sMotDrv;
} axis_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void AxisCreat(axis_t* pAxis);
void AxisInit(axis_t* pAxis);
void AxisCycle(axis_t* pAxis);
void AxisIsr(axis_t* pAxis);

#ifdef __cplusplus
}
#endif

#endif
