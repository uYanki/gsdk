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
    // 速度指令输入
    s32* ps32LogicSpdRef_i;

    // 鬼火模式
    u16* pu16LogicSpdPlanMode_i;

    // 加减速
    u16* pu16LogicSpdAccTime_i;
    u16* pu16LogicSpdDecTime_i;

    // 速度限制
    s32* ps32LogicSpdLimFwd_i;
    s32* ps32LogicSpdLimRev_i;

    // 速度指令输出
    s32* ps32PlanSpdRef_o;
} spd_plan_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdPlanCreat(spd_plan_t* pSpdPlan, axis_e eAxisNo);
void SpdPlanInit(spd_plan_t* pSpdPlan);
void SpdPlanCycle(spd_plan_t* pSpdPlan);
void SpdPlanIsr(spd_plan_t* pSpdPlan);

#ifdef __cplusplus
}
#endif

#endif
