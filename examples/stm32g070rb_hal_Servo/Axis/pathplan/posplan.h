#ifndef __POS_PLAN_H__
#define __POS_PLAN_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} pos_plan_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void PosPlanCreat(pos_plan_t* pPosPlan, axis_e eAxisNo);
void PosPlanInit(pos_plan_t* pPosPlan);
void PosPlanCycle(pos_plan_t* pPosPlan);
void PosPlanIsr(pos_plan_t* pPosPlan);

#ifdef __cplusplus
}
#endif

#endif
