#ifndef __BSP_ENCODER_H__
#define __BSP_ENCODER_H__

#include "hall_enc.h"
#include "inc_enc.h"
#include "abs_enc.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef enum {
    ENC_HALL_UVW,      // 霍尔: UVW
    ENC_INC_AB,        // 增量式: AB
    ENC_INC_ABZ,       // 增量式: ABZ
    ENC_INC_ABZ_UVW,   // 增量式: ABZ+UVW
    ENC_ABS_BISS,      // 绝对值: BiSS
    ENC_ABS_TAMAGAWA,  // 绝对值: Tamagawa
    ENC_ABS_ENDAT,     // 绝对值: EnDat
    ENC_SINCOS,        // 正余弦
    ENC_RESOLVER       // 旋变
} encoder_type_e;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
