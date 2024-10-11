#ifndef __TRQ_PLAN_H__
#define __TRQ_PLAN_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    s16* ps16LogicTrqRef_i;
    s32* ps32DrvTrqRef_o;
} trq_plan_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void TrqPlanCreat(trq_plan_t* pTrqPlan, axis_e eAxisNo);
void TrqPlanInit(trq_plan_t* pTrqPlan);
void TrqPlanCycle(trq_plan_t* pTrqPlan);
void TrqPlanIsr(trq_plan_t* pTrqPlan);

#ifdef __cplusplus
}
#endif

#endif
