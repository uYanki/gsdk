#ifndef __MOT_SPD_H__
#define __MOT_SPD_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} mot_spd_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MotSpdCreat(mot_spd_t* pMotSpd, axis_e eAxisNo);
void MotSpdInit(mot_spd_t* pMotSpd, axis_e eAxisNo);
void MotSpdCycle(mot_spd_t* pMotSpd, axis_e eAxisNo);
void MotSpdIsr(mot_spd_t* pMotSpd, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
