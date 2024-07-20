#ifndef __SPD_PLAN_H__
#define __SPD_PLAN_H__

#include "paratbl.h"

#include "logicctrl/spdctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} spd_plan_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdPlanCreat(spd_plan_t* pSpdPlan, axis_e eAxisNo);
void SpdPlanInit(spd_plan_t* pSpdPlan, axis_e eAxisNo);
void SpdPlanCycle(spd_plan_t* pSpdPlan, axis_e eAxisNo);
void SpdPlanIsr(spd_plan_t* pSpdPlan, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
