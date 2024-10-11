#ifndef __MOT_POS_H__
#define __MOT_POS_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENC_INC             0
#define ENC_ABS             1
#define ENC_ROT             2

#define CONFIG_ENCODER_TYPE ENC_ABS

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    s32 s32PosDelt;
    s32 s32PosPre;
} abs_enc_t;

typedef struct {
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    inc_enc_t sIncEnc;  ///< 增量编码器
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    abs_enc_t sAbsEnc;  ///< 绝对值编码器
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    rot_enc_t sRotEnc;  ///< 旋转变压器编码器
#endif

    uint16_t u16CmdPre;

#if (CONFIG_ENCODER_TYPE == ENC_ABS)
    // AbsEncWorkModePre;
#endif

} mot_pos_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MotPosCreat(mot_pos_t* pCurLoop, axis_e eAxisNo);
void MotPosInit(mot_pos_t* pCurLoop);
void MotPosCycle(mot_pos_t* pCurLoop);
void MotPosIsr(mot_pos_t* pCurLoop);

#ifdef __cplusplus
}
#endif

#endif
