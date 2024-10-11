#ifndef __CUR_SAMP_H__
#define __CUR_SAMP_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} cur_samp_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void CurSampCreat(cur_samp_t* pCurSamp, axis_e eAxisNo);
void CurSampInit(cur_samp_t* pCurSamp);
void CurSampCycle(cur_samp_t* pCurSamp);
void CurSampIsr(cur_samp_t* pCurSamp);

#ifdef __cplusplus
}
#endif

#endif
