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

static axis_e s_eAxisNo = AXIS_0;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void AxisCreat(axis_t* pAxis)
{
    pAxis->eAxisNo = s_eAxisNo++;

    AppMgrCreat(&pAxis->sAppMgr, pAxis->eAxisNo);
    LogicCtrlCreat(&pAxis->sLogicCtrl, pAxis->eAxisNo);
    PathPlanCreat(&pAxis->sPathPlan, pAxis->eAxisNo);
    MotDrvCreat(&pAxis->sMotDrv, pAxis->eAxisNo);
}

void AxisInit(axis_t* pAxis)
{
    AppMgrInit(&pAxis->sAppMgr, pAxis->eAxisNo);
    LogicCtrlInit(&pAxis->sLogicCtrl, pAxis->eAxisNo);
    PathPlanInit(&pAxis->sPathPlan, pAxis->eAxisNo);
    MotDrvInit(&pAxis->sMotDrv, pAxis->eAxisNo);
}

void AxisCycle(axis_t* pAxis)
{
    AppMgrCycle(&pAxis->sAppMgr, pAxis->eAxisNo);
    LogicCtrlCycle(&pAxis->sLogicCtrl, pAxis->eAxisNo);
    PathPlanCycle(&pAxis->sPathPlan, pAxis->eAxisNo);
    MotDrvCycle(&pAxis->sMotDrv, pAxis->eAxisNo);
}

void AxisIsr(axis_t* pAxis)
{
    AppMgrIsr(&pAxis->sAppMgr, pAxis->eAxisNo);
    LogicCtrlIsr(&pAxis->sLogicCtrl, pAxis->eAxisNo);
    PathPlanIsr(&pAxis->sPathPlan, pAxis->eAxisNo);
    MotDrvIsr(&pAxis->sMotDrv, pAxis->eAxisNo);
}
