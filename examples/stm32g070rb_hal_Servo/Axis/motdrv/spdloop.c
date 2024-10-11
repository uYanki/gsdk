#include "spdloop.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "spdloop"
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

void SpdLoopCreat(spd_loop_t* pSpdLoop, axis_e eAxisNo)
{
    pSpdLoop->pu16SpdLoopKp_i = &P(eAxisNo).u16SpdLoopKp;
    pSpdLoop->pu16SpdLoopKi_i = &P(eAxisNo).u16SpdLoopKi;
    pSpdLoop->pu16SpdLoopKd_i = &P(eAxisNo).u16SpdLoopKd;

    pSpdLoop->ps32PlanSpdRef_i = &P(eAxisNo).s32PlanSpdRef;
    pSpdLoop->ps32DrvSpdFb_i   = &P(eAxisNo).s32DrvSpdFb;
    pSpdLoop->ps32DrvSpdRef_o  = &P(eAxisNo).s32DrvSpdRef;

    pSpdLoop->pu16AxisFSM_i = &P(eAxisNo).u16AxisFSM;

    MotSpdCreat(&pSpdLoop->sMotSpd, eAxisNo);
}

void SpdLoopInit(spd_loop_t* pSpdLoop)
{
    MotSpdInit(&pSpdLoop->sMotSpd);
}

void SpdLoopCycle(spd_loop_t* pSpdLoop)
{
    MotSpdCycle(&pSpdLoop->sMotSpd);
}

#include "pid.h"

void SpdLoopIsr(spd_loop_t* pSpdLoop)
{
    static PID_t sPID = {0};

    MotSpdIsr(&pSpdLoop->sMotSpd);

    if (__get_u16(pSpdLoop->pu16AxisFSM_i) == AXIS_STATE_ENABLE)
    {
        sPID.Kp = __get_u16(pSpdLoop->pu16SpdLoopKp_i) / 1000.f;
        sPID.Ki = __get_u16(pSpdLoop->pu16SpdLoopKi_i) / 1000.f;
        sPID.Kd = __get_u16(pSpdLoop->pu16SpdLoopKd_i) / 1000.f;

        //  arm_pid_init_q15(&pSpdLoop->sPID, false);

        sPID.ref = __get_s32(pSpdLoop->ps32PlanSpdRef_i);  // 0.001rpm
        sPID.fbk = __get_s32(pSpdLoop->ps32DrvSpdFb_i);    // 0.001rpm
        sPID.Ts  = 1;                                      // 1.f / P(eAxisNo).u16SpdLoopFreq;

        PID_Handler_Tustin(&sPID);

        __set_s32(pSpdLoop->ps32DrvSpdRef_o, CLAMP(sPID.out, -15000, 15000));

        // __set_s32(pSpdLoop->ps32DrvSpdRef_o, arm_pid_q15(&pSpdLoop->sPID, __get_s32(pSpdLoop->ps32PlanSpdRef_i) - __get_s32(pSpdLoop->ps32DrvSpdFb_i)));
    }
    else
    {
        //  arm_pid_reset_q15(&pSpdLoop->sPID);

        sPID.prop = 0;
        sPID.inte = 0;
        sPID.deri = 0;
    }
}
