#ifndef __CLARKE_H__
#define __CLARKE_H__

#include "sintbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    q15* q15PhA_i;    // [Pu] Q15
    q15* q15PhB_i;    // [Pu] Q15
    q15* q15PhC_i;    // [Pu] Q15
    q15* q15Alpha_o;  // [Pu] Q15
    q15* q15Beta_o;   // [Pu] Q15
} clarke_t;

typedef struct {
    q15* q15Alpha_i;  // [Pu] Q15
    q15* q15Beta_i;   // [Pu] Q15
    q15* q15PhA_o;    // [Pu] Q15
    q15* q15PhB_o;    // [Pu] Q15
    q15* q15PhC_o;    // [Pu] Q15
} iclarke_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

extern RO q15 c_q15SinTbl[];

static inline void Clarke2(clarke_t* pClarke)
{
}

static inline void Clarke3(clarke_t* pClarke)
{
    *pClarke->q15Alpha_o = *pClarke->q15PhA_i;
    *pClarke->q15Beta_o  = Q15Mul(M_Q15_INVSQRT3, *pClarke->q15PhC_i - *pClarke->q15PhB_i);
}

static inline void RevClarke(iclarke_t* pRevClarke)
{
    *pRevClarke->q15PhA_o = *pRevClarke->q15Alpha_i;
    *pRevClarke->q15PhB_o = 0 - Q15Mul(M_Q15_1_2, *pRevClarke->q15Alpha_i) - Q15Mul(M_Q15_SQRT3_2, *pRevClarke->q15Beta_i);
    *pRevClarke->q15PhC_o = 0 - *pRevClarke->q15PhA_o - *pRevClarke->q15PhB_o;
}

#ifdef __cplusplus
}
#endif

#endif
