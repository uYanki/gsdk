#ifndef __CUR_LOOP_H__
#define __CUR_LOOP_H__

#include "paratbl.h"

#include "motpos.h"
#include "cursamp.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    mot_pos_t  sMotPos;
    cur_samp_t sCurSamp;
} cur_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void CurLoopCreat(cur_loop_t* pCurLoop, axis_e eAxisNo);
void CurLoopInit(cur_loop_t* pCurLoop);
void CurLoopCycle(cur_loop_t* pCurLoop);
void CurLoopIsr(cur_loop_t* pCurLoop);

#ifdef __cplusplus
}
#endif

#endif
