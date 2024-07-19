#ifndef __MOT_DRV_H__
#define __MOT_DRV_H__

#include "gsdk.h"

#include "posloop.h"
#include "spdloop.h"
#include "trqloop.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    pos_loop_t sPosLoop;
    trq_loop_t sTrqLoop;
    spd_loop_t sSpdLoop;
} motdrv_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MotDrvCreat(motdrv_t* pTrqLoop, axis_e eAxisNo);
void MotDrvInit(motdrv_t* pTrqLoop, axis_e eAxisNo);
void MotDrvCycle(motdrv_t* pTrqLoop, axis_e eAxisNo);
void MotDrvIsr(motdrv_t* pTrqLoop, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
