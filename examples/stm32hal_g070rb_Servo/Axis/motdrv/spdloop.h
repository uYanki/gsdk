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
} spd_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdLoopCreat(spd_loop_t* pSpdLoop, axis_e eAxisNo);
void SpdLoopInit(spd_loop_t* pSpdLoop, axis_e eAxisNo);
void SpdLoopCycle(spd_loop_t* pSpdLoop, axis_e eAxisNo);
void SpdLoopIsr(spd_loop_t* pSpdLoop, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
