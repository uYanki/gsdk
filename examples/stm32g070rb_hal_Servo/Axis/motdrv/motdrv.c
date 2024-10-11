#include "motdrv.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "motdrv"
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

void MotDrvCreat(motdrv_t* pMotDrv, axis_e eAxisNo)
{
    PosLoopCreat(&pMotDrv->sPosLoop, eAxisNo);
    SpdLoopCreat(&pMotDrv->sSpdLoop, eAxisNo);
    CurLoopCreat(&pMotDrv->sCurLoop, eAxisNo);
}

void MotDrvInit(motdrv_t* pMotDrv)
{
    PosLoopInit(&pMotDrv->sPosLoop);
    SpdLoopInit(&pMotDrv->sSpdLoop);
    CurLoopInit(&pMotDrv->sCurLoop);
}

void MotDrvCycle(motdrv_t* pMotDrv)
{
    PosLoopCycle(&pMotDrv->sPosLoop);
    SpdLoopCycle(&pMotDrv->sSpdLoop);
    CurLoopCycle(&pMotDrv->sCurLoop);
}

void MotDrvIsr(motdrv_t* pMotDrv)
{
    PosLoopIsr(&pMotDrv->sPosLoop);
    SpdLoopIsr(&pMotDrv->sSpdLoop);
    CurLoopIsr(&pMotDrv->sCurLoop);
}