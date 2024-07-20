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
} openloop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void OpenLoopCreat(openloop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopInit(openloop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopCycle(openloop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopIsr(openloop_t* pOpenLoop, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
