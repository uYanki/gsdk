#include "spdplan.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG                  "spdplan"
#define LOG_LOCAL_LEVEL                LOG_LEVEL_INFO

#define s32LogicSpdRef_i(eAxisNo)      P(eAxisNo).s32LogicSpdRef
#define u16LogicSpdAccTime_i(eAxisNo)  P(eAxisNo).u16LogicSpdAccTime
#define u16LogicSpdDecTime_i(eAxisNo)  P(eAxisNo).u16LogicSpdDecTime
#define u16LogicSpdPlanMode_i(eAxisNo) P(eAxisNo).u16LogicSpdPlanMode
#define s32LogicSpdLimFwd_i(eAxisNo)   P(eAxisNo).s32LogicSpdLimFwd
#define s32LogicSpdLimRev_i(eAxisNo)   P(eAxisNo).s32LogicSpdLimRev

#define s32PlanSpdRef_o(eAxisNo)       P(eAxisNo).s32PlanSpdRef

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdPlanCreat(spd_plan_t* pSpdPlan, axis_e eAxisNo)
{
}

void SpdPlanInit(spd_plan_t* pSpdPlan, axis_e eAxisNo)
{
}

void SpdPlanCycle(spd_plan_t* pSpdPlan, axis_e eAxisNo)
{
    s32PlanSpdRef_o(eAxisNo) = s32LogicSpdRef_i(eAxisNo); 
}

void SpdPlanIsr(spd_plan_t* pSpdPlan, axis_e eAxisNo)
{
}
