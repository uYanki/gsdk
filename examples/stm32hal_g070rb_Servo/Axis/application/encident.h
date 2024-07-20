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
} enc_ident_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void EncIdentCreat(enc_ident_t* pEncident, axis_e eAxisNo);
void EncIdentInit(enc_ident_t* pEncident, axis_e eAxisNo);
void EncIdentEnter(enc_ident_t* pEncident, axis_e eAxisNo);
void EncIdentExit(enc_ident_t* pEncident, axis_e eAxisNo);
void EncIdentCycle(enc_ident_t* pEncident, axis_e eAxisNo);
void EncIdentIsr(enc_ident_t* pEncident, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
