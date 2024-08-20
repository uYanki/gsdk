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

void CurLoopInit(cur_loop_t* pCurLoop, axis_e eAxisNo)
{
    MotPosInit(&pCurLoop->sMotPos, eAxisNo);
    CurSampInit(&pCurLoop->sCurSamp, eAxisNo);
}

void CurLoopCycle(cur_loop_t* pCurLoop, axis_e eAxisNo)
{
    MotPosCycle(&pCurLoop->sMotPos, eAxisNo);
    CurSampCycle(&pCurLoop->sCurSamp, eAxisNo);
}

void CurLoopIsr(cur_loop_t* pCurLoop, axis_e eAxisNo)
{
    MotPosIsr(&pCurLoop->sMotPos, eAxisNo);
    CurSampIsr(&pCurLoop->sCurSamp, eAxisNo);
}
