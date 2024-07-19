#ifndef __SPD_LOOP_H__
#define __SPD_LOOP_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
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
