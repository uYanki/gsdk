#ifndef __TRQ_LOOP_H__
#define __TRQ_LOOP_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} trq_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void TrqLoopCreat(trq_loop_t* pTrqLoop, axis_e eAxisNo);
void TrqLoopInit(trq_loop_t* pTrqLoop, axis_e eAxisNo);
void TrqLoopCycle(trq_loop_t* pTrqLoop, axis_e eAxisNo);
void TrqLoopIsr(trq_loop_t* pTrqLoop, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
