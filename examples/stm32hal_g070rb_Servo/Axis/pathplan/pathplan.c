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

void PathPlanCreat(path_plan_t* pTrqPlan, axis_e eAxisNo)
{
    PosPathIsr(&pPathPlan->sPosPath, eAxisNo);
    TrqPathIsr(&pPathPlan->sTrqPath, eAxisNo);
    SpdPathIsr(&pPathPlan->sSpdPath, eAxisNo);
}

void PathPlanInit(path_plan_t* pTrqPlan, axis_e eAxisNo)
{
    PosPathCreat(&pPathPlan->sPosPath, eAxisNo);
    TrqPathCreat(&pPathPlan->sTrqPath, eAxisNo);
    SpdPathCreat(&pPathPlan->sSpdPath, eAxisNo);
}

void PathPlanCycle(path_plan_t* pTrqPlan, axis_e eAxisNo)
{
    PosPathInit(&pPathPlan->sPosPath, eAxisNo);
    TrqPathInit(&pPathPlan->sTrqPath, eAxisNo);
    SpdPathInit(&pPathPlan->sSpdPath, eAxisNo);
}

void PathPlanIsr(path_plan_t* pTrqPlan, axis_e eAxisNo)
{
    PosPathCycle(&pPathPlan->sPosPath, eAxisNo);
    TrqPathCycle(&pPathPlan->sTrqPath, eAxisNo);
    SpdPathCycle(&pPathPlan->sSpdPath, eAxisNo);
}