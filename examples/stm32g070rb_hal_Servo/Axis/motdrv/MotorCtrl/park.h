#ifndef __PARK_H__
#define __PARK_H__

#include "sintbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    q15* q15Alpha_i;    // [Pu] Q15
    q15* q15Beta_i;     // [Pu] Q15
    u16* u16ElecAng_i;  // [Pu] 0 ~ U16_MAX
    q15* q15D_o;        // [Pu] Q15
    q15* q15Q_o;        // [Pu] Q15
} park_t;

typedef struct {
    q15* q15D_i;        // [Pu] Q15
    q15* q15Q_i;        // [Pu] Q15
    u16* u16ElecAng_i;  // [Pu] 0 ~ U16_MAX
    q15* q15Alpha_o;    // [Pu] Q15
    q15* q15Beta_o;     // [Pu] Q15
} ipark_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

extern RO q15 c_q15SinTbl[];

/**
 *  d = alpha * sin + beta * cos;
 *  q = alpha * cos - beta * sin;
 */
static inline void Park(park_t* pPark)
{
    u16 u16Theta    = *pPark->u16ElecAng_i >> 6;
    q15 q15SinTheta = c_q15SinTbl[u16Theta & 0x03FF];
    q15 q15CosTheta = c_q15SinTbl[(u16Theta + 0x0100) & 0x03FF];

    *pPark->q15D_o = Q15Mul(*pPark->q15Alpha_i, q15SinTheta) + Q15Mul(*pPark->q15Beta_i, q15CosTheta);
    *pPark->q15Q_o = Q15Mul(*pPark->q15Alpha_i, q15CosTheta) - Q15Mul(*pPark->q15Beta_i, q15SinTheta);
}

/**
 *  alpha = d * sin + q * cos;
 *  beta  = d * cos - q * sin;
 */
static inline void RevPark(ipark_t* pRevPark)
{
    u16 u16Theta    = *pRevPark->u16ElecAng_i >> 6;
    q15 q15SinTheta = c_q15SinTbl[u16Theta & 0x03FF];
    q15 q15CosTheta = c_q15SinTbl[(u16Theta + 0x0100) & 0x03FF];

    *pRevPark->q15Alpha_o = Q15Mul(*pRevPark->q15D_i, q15SinTheta) + Q15Mul(*pRevPark->q15Q_i, q15CosTheta);
    *pRevPark->q15Beta_o  = Q15Mul(*pRevPark->q15D_i, q15CosTheta) - Q15Mul(*pRevPark->q15Q_i, q15SinTheta);
}

#ifdef __cplusplus
}
#endif

#endif
