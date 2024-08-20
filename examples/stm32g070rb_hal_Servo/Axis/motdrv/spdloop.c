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

#include "pid.h"

void SpdLoopIsr(spd_loop_t* pSpdLoop, axis_e eAxisNo)
{
	static PID_t sPID = {0};
	 
    MotSpdIsr(&pSpdLoop->sMotSpd, eAxisNo);

    if (u16AxisFSM_i(eAxisNo) == AXIS_STATE_ENABLE)
    {
        sPID.Kp = u16SpdLoopKp_i(eAxisNo) / 1000.f;
        sPID.Ki = u16SpdLoopKi_i(eAxisNo) / 1000.f;
        sPID.Kd = u16SpdLoopKd_i(eAxisNo) / 1000.f;

      //  arm_pid_init_q15(&pSpdLoop->sPID, false);
			
			sPID.ref = s32PlanSpdRef_i(eAxisNo) ; // 0.001rpm
			sPID.fbk = s32DrvSpdFb_i(eAxisNo) ; // 0.001rpm
			sPID.Ts  = 1;//1.f / P(eAxisNo).u16SpdLoopFreq;
			
			PID_Handler_Tustin(&sPID);
			
			s32DrvSpdRef_o(eAxisNo) = CLAMP(sPID.out,-15000,15000); 

       // s32DrvSpdRef_o(eAxisNo) = arm_pid_q15(&pSpdLoop->sPID, s32PlanSpdRef_i(eAxisNo) - s32DrvSpdFb_i(eAxisNo));
    }
    else
    {
      //  arm_pid_reset_q15(&pSpdLoop->sPID);
			
			sPID.prop = 0;
			sPID.inte = 0;
			sPID.deri = 0;
    }
}
