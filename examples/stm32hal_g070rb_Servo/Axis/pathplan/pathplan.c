#include "pathplan.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "pathplan"
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

void PathPlanCreat(path_plan_t* pPathPlan, axis_e eAxisNo)
{
    PosPlanIsr(&pPathPlan->sPosPlan, eAxisNo);
    TrqPlanIsr(&pPathPlan->sTrqPlan, eAxisNo);
    SpdPlanIsr(&pPathPlan->sSpdPlan, eAxisNo);
}

void PathPlanInit(path_plan_t* pPathPlan, axis_e eAxisNo)
{
    PosPlanCreat(&pPathPlan->sPosPlan, eAxisNo);
    TrqPlanCreat(&pPathPlan->sTrqPlan, eAxisNo);
    SpdPlanCreat(&pPathPlan->sSpdPlan, eAxisNo);
}

void PathPlanCycle(path_plan_t* pPathPlan, axis_e eAxisNo)
{
    PosPlanInit(&pPathPlan->sPosPlan, eAxisNo);
    TrqPlanInit(&pPathPlan->sTrqPlan, eAxisNo);
    SpdPlanInit(&pPathPlan->sSpdPlan, eAxisNo);
}

void PathPlanIsr(path_plan_t* pPathPlan, axis_e eAxisNo)
{
    PosPlanCycle(&pPathPlan->sPosPlan, eAxisNo);
    TrqPlanCycle(&pPathPlan->sTrqPlan, eAxisNo);
    SpdPlanCycle(&pPathPlan->sSpdPlan, eAxisNo);
}