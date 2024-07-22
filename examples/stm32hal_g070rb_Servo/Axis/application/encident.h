#ifndef __MOT_ENC_IDENT_H__
#define __MOT_ENC_IDENT_H__

#include "paratbl.h"

/**
 * @brief 电机编码器参数辨识
 *
 * 使用前提：
 * - 1.电机空载
 * - 2.编码器读数正常
 * - 3.动力线UVW正常(无对地短路)
 *
 * 可辨识信息：
 * - 1.电机相序
 * - 2.编码器分辨率
 * - 3.电机极对数
 * - 4.编码器安装偏置
 */

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    // 电机旋转方向辨识
    uint16_t u16CycleTimes;
    uint16_t u16EncPosIncTimes;
    uint16_t u16EncPosDecTimes;

    // 编码器最大分辨率辨识
    uint32_t u32EncPosMax;
    uint32_t u32EncPosPre;

    // 电机极对数&编码器安装偏置辨识
    uint16_t u16LockTimes;
    uint32_t u32EncPosCcwInit;
    uint32_t u32ZeroAngPos[32];
    uint16_t u16RotTimes;
    uint16_t u16CwRotTimes;
} enc_ident_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void EncIdentCreat(enc_ident_t* pMotEncIdent, axis_e eAxisNo);
void EncIdentInit(enc_ident_t* pMotEncIdent, axis_e eAxisNo);
void EncIdentEnter(enc_ident_t* pMotEncIdent, axis_e eAxisNo);
void EncIdentExit(enc_ident_t* pMotEncIdent, axis_e eAxisNo);
void EncIdentCycle(enc_ident_t* pMotEncIdent, axis_e eAxisNo);
void EncIdentIsr(enc_ident_t* pMotEncIdent, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
