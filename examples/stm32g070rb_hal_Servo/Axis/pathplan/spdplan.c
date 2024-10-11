#include "spdplan.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "spdplan"
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

void SpdPlanCreat(spd_plan_t* pSpdPlan, axis_e eAxisNo)
{
    pSpdPlan->ps32LogicSpdRef_i      = &P(eAxisNo).s32LogicSpdRef;
    
    pSpdPlan->pu16LogicSpdAccTime_i  = &P(eAxisNo).u16LogicSpdAccTime;
    pSpdPlan->pu16LogicSpdDecTime_i  = &P(eAxisNo).u16LogicSpdDecTime;

    pSpdPlan->pu16LogicSpdPlanMode_i = &P(eAxisNo).u16LogicSpdPlanMode;

    pSpdPlan->ps32LogicSpdLimFwd_i   = &P(eAxisNo).s32LogicSpdLimFwd;
    pSpdPlan->ps32LogicSpdLimRev_i   = &P(eAxisNo).s32LogicSpdLimRev;

    pSpdPlan->ps32PlanSpdRef_o = &P(eAxisNo).s32PlanSpdRef;
}

void SpdPlanInit(spd_plan_t* pSpdPlan)
{
}

void SpdPlanCycle(spd_plan_t* pSpdPlan)
{
    __set_s32(pSpdPlan->ps32PlanSpdRef_o, __get_s32(pSpdPlan->ps32LogicSpdRef_i));
}

void SpdPlanIsr(spd_plan_t* pSpdPlan)
{
}
