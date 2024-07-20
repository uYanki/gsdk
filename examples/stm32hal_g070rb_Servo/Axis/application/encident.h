#ifndef __ENCIDENT_H__
#define __ENCIDENT_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} encident_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void EncidentCreat(encident_t* pEncident, axis_e eAxisNo);
void EncidentInit(encident_t* pEncident, axis_e eAxisNo);
void EncidentCycle(encident_t* pEncident, axis_e eAxisNo);
void EncidentIsr(encident_t* pEncident, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
