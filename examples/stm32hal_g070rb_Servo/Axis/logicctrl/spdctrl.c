#include "spdctrl.h"

#define SPD_RPM(rpm) (10 * rpm)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG                   "spdctrl"
#define LOG_LOCAL_LEVEL                 LOG_LEVEL_INFO

#define u16StopCmd_i(eAxisNo)           P(eAxisNo).u16StopCmd
#define u16StopPlanMode_i(eAxisNo)      P(eAxisNo).u16StopPlanMode

#define u16SpdRefSrc_i(eAxisNo)         P(eAxisNo).u16SpdRefSrc
#define s16SpdDigRef_i(eAxisNo)         P(eAxisNo).s32SpdDigRef
#define u16MultMotionEn_i(eAxisNo)      P(eAxisNo).u16MultMotionEn
#define u16SpdMulRefSel_i(eAxisNo)      P(eAxisNo).u16SpdMulRefSel
#define s16SpdDigRefs_i(eAxisNo, Index) (&P(eAxisNo).s32SpdDigRef00)[Index]
#define u16SpdAccTime_i(eAxisNo)        P(eAxisNo).u16SpdAccTime
#define u16SpdDecTime_i(eAxisNo)        P(eAxisNo).u16SpdDecTime
#define u16SpdPlanMode_i(eAxisNo)       P(eAxisNo).u16SpdPlanMode

#define u16JogFwdCmd_i(eAxisNo)         (P(eAxisNo).u16JogCmd & BV(0))
#define u16JogRevCmd_i(eAxisNo)         (P(eAxisNo).u16JogCmd & BV(1))
#define u16JogSpdRef_i(eAxisNo)         P(eAxisNo).u16JogSpdRef
#define u16JogAccDecTime_i(eAxisNo)     P(eAxisNo).u16JogAccDecTime

#define u16SpdLimSrc_i(eAxisNo)         P(eAxisNo).u16SpdLimSrc
#define u16SpdLimFwd_i(eAxisNo)         P(eAxisNo).u16SpdLimFwd
#define u16SpdLimRev_i(eAxisNo)         P(eAxisNo).u16SpdLimRev
#define s32MotMaxSpeed_i(eAxisNo)       SPD_RPM(4000)  // 额定速度

#define s16UaiSi0_i(eAxisNo)            D.s16UaiSi0
#define s16UaiSi1_i(eAxisNo)            D.s16UaiSi1
#define u16AiSpdCoeff0_i(eAxisNo)       P(eAxisNo).u16AiSpdCoeff00
#define u16AiSpdCoeff1_i(eAxisNo)       P(eAxisNo).u16AiSpdCoeff01

#define s32LogicSpdRef_o(eAxisNo)       P(eAxisNo).s32LogicSpdRef
#define u16LogicSpdAccTime_o(eAxisNo)   P(eAxisNo).u16LogicSpdAccTime
#define u16LogicSpdDecTime_o(eAxisNo)   P(eAxisNo).u16LogicSpdDecTime
#define u16LogicSpdPlanMode_o(eAxisNo)  P(eAxisNo).u16LogicSpdPlanMode
#define s32LogicSpdLimFwd_o(eAxisNo)    P(eAxisNo).s32LogicSpdLimFwd
#define s32LogicSpdLimRev_o(eAxisNo)    P(eAxisNo).s32LogicSpdLimRev

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static void _SpdRefSel(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
    s32 s32SpdRef = 0;

    if (u16StopCmd_i(eAxisNo))  // 停机
    {
        s32SpdRef = 0;
    }
    else if (u16JogFwdCmd_i(eAxisNo) || u16JogRevCmd_i(eAxisNo) || pSpdCtrl->bJogFlag)  // 点动
    {
        if (u16JogFwdCmd_i(eAxisNo))  // 正向(高优先级)
        {
            pSpdCtrl->bJogFlag = true;

            s32SpdRef = +(s32)u16JogSpdRef_i(eAxisNo);
        }
        else if (u16JogRevCmd_i(eAxisNo))  // 反向
        {
            pSpdCtrl->bJogFlag = true;

            s32SpdRef = -(s32)u16JogSpdRef_i(eAxisNo);
        }
        else
        {
            if (labs(P(eAxisNo).s32DrvSpdFb) < SPD_RPM(10))
            {
                // 速度降到10转以内再退出点动模式
                pSpdCtrl->bJogFlag = false;
            }

            s32SpdRef = 0;
        }
    }
    else  // 通用控制
    {
        switch ((spd_ref_src_e)u16SpdRefSrc_i(eAxisNo))
        {
            case SPD_REF_SRC_DIGITAL:  // 数字量
            {
                s32SpdRef = s16SpdDigRef_i(eAxisNo);
                break;
            }
            case SPD_REF_SRC_MULTI_DIGITAL:  // 多段数字量
            {
                if (u16MultMotionEn_i(eAxisNo))  // 多段启动
                {
                    s32SpdRef = s16SpdDigRefs_i(eAxisNo, u16SpdMulRefSel_i(eAxisNo));
                }

                break;
            }
#if CONFIG_EXT_AI_NUM >= 1
            case SPD_REF_SRC_AI_1:  // 模拟量
            {
                s32SpdRef = (s32)s16UaiSi0_i(eAxisNo) * (s32)u16AiSpdCoeff0_i(eAxisNo) / 100;
                break;
            }
#endif
#if CONFIG_EXT_AI_NUM >= 2
            case SPD_REF_SRC_AI_2:  // 模拟量
            {
                s32SpdRef = (s32)s16UaiSi1_i(eAxisNo) * (s32)u16AiSpdCoeff1_i(eAxisNo) / 100;
                break;
            }
#endif
        }
    }

    // 逻辑层输出
    s32LogicSpdRef_o(eAxisNo) = s32SpdRef;
}

static void _SpdLimSel(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
    s32 s32LimFwd = 0;
    s32 s32LimRev = 0;

    switch ((spd_lim_src_e)u16SpdLimSrc_i(eAxisNo))
    {
        default:
        case SPD_LIM_NONE:  // 默认
        {
            s32LimFwd = s32MotMaxSpeed_i(eAxisNo);  // 额定速度
            s32LimRev = s32LimFwd;
        }
        case SPD_LIM_S:  // 单数字量
        {
            s32LimFwd = (s32)u16SpdLimFwd_i(eAxisNo);
            s32LimRev = s32LimFwd;
            break;
        }
        case SPD_LIM_M:  // 双数字量
        {
            s32LimFwd = (s32)u16SpdLimFwd_i(eAxisNo);
            s32LimRev = (s32)u16SpdLimRev_i(eAxisNo);
            break;
        }
#if CONFIG_EXT_AI_NUM >= 1
        case SPD_LIM_AI_S:  // 单模拟量
        {
            s32LimFwd = (s32)s16UaiSi0_i(eAxisNo) * (s32)u16AiSpdCoeff0_i(eAxisNo) / 100;
            s32LimRev = s32LimRev;
            break;
        }
#endif
#if CONFIG_EXT_AI_NUM >= 2
        case SPD_LIM_AI_M:  // 双模拟量
        {
            s32LimFwd = (s32)s16UaiSi0_i(eAxisNo) * (s32)u16AiSpdCoeff0_i(eAxisNo) / 100;
            s32LimRev = (s32)s16UaiSi1_i(eAxisNo) * (s32)u16AiSpdCoeff1_i(eAxisNo) / 100;
            break;
        }
#endif
    }

    // 逻辑层输出
    s32LogicSpdLimFwd_o(eAxisNo) = s32LimFwd;
    s32LogicSpdLimRev_o(eAxisNo) = s32LimRev;
}

static void _SpdAccDecTimeSel(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
    u16 u16SpdAccTime = 0;
    u16 u16SpdDecTime = 0;

    if (u16StopCmd_i(eAxisNo))  // 停机
    {
        u16SpdDecTime = u16SpdDecTime_i(eAxisNo);
    }
    else if (u16JogFwdCmd_i(eAxisNo) || u16JogRevCmd_i(eAxisNo))  // 点动
    {
        u16SpdAccTime = u16JogAccDecTime_i(eAxisNo);
        u16SpdDecTime = u16JogAccDecTime_i(eAxisNo);
    }
    else  // 默认值
    {
        u16SpdAccTime = u16SpdAccTime_i(eAxisNo);
        u16SpdDecTime = u16SpdDecTime_i(eAxisNo);
    }

    // 逻辑层输出
    u16LogicSpdAccTime_o(eAxisNo) = u16SpdAccTime;
    u16LogicSpdDecTime_o(eAxisNo) = u16SpdDecTime;
}

static void _SpdPlanModeSel(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
    spd_plan_mode_e ePlanMode;

    if (u16StopCmd_i(eAxisNo))  // 停机
    {
        ePlanMode = (spd_plan_mode_e)u16StopPlanMode_i(eAxisNo);
    }
    else if (u16JogFwdCmd_i(eAxisNo) || u16JogRevCmd_i(eAxisNo))  // 点动
    {
        ePlanMode = SPD_PLAN_MODE_SLOPE;
    }
    else  // 默认
    {
        ePlanMode = (spd_plan_mode_e)u16SpdPlanMode_i(eAxisNo);
    }

    // 逻辑层输出
    u16LogicSpdPlanMode_o(eAxisNo) = (u16)ePlanMode;
}

void SpdCtrlCreat(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
}

void SpdCtrlInit(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
}

void SpdCtrlCycle(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
    // 速度限制
    _SpdLimSel(pSpdCtrl, eAxisNo);

    // 加减速时间
    _SpdAccDecTimeSel(pSpdCtrl, eAxisNo);

    // 速度规划模式
    _SpdPlanModeSel(pSpdCtrl, eAxisNo);
}

void SpdCtrlIsr(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
    // 速度指令
    _SpdRefSel(pSpdCtrl, eAxisNo);
}
