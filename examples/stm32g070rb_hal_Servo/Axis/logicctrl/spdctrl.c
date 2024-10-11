#include "spdctrl.h"

#define SPD_RPM(rpm)    (10 * rpm)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "spdctrl"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static void _SpdRefSel(spd_ctrl_t* pSpdCtrl)
{
    s32 s32SpdRef = 0;

    if (__get_u16(pSpdCtrl->pu16StopCmd_i))  // 停机
    {
        s32SpdRef = 0;
    }
    else if (pSpdCtrl->pu16JogCmd_i->u16Bit.JogFwd || pSpdCtrl->pu16JogCmd_i->u16Bit.JogRev || pSpdCtrl->bJogFlag)  // 点动
    {
        if (pSpdCtrl->pu16JogCmd_i->u16Bit.JogFwd)  // 正向(高优先级)
        {
            pSpdCtrl->bJogFlag = true;

            s32SpdRef = +(s32)__get_u16(pSpdCtrl->pu16JogSpdRef_i);
        }
        else if (pSpdCtrl->pu16JogCmd_i->u16Bit.JogRev)  // 反向
        {
            pSpdCtrl->bJogFlag = true;

            s32SpdRef = -(s32)__get_u16(pSpdCtrl->pu16JogSpdRef_i);
        }
        else
        {
            if (labs(__get_s32(pSpdCtrl->ps32DrvSpdFb)) < SPD_RPM(10))
            {
                // 速度降到10转以内再退出点动模式
                pSpdCtrl->bJogFlag = false;
            }

            s32SpdRef = 0;
        }
    }
    else  // 通用控制
    {
        switch ((spd_ref_src_e)__get_u16(pSpdCtrl->pu16SpdRefSrc_i))
        {
            case SPD_REF_SRC_DIGITAL:  // 数字量
            {
                s32SpdRef = __get_s16(pSpdCtrl->ps16SpdDigRef_i);
                break;
            }
            case SPD_REF_SRC_MULTI_DIGITAL:  // 多段数字量
            {
                if (__get_u16(pSpdCtrl->pu16MultMotionEn_i))  // 多段启动
                {
                    s32SpdRef = __get_u16(pSpdCtrl->ps16SpdDigRefs_i + __get_u16(pSpdCtrl->pu16SpdMulRefSel_i));
                }

                break;
            }
#if CONFIG_EXT_AI_NUM >= 1
            case SPD_REF_SRC_AI_1:  // 模拟量
            {
                s32SpdRef = (s32)__get_s16(pSpdCtrl->ps16UaiSi0_i) * (s32)__get_u16(pSpdCtrl->pu16AiSpdCoeff0_i) / 100;
                break;
            }
#endif
#if CONFIG_EXT_AI_NUM >= 2
            case SPD_REF_SRC_AI_2:  // 模拟量
            {
                s32SpdRef = (s32)__get_s16(pSpdCtrl->ps16UaiSi1_i) * (s32)__get_u16(pSpdCtrl->pu16AiSpdCoeff1_i) / 100;
                break;
            }
#endif
        }
    }

    // 逻辑层输出
    __set_s32(pSpdCtrl->ps32LogicSpdRef_o, s32SpdRef);
}

static void _SpdLimSel(spd_ctrl_t* pSpdCtrl)
{
    s32 s32LimFwd = 0;
    s32 s32LimRev = 0;

    switch ((spd_lim_src_e)__get_u16(pSpdCtrl->pu16SpdLimSrc_i))
    {
        default:
        case SPD_LIM_NONE:  // 默认
        {
            s32LimFwd = SPD_RPM(__get_s32(pSpdCtrl->ps16MotMaxSpeed_i));  // 额定速度
            s32LimRev = s32LimFwd;
        }
        case SPD_LIM_S:  // 单数字量
        {
            s32LimFwd = (s32)__get_u16(pSpdCtrl->pu16SpdLimFwd_i);
            s32LimRev = s32LimFwd;
            break;
        }
        case SPD_LIM_M:  // 双数字量
        {
            s32LimFwd = (s32)__get_u16(pSpdCtrl->pu16SpdLimFwd_i);
            s32LimRev = (s32)__get_u16(pSpdCtrl->pu16SpdLimRev_i);
            break;
        }
#if CONFIG_EXT_AI_NUM >= 1
        case SPD_LIM_AI_S:  // 单模拟量
        {
            s32LimFwd = (s32)__get_s16(pSpdCtrl->ps16UaiSi0_i) * (s32)__get_u16(pSpdCtrl->pu16AiSpdCoeff0_i) / 100;
            s32LimRev = s32LimRev;
            break;
        }
#endif
#if CONFIG_EXT_AI_NUM >= 2
        case SPD_LIM_AI_M:  // 双模拟量
        {
            s32LimFwd = (s32)__get_s16(pSpdCtrl->ps16UaiSi0_i) * (s32)__get_u16(pSpdCtrl->pu16AiSpdCoeff0_i) / 100;
            s32LimRev = (s32)__get_s16(pSpdCtrl->ps16UaiSi1_i) * (s32)__get_u16(pSpdCtrl->pu16AiSpdCoeff1_i) / 100;
            break;
        }
#endif
    }

    // 逻辑层输出
    __set_s32(pSpdCtrl->ps32LogicSpdLimFwd_o, s32LimFwd);
    __set_s32(pSpdCtrl->ps32LogicSpdLimRev_o, s32LimRev);
}

static void _SpdAccDecTimeSel(spd_ctrl_t* pSpdCtrl)
{
    u16 u16SpdAccTime = 0;
    u16 u16SpdDecTime = 0;

    if (__get_u16(pSpdCtrl->pu16StopCmd_i))  // 停机
    {
        u16SpdDecTime = __get_u16(pSpdCtrl->pu16SpdDecTime_i);
    }
    else if (pSpdCtrl->pu16JogCmd_i->u16Bit.JogFwd || pSpdCtrl->pu16JogCmd_i->u16Bit.JogRev)  // 点动
    {
        u16SpdAccTime = __get_u16(pSpdCtrl->pu16JogAccDecTime_i);
        u16SpdDecTime = __get_u16(pSpdCtrl->pu16JogAccDecTime_i);
    }
    else  // 默认值
    {
        u16SpdAccTime = __get_u16(pSpdCtrl->pu16SpdAccTime_i);
        u16SpdDecTime = __get_u16(pSpdCtrl->pu16SpdDecTime_i);
    }

    // 逻辑层输出
    __set_u16(pSpdCtrl->pu16LogicSpdAccTime_o, u16SpdAccTime);
    __set_u16(pSpdCtrl->pu16LogicSpdDecTime_o, u16SpdDecTime);
}

static void _SpdPlanModeSel(spd_ctrl_t* pSpdCtrl)
{
    spd_plan_mode_e ePlanMode;

    if (__get_u16(pSpdCtrl->pu16StopCmd_i))  // 停机
    {
        ePlanMode = (spd_plan_mode_e)__get_u16(pSpdCtrl->pu16StopPlanMode_i);
    }
    else if (pSpdCtrl->pu16JogCmd_i->u16Bit.JogFwd || pSpdCtrl->pu16JogCmd_i->u16Bit.JogRev)  // 点动
    {
        ePlanMode = SPD_PLAN_MODE_SLOPE;
    }
    else  // 默认
    {
        ePlanMode = (spd_plan_mode_e)__get_u16(pSpdCtrl->pu16SpdPlanMode_i);
    }

    // 逻辑层输出
    __set_u16(pSpdCtrl->pu16LogicSpdPlanMode_o, (u16)ePlanMode);
}

void SpdCtrlCreat(spd_ctrl_t* pSpdCtrl, axis_e eAxisNo)
{
    pSpdCtrl->pu16StopCmd_i      = &P(eAxisNo).u16StopCmd;
    pSpdCtrl->pu16StopPlanMode_i = &P(eAxisNo).u16StopPlanMode;

    pSpdCtrl->pu16SpdRefSrc_i = &P(eAxisNo).u16SpdRefSrc;
    pSpdCtrl->ps16SpdDigRef_i = &P(eAxisNo).s32SpdDigRef;

    pSpdCtrl->pu16MultMotionEn_i = &P(eAxisNo).u16MultMotionEn;
    pSpdCtrl->pu16SpdMulRefSel_i = &P(eAxisNo).u16SpdMulRefSel;
    pSpdCtrl->ps16SpdDigRefs_i   = &P(eAxisNo).s32SpdDigRef00;

    pSpdCtrl->pu16SpdAccTime_i = &P(eAxisNo).u16SpdAccTime;
    pSpdCtrl->pu16SpdDecTime_i = &P(eAxisNo).u16SpdDecTime;

    pSpdCtrl->pu16SpdPlanMode_i = &P(eAxisNo).u16SpdPlanMode;

    pSpdCtrl->pu16JogCmd_i        = &P(eAxisNo).u16JogCmd;
    pSpdCtrl->pu16JogSpdRef_i     = &P(eAxisNo).u16JogSpdRef;
    pSpdCtrl->pu16JogAccDecTime_i = &P(eAxisNo).u16JogAccDecTime;

    pSpdCtrl->pu16SpdLimSrc_i   = &P(eAxisNo).u16SpdLimSrc;
    pSpdCtrl->pu16SpdLimFwd_i   = &P(eAxisNo).u16SpdLimFwd;
    pSpdCtrl->pu16SpdLimRev_i   = &P(eAxisNo).u16SpdLimRev;
    pSpdCtrl->ps16MotMaxSpeed_i = &P(eAxisNo).u16MotSpdMax;  // 额定速度

    pSpdCtrl->ps16UaiSi0_i      = &D.s16UaiSi0;
    pSpdCtrl->ps16UaiSi1_i      = &D.s16UaiSi1;
    pSpdCtrl->pu16AiSpdCoeff0_i = &P(eAxisNo).u16AiSpdCoeff00;
    pSpdCtrl->pu16AiSpdCoeff1_i = &P(eAxisNo).u16AiSpdCoeff01;

    pSpdCtrl->ps32LogicSpdRef_o      = &P(eAxisNo).s32LogicSpdRef;
    pSpdCtrl->pu16LogicSpdAccTime_o  = &P(eAxisNo).u16LogicSpdAccTime;
    pSpdCtrl->pu16LogicSpdDecTime_o  = &P(eAxisNo).u16LogicSpdDecTime;
    pSpdCtrl->pu16LogicSpdPlanMode_o = &P(eAxisNo).u16LogicSpdPlanMode;
    pSpdCtrl->ps32LogicSpdLimFwd_o   = &P(eAxisNo).s32LogicSpdLimFwd;
    pSpdCtrl->ps32LogicSpdLimRev_o   = &P(eAxisNo).s32LogicSpdLimRev;

    pSpdCtrl->ps32DrvSpdFb = &P(eAxisNo).s32DrvSpdFb;
}

void SpdCtrlInit(spd_ctrl_t* pSpdCtrl)
{
}

void SpdCtrlCycle(spd_ctrl_t* pSpdCtrl)
{
    // 速度限制
    _SpdLimSel(pSpdCtrl);

    // 加减速时间
    _SpdAccDecTimeSel(pSpdCtrl);

    // 速度规划模式
    _SpdPlanModeSel(pSpdCtrl);
}

void SpdCtrlIsr(spd_ctrl_t* pSpdCtrl)
{
    // 速度指令
    _SpdRefSel(pSpdCtrl);
}
