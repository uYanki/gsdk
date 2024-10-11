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
    PosPlanCreat(&pPathPlan->sPosPlan, eAxisNo);
    TrqPlanCreat(&pPathPlan->sTrqPlan, eAxisNo);
    SpdPlanCreat(&pPathPlan->sSpdPlan, eAxisNo);
}

void PathPlanInit(path_plan_t* pPathPlan)
{
    PosPlanIsr(&pPathPlan->sPosPlan);
    TrqPlanIsr(&pPathPlan->sTrqPlan);
    SpdPlanIsr(&pPathPlan->sSpdPlan);
}

void PathPlanCycle(path_plan_t* pPathPlan)
{
    PosPlanInit(&pPathPlan->sPosPlan);
    TrqPlanInit(&pPathPlan->sTrqPlan);
    SpdPlanInit(&pPathPlan->sSpdPlan);
}

void PathPlanIsr(path_plan_t* pPathPlan)
{
    PosPlanCycle(&pPathPlan->sPosPlan);
    TrqPlanCycle(&pPathPlan->sTrqPlan);
    SpdPlanCycle(&pPathPlan->sSpdPlan);
}