#ifndef __HOMING_H__
#define __HOMING_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} homing_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void HomingCreat(homing_t* pHoming, axis_e eAxisNo);
void HomingInit(homing_t* pHoming);
void HomingCycle(homing_t* pHoming);
void HomingIsr(homing_t* pHoming);

#ifdef __cplusplus
}
#endif

#endif
