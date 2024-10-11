#ifndef __POS_LOOP_H__
#define __POS_LOOP_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} pos_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void PosLoopCreat(pos_loop_t* pPosLoop, axis_e eAxisNo);
void PosLoopInit(pos_loop_t* pPosLoop);
void PosLoopCycle(pos_loop_t* pPosLoop);
void PosLoopIsr(pos_loop_t* pPosLoop);

#ifdef __cplusplus
}
#endif

#endif
