#include "axis.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "axis"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void AxisCreat(axis_t* pAxis, axis_e eAxisNo)
{
    AppMgrCreat(&pAxis->sAppMgr, eAxisNo);
    LogicCtrlCreat(&pAxis->sLogicCtrl, eAxisNo);
    PathPlanCreat(&pAxis->sPathPlan, eAxisNo);
    MotDrvCreat(&pAxis->sMotDrv, eAxisNo);
}

void AxisInit(axis_t* pAxis, axis_e eAxisNo)
{
    AppMgrInit(&pAxis->sAppMgr, eAxisNo);
    LogicCtrlInit(&pAxis->sLogicCtrl, eAxisNo);
    PathPlanInit(&pAxis->sPathPlan, eAxisNo);
    MotDrvInit(&pAxis->sMotDrv, eAxisNo);
}

void AxisCycle(axis_t* pAxis, axis_e eAxisNo)
{
    AppMgrCycle(&pAxis->sAppMgr, eAxisNo);
    LogicCtrlCycle(&pAxis->sLogicCtrl, eAxisNo);
    PathPlanCycle(&pAxis->sPathPlan, eAxisNo);
    MotDrvCycle(&pAxis->sMotDrv, eAxisNo);
}

void AxisIsr(axis_t* pAxis, axis_e eAxisNo)
{
    AppMgrIsr(&pAxis->sAppMgr, eAxisNo);
    LogicCtrlIsr(&pAxis->sLogicCtrl, eAxisNo);
    PathPlanIsr(&pAxis->sPathPlan, eAxisNo);
    MotDrvIsr(&pAxis->sMotDrv, eAxisNo);
}
