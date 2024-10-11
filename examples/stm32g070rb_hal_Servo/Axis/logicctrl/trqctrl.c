#include "trqctrl.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "trqctrl"
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

static void _TrqRefSel(trq_ctrl_t* pTrqCtrl)
{
    s16 s16TrqRef = 0;

    if (__get_u16(pTrqCtrl->pu16StopCmd_i) && (stop_mode_e)__get_u16(pTrqCtrl->pu16StopMode_i) == STOP_MODE_DB_TRQ)  // 反向转矩停机
    {
        if (__get_s32(pTrqCtrl->ps32DrvTrqFb_i) > 0)
        {
            s16TrqRef = -__get_u16(pTrqCtrl->pu16EmStopTrqDigRef_i);
        }
        else
        {
            s16TrqRef = __get_u16(pTrqCtrl->pu16EmStopTrqDigRef_i);
        }
    }
    else
    {
        switch ((trq_ref_src_e)__get_u16(pTrqCtrl->pu16TrqRefSrc_i))
        {
            case TRQ_REF_SRC_DIGITAL: {
                s16TrqRef = __get_s16(pTrqCtrl->ps16TrqDigRef_i);
                break;
            }
            case TRQ_REF_SRC_MULTI_DIGITAL: {
                if (__get_u16(pTrqCtrl->pu16MultMotionEn_i))  // 多段启动
                {
                    s16TrqRef = __get_u16(pTrqCtrl->ps16TrqDigRefs_i + __get_u16(pTrqCtrl->pu16TrqMulRefSel_i));
                }
                break;
            }
#if CONFIG_EXT_AI_NUM >= 1
            case TRQ_REF_SRC_AI_1: {
                s16TrqRef = (s16)((s32)__get_s16(pTrqCtrl->ps16UaiSi0_i) * (s32)__get_u16(pTrqCtrl->pu16AiTrqCoeff0_i) / 100);
                break;
            }
#endif
#if CONFIG_EXT_AI_NUM >= 2
            case TRQ_REF_SRC_AI_2: {
                s16TrqRef = (s16)((s32)__get_s16(pTrqCtrl->ps16UaiSi1_i) * (s32)__get_u16(pTrqCtrl->pu16AiTrqCoeff1_i) / 100);
                break;
            }
#endif
        }
    }

    // 逻辑层输出
    __set_s16(pTrqCtrl->ps16LogicTrqRef_o, s16TrqRef);
}

static void _TrqLimSel(trq_ctrl_t* pTrqCtrl)
{
    s16 s16LimFwd = 0;
    s16 s16LimRev = 0;

    switch ((spd_lim_src_e)__get_u16(pTrqCtrl->pu16TrqLimSrc_i))
    {
        default:
        case TRQ_LIM_NONE: {
            s16LimFwd = s16LimRev = S16_MAX;
            break;
        }

        case TRQ_LIM_S: {
            s16LimFwd = (s16)__get_u16(pTrqCtrl->pu16TrqLimFwd_i);
            s16LimRev = s16LimFwd;
            break;
        }

        case TRQ_LIM_M: {
            s16LimFwd = (s16)__get_u16(pTrqCtrl->pu16TrqLimFwd_i);
            s16LimRev = (s16)__get_u16(pTrqCtrl->pu16TrqLimRev_i);
            break;
        }

#if CONFIG_EXT_AI_NUM >= 1
        case TRQ_LIM_AI_S: {
            s16LimFwd = (s16)((s32)__get_s16(pTrqCtrl->ps16UaiSi0_i) * (s32)__get_u16(pTrqCtrl->pu16AiTrqCoeff0_i) / 100);
            s16LimRev = s16LimFwd;
            break;
        }
#endif

#if CONFIG_EXT_AI_NUM >= 2
        case TRQ_LIM_AI_M: {
            s16LimFwd = (s16)((s32)__get_s16(pTrqCtrl->ps16UaiSi0_i) * (s32)__get_u16(pTrqCtrl->pu16AiTrqCoeff0_i) / 100);
            s16LimRev = (s16)((s32)__get_s16(pTrqCtrl->ps16UaiSi1_i) * (s32)__get_u16(pTrqCtrl->pu16AiTrqCoeff1_i) / 100);
            break;
        }
#endif
    }

    // 逻辑层输出
    __set_s16(pTrqCtrl->ps16LogicTrqLimFwd_o, s16LimFwd);
    __set_s16(pTrqCtrl->ps16LogicTrqLimRev_o, s16LimRev);
}

void TrqCtrlCreat(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo)
{
    pTrqCtrl->pu16StopCmd_i         = &P(eAxisNo).u16StopCmd;
    pTrqCtrl->pu16StopMode_i        = &P(eAxisNo).u16StopMode;
    pTrqCtrl->pu16StopPlanMode_i    = &P(eAxisNo).u16StopPlanMode;
    pTrqCtrl->pu16EmStopTrqDigRef_i = &P(eAxisNo).u16EmStopTrqDigRef;

    pTrqCtrl->pu16TrqRefSrc_i    = &P(eAxisNo).u16TrqRefSrc;
    pTrqCtrl->ps16TrqDigRef_i    = &P(eAxisNo).s16TrqDigRef;
    pTrqCtrl->pu16MultMotionEn_i = &P(eAxisNo).u16MultMotionEn;
    pTrqCtrl->ps16TrqDigRefs_i   = &P(eAxisNo).s16TrqDigRef00;
    pTrqCtrl->pu16TrqMulRefSel_i = &P(eAxisNo).u16TrqMulRefSel;

    pTrqCtrl->pu16TrqLimSrc_i = &P(eAxisNo).u16TrqLimSrc;
    pTrqCtrl->pu16TrqLimFwd_i = &P(eAxisNo).u16TrqLimFwd;
    pTrqCtrl->pu16TrqLimRev_i = &P(eAxisNo).u16TrqLimRev;

    pTrqCtrl->ps16UaiSi0_i      = &D.s16UaiSi0;
    pTrqCtrl->ps16UaiSi1_i      = &D.s16UaiSi1;
    pTrqCtrl->pu16AiTrqCoeff0_i = &P(eAxisNo).u16AiTrqCoeff0;
    pTrqCtrl->pu16AiTrqCoeff1_i = &P(eAxisNo).u16AiTrqCoeff1;

    pTrqCtrl->ps16LogicTrqRef_o    = &P(eAxisNo).s16LogicTrqRef;
    pTrqCtrl->ps16LogicTrqLimFwd_o = &P(eAxisNo).s16LogicTrqLimFwd;
    pTrqCtrl->ps16LogicTrqLimRev_o = &P(eAxisNo).s16LogicTrqLimRev;

    pTrqCtrl->ps32DrvTrqFb_i = &P(eAxisNo).s32DrvTrqFb;
}

void TrqCtrlInit(trq_ctrl_t* pTrqCtrl)
{
}

void TrqCtrlCycle(trq_ctrl_t* pTrqCtrl)
{
    // 转矩限制
    _TrqLimSel(pTrqCtrl);
}

void TrqCtrlIsr(trq_ctrl_t* pTrqCtrl)
{
    // 转矩指令
    _TrqRefSel(pTrqCtrl);
}
