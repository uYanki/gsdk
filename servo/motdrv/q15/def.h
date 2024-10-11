#ifndef __def
#define __def

#include "stdint.h"
#include "stdio.h"

typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;
typedef s16      q15;
typedef s32      q31;

#define RO             const

#define Q15_MAX        32768

#define M_Q15_INVSQRT3 (q15)(0.5773502691 * Q15_MAX)
#define M_Q15_SQRT3_2  (q15)(0.86602540378443 * Q15_MAX)
#define M_Q15_1_2      (q15)(0.5 * Q15_MAX)

#define Q15Mul(q1, q2) (q15)((q31)(q1) * (q31)(q2) / Q15_MAX)

#endif