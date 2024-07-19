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
    LogicCtrlCreat(&pAxis->pLogicCtrl, eAxisNo);
    PathPlanCreat(&pAxis->pTrqPlan, eAxisNo);
    MotDrvCreat(&pAxis->pTrqLoop, eAxisNo);
}

void AxisInit(axis_t* pAxis, axis_e eAxisNo)
{
    LogicCtrlInit(&pAxis->pLogicCtrl, eAxisNo);
    PathPlanInit(&pAxis->pTrqPlan, eAxisNo);
    MotDrvInit(&pAxis->pTrqLoop, eAxisNo);
}

void AxisCycle(axis_t* pAxis, axis_e eAxisNo)
{
    LogicCtrlCycle(&pAxis->pLogicCtrl, eAxisNo);
    PathPlanCycle(&pAxis->pTrqPlan, eAxisNo);
    MotDrvCycle(&pAxis->pTrqLoop, eAxisNo);
}

void AxisIsr(axis_t* pAxis, axis_e eAxisNo)
{
    LogicCtrlIsr(&pAxis->pLogicCtrl, eAxisNo);
    PathPlanIsr(&pAxis->pTrqPlan, eAxisNo);
    MotDrvIsr(&pAxis->pTrqLoop, eAxisNo);
}
