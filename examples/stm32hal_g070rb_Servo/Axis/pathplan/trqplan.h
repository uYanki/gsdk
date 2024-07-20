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
    u16 _Resv;
} trq_plan_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void TrqPlanCreat(trq_plan_t* pTrqPlan, axis_e eAxisNo);
void TrqPlanInit(trq_plan_t* pTrqPlan, axis_e eAxisNo);
void TrqPlanCycle(trq_plan_t* pTrqPlan, axis_e eAxisNo);
void TrqPlanIsr(trq_plan_t* pTrqPlan, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
