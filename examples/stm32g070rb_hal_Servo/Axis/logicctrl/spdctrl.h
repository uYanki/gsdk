#ifndef __SPD_CTRL_H__
#define __SPD_CTRL_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef union {
    struct {
        u16 JogFwd : 1;
        u16 JogRev : 1;
        u16 _Resv  : 14;
    } u16Bit;
    u16 u16All;
} jog_cmd_u;

typedef struct {
    u16* pu16StopCmd_i;
    u16* pu16StopPlanMode_i;

    // 数字速度指令
    u16* pu16SpdRefSrc_i;
    s16* ps16SpdDigRef_i;

    // 多段数字速度指令
    u16* pu16MultMotionEn_i;
    u16* pu16SpdMulRefSel_i;
    s16* ps16SpdDigRefs_i;

    // 模拟量速度指令
    s16* ps16UaiSi0_i;
    s16* ps16UaiSi1_i;
    u16* pu16AiSpdCoeff0_i;
    u16* pu16AiSpdCoeff1_i;

    // 点动指令
    jog_cmd_u* pu16JogCmd_i;
    u16*       pu16JogSpdRef_i;
    u16*       pu16JogAccDecTime_i;
    bool       bJogFlag;  ///< 点动标志

    // 速度限制
    u16* pu16SpdLimSrc_i;
    u16* pu16SpdLimFwd_i;
    u16* pu16SpdLimRev_i;
    s16* ps16MotMaxSpeed_i;

    // 加速度
    u16* pu16SpdAccTime_i;
    u16* pu16SpdDecTime_i;
    u16* pu16SpdPlanMode_i;

    // 逻辑层输出
    s32* ps32LogicSpdRef_o;
    u16* pu16LogicSpdAccTime_o;
    u16* pu16LogicSpdDecTime_o;
    u16* pu16LogicSpdPlanMode_o;
    s32* ps32LogicSpdLimFwd_o;
    s32* ps32LogicSpdLimRev_o;

    // 速度反馈
    s32* ps32DrvSpdFb;

} spd_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SpdCtrlCreat(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo);
void SpdCtrlInit(spd_ctrl_t* pSpdCtrl);
void SpdCtrlCycle(spd_ctrl_t* pSpdCtrl);
void SpdCtrlIsr(spd_ctrl_t* pSpdCtrl);

#ifdef __cplusplus
}
#endif

#endif
