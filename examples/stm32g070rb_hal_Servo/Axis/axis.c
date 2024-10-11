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
    AppMgrInit(&pAxis->sAppMgr);
    LogicCtrlInit(&pAxis->sLogicCtrl);
    PathPlanInit(&pAxis->sPathPlan);
    MotDrvInit(&pAxis->sMotDrv);
}

void AxisCycle(axis_t* pAxis)
{
    AppMgrCycle(&pAxis->sAppMgr);
    LogicCtrlCycle(&pAxis->sLogicCtrl);
    PathPlanCycle(&pAxis->sPathPlan);
    MotDrvCycle(&pAxis->sMotDrv);
}

void AxisIsr(axis_t* pAxis)
{
    AppMgrIsr(&pAxis->sAppMgr);
    LogicCtrlIsr(&pAxis->sLogicCtrl);
    PathPlanIsr(&pAxis->sPathPlan);
    MotDrvIsr(&pAxis->sMotDrv);
}
