#include "spdloop.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG            "spdloop"
#define LOG_LOCAL_LEVEL          LOG_LEVEL_INFO

#define u16SpdLoopKp_i(eAxisNo)  P(eAxisNo).u16SpdLoopKp
#define u16SpdLoopKi_i(eAxisNo)  P(eAxisNo).u16SpdLoopKi
#define u16SpdLoopKd_i(eAxisNo)  P(eAxisNo).u16SpdLoopKd

#define s32PlanSpdRef_i(eAxisNo) P(eAxisNo).s32PlanSpdRef
#define s32DrvSpdFb_i(eAxisNo)   P(eAxisNo).s32DrvSpdFb
#define s32DrvSpdRef_o(eAxisNo)  P(eAxisNo).s32DrvSpdRef

#define u16AxisFSM_i(eAxisNo)    P(eAxisNo).u16AxisFSM

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdLoopCreat(spd_loop_t* pSpdLoop, axis_e eAxisNo)
{
    MotSpdCreat(&pSpdLoop->sMotSpd, eAxisNo);
}

void SpdLoopInit(spd_loop_t* pSpdLoop, axis_e eAxisNo)
{
    MotSpdInit(&pSpdLoop->sMotSpd, eAxisNo);
}

void SpdLoopCycle(spd_loop_t* pSpdLoop, axis_e eAxisNo)
{
    MotSpdCycle(&pSpdLoop->sMotSpd, eAxisNo);
}

void SpdLoopIsr(spd_loop_t* pSpdLoop, axis_e eAxisNo)
{
    MotSpdIsr(&pSpdLoop->sMotSpd, eAxisNo);

    if (u16AxisFSM_i(eAxisNo) == AXIS_RUN)
    {
        pSpdLoop->sPID.Kp = u16SpdLoopKp_i(eAxisNo);
        pSpdLoop->sPID.Ki = u16SpdLoopKi_i(eAxisNo);
        pSpdLoop->sPID.Kd = u16SpdLoopKd_i(eAxisNo);

        arm_pid_init_q15(&pSpdLoop->sPID, false);

        s32DrvSpdRef_o(eAxisNo) = arm_pid_q15(&pSpdLoop->sPID, s32PlanSpdRef_i(eAxisNo) - s32DrvSpdFb_i(eAxisNo));
    }
    else
    {
        arm_pid_reset_q15(&pSpdLoop->sPID);
    }
}
