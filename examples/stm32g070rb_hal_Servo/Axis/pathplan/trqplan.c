#include "trqplan.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "trqplan"
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

void TrqPlanCreat(trq_plan_t* pTrqPlan, axis_e eAxisNo)
{
    pTrqPlan->ps16LogicTrqRef_i = &P(eAxisNo).s16LogicTrqRef;

    pTrqPlan->ps32DrvTrqRef_o = &P(eAxisNo).s32DrvTrqRef;
}

void TrqPlanInit(trq_plan_t* pTrqPlan)
{
}

void TrqPlanCycle(trq_plan_t* pTrqPlan)
{
}

void TrqPlanIsr(trq_plan_t* pTrqPlan)
{
    __set_s32(pTrqPlan->ps32DrvTrqRef_o, __get_s16(pTrqPlan->ps16LogicTrqRef_i));
}
