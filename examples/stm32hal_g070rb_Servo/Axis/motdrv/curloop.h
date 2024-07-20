#ifndef __CUR_LOOP_H__
#define __CUR_LOOP_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} cur_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void CurLoopCreat(cur_loop_t* pCurLoop, axis_e eAxisNo);
void CurLoopInit(cur_loop_t* pCurLoop, axis_e eAxisNo);
void CurLoopCycle(cur_loop_t* pCurLoop, axis_e eAxisNo);
void CurLoopIsr(cur_loop_t* pCurLoop, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
