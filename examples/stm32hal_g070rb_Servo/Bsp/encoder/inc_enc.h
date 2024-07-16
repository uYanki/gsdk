#ifndef __INC_ENC_H__
#define __INC_ENC_H__

#include "axis.h"

typedef struct {
} inc_enc_t;

extern void IncEncCreat(inc_enc_t* pEnc, axis_e eAxisNo);
extern void IncEncInit(inc_enc_t* pEnc, axis_e eAxisNo);
extern void IncEncCycle(inc_enc_t* pEnc, axis_e eAxisNo);
extern void IncEncIsr(inc_enc_t* pEnc, axis_e eAxisNo);

#endif
