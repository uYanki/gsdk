#ifndef __PATH_PLAN_H__
#define __PATH_PLAN_H__

#include "paratbl.h"

#include "posplan.h"
#include "spdplan.h"
#include "trqplan.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    pos_plan_t sPosPlan;
    trq_plan_t sTrqPlan;
    spd_plan_t sSpdPlan;
} path_plan_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void PathPlanCreat(path_plan_t* pPlanPlan, axis_e eAxisNo);
void PathPlanInit(path_plan_t* pPlanPlan, axis_e eAxisNo);
void PathPlanCycle(path_plan_t* pPlanPlan, axis_e eAxisNo);
void PathPlanIsr(path_plan_t* pPlanPlan, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
