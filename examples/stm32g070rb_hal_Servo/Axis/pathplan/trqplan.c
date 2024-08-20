#include "trqplan.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG             "trqplan"
#define LOG_LOCAL_LEVEL           LOG_LEVEL_INFO

#define s16LogicTrqRef_i(eAxisNo) P(eAxisNo).s16LogicTrqRef
#define s32DrvTrqRef_o(eAxisNo)   P(eAxisNo).s32DrvTrqRef

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void TrqPlanCreat(trq_plan_t* pTrqPlan, axis_e eAxisNo)
{
}

void TrqPlanInit(trq_plan_t* pTrqPlan, axis_e eAxisNo)
{
}

void TrqPlanCycle(trq_plan_t* pTrqPlan, axis_e eAxisNo)
{
}

void TrqPlanIsr(trq_plan_t* pTrqPlan, axis_e eAxisNo)
{
    s32DrvTrqRef_o(eAxisNo) = s16LogicTrqRef_i(eAxisNo);
}
