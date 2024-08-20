#ifndef __OPENLOOP_H__
#define __OPENLOOP_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} open_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void OpenLoopCreat(open_loop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopInit(open_loop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopEnter(open_loop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopExit(open_loop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopCycle(open_loop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopIsr(open_loop_t* pOpenLoop, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
