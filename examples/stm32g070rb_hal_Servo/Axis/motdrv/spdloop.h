#ifndef __SPD_LOOP_H__
#define __SPD_LOOP_H__

#include "paratbl.h"
#include "motspd.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    arm_pid_instance_q15 sPID;

    mot_spd_t sMotSpd;

    u16* pu16SpdLoopKp_i;
    u16* pu16SpdLoopKi_i;
    u16* pu16SpdLoopKd_i;
    s32* ps32PlanSpdRef_i;
    s32* ps32DrvSpdFb_i;
    s32* ps32DrvSpdRef_o;
    u16* pu16AxisFSM_i;

} spd_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdLoopCreat(spd_loop_t* pSpdLoop, axis_e eAxisNo);
void SpdLoopInit(spd_loop_t* pSpdLoop);
void SpdLoopCycle(spd_loop_t* pSpdLoop);
void SpdLoopIsr(spd_loop_t* pSpdLoop);

#ifdef __cplusplus
}
#endif

#endif
