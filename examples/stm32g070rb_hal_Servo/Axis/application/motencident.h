#ifndef __MOT_ENC_IDENT_H__
#define __MOT_ENC_IDENT_H__

#include "paratbl.h"

#define CONFIG_CURSAMP_SW 0

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

/**
 * @brief 辨识状态
 */
typedef enum {
    MOT_ENC_IDENT_STATE_INIT,         ///< 初始化阶段
    MOT_ENC_IDENT_STATE_ADAPT_VOLT,   ///< 电压自适应阶段
    MOT_ENC_IDENT_STATE_DIR_CONFIRM,  ///< 修正电机正方向
    MOT_ENC_IDENT_STATE_RES_IND,      ///< 编码器分辨率辨识阶段
    MOT_ENC_IDENT_STATE_RETURN,       ///< 电机电角度回初阶段
    MOT_ENC_IDENT_STATE_CCW,          ///< 电机往正方向旋转（初始以电角度自增为正方向）
    MOT_ENC_IDENT_STATE_CW,           ///< 电机往反方向旋转（初始以电角度自减为正方向）
    MOT_ENC_IDENT_STATE_ANALYSE,      ///< 计算编码器安装偏置
    MOT_ENC_IDENT_STATE_ERR,          ///< 辨识过程中出错，错误处理阶段
    MOT_ENC_IDENT_STATE_FINISH,       ///< 辨识结束
} mot_enc_ident_state_e;

/**
 * @brief 方向匹配
 */
typedef enum {
    MOT_ENC_ROT_DIR_NONE,
    MOT_ENC_ROT_DIR_SAME,  ///< 编码器单圈位置反馈递增方向和电角度递增方向相同
    MOT_ENC_ROT_DIR_DIFF,  ///< 编码器单圈位置反馈递增方向和电角度递增方向相反
} mot_enc_ident_dir_e;

/**
 * @brief 报警类型
 */
typedef enum {
    MOT_ENC_IDENT_ERR_NONE,                 ///< 无报警
    MOT_ENC_IDENT_ERR_PHASE_OC,             ///< 单相过流
    MOT_ENC_IDENT_ERR_IDIQ_OC,              ///< ID/IQ 过流
    MOT_ENC_IDNET_ERR_BREAK,                ///< 未完成辨识流程意外断使能
    MOT_ENC_IDENT_ERR_POLE_PAIRS_TOO_MANY,  ///< 电机极对数过多
} mot_enc_ident_err_e;

/**
 * @brief 控制字
 */
typedef union {
    struct {
        u16 Enable   : 1;  ///< 0: disable; 1: enable;
        u16 AxisLock : 1;  ///< 0: d-axis; 1: q-axis;
        u16 _Resv    : 14;
    } u16Bit;
    u16 u16All;
} mot_enc_ctrlword_u;

typedef struct {
    // 轴状态
    u16* pu16AxisFSM_i;
    u32* pu32EncPos_i;
#if CONFIG_CURSAMP_SW
    s16* ps16IaPu_i;
    s16* ps16IbPu_i;
    s16* ps16IcPu_i;
#endif

    // 指令源&控制字
    u16*                pu16CtrlCmdSrc_io;
    ctrlword_u*         pu32CommCmd_o;
    mot_enc_ctrlword_u* pu16IdentCmd_i;

    // 指令源备份
    u16 u16CtrlCmdSrcBak;

    // 电机旋转方向辨识
    u16 u16CycleTimes;
    u16 u16EncPosIncTimes;
    u16 u16EncPosDecTimes;

    // 编码器最大分辨率辨识
    u32 u32EncPosMax;
    u32 u32EncPosPre;

    // 电机极对数&编码器安装偏置辨识
    u16 u16LockTimes;
    u32 u32EncPosCcwInit;
    u32 u32ZeroAngPos[32];
    u16 u16RotTimes;
    u16 u16CwRotTimes;

    // 开环给定指令
    s16* ps16UdRef_o;
    s16* ps16UqRef_o;
    u16* pu16ElecAngRef_o;

    // 辨识状态&结果
    u16* pu16IdentFSM_o;
    u16* pu16IdentErrType_o;
    u16* pu16IdentDirMatched_o;
    u32* pu32IdentEncRes_o;
    u32* pu32IdentEncOffset_o;
    u16* pu16IdentMotPolePairs_o;

} mot_enc_ident_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MotEncIdentCreat(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo);
void MotEncIdentInit(mot_enc_ident_t* pMotEncIdent);
void MotEncIdentEnter(mot_enc_ident_t* pMotEncIdent);
void MotEncIdentExit(mot_enc_ident_t* pMotEncIdent);
void MotEncIdentCycle(mot_enc_ident_t* pMotEncIdent);
void MotEncIdentIsr(mot_enc_ident_t* pMotEncIdent);

#ifdef __cplusplus
}
#endif

#endif
