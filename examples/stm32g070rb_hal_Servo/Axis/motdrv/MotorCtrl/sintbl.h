#ifndef __SINTBL_H__
#define __SINTBL_H__

#include "gdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef s16 q15;
typedef s32 q31;

#define M_Q15_INVSQRT3 (q15)(0.5773502691 * Q15_MAX)
#define M_Q15_SQRT3_2  (q15)(0.86602540378443 * Q15_MAX)
#define M_Q15_1_2      (q15)(0.5 * Q15_MAX)

#define Q15Mul(q1, q2) (q15)((q31)(q1) * (q31)(q2) / Q15_MAX)

extern RO q15 c_q15SinTbl[];

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
