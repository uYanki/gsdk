#include "curloop.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "curloop"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void CurLoopCreat(cur_loop_t* pCurLoop, axis_e eAxisNo)
{
    MotPosCreat(&pCurLoop->sMotPos, eAxisNo);
    CurSampCreat(&pCurLoop->sCurSamp, eAxisNo);
}

void CurLoopInit(cur_loop_t* pCurLoop)
{
    MotPosInit(&pCurLoop->sMotPos);
    CurSampInit(&pCurLoop->sCurSamp);
}

void CurLoopCycle(cur_loop_t* pCurLoop)
{
    MotPosCycle(&pCurLoop->sMotPos);
    CurSampCycle(&pCurLoop->sCurSamp);
}

void CurLoopIsr(cur_loop_t* pCurLoop)
{
    MotPosIsr(&pCurLoop->sMotPos);
    CurSampIsr(&pCurLoop->sCurSamp);
}
