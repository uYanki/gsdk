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
    TrqLoopCreat(&pMotDrv->sTrqLoop, eAxisNo);
}

void MotDrvInit(motdrv_t* pMotDrv, axis_e eAxisNo)
{
    PosLoopInit(&pMotDrv->sPosLoop, eAxisNo);
    SpdLoopInit(&pMotDrv->sSpdLoop, eAxisNo);
    TrqLoopInit(&pMotDrv->sTrqLoop, eAxisNo);
}

void MotDrvCycle(motdrv_t* pMotDrv, axis_e eAxisNo)
{
    PosLoopCycle(&pMotDrv->sPosLoop, eAxisNo);
    SpdLoopCycle(&pMotDrv->sSpdLoop, eAxisNo);
    TrqLoopCycle(&pMotDrv->sTrqLoop, eAxisNo);
}

void MotDrvIsr(motdrv_t* pMotDrv, axis_e eAxisNo)
{
    PosLoopIsr(&pMotDrv->sPosLoop, eAxisNo);
    SpdLoopIsr(&pMotDrv->sSpdLoop, eAxisNo);
    TrqLoopIsr(&pMotDrv->sTrqLoop, eAxisNo);
}