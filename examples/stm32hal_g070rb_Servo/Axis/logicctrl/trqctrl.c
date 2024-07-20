#include "trqctrl.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG                   "trqctrl"
#define LOG_LOCAL_LEVEL                 LOG_LEVEL_INFO

#define u16StopCmd_i(eAxisNo)           P(eAxisNo).u16StopCmd
#define u16StopMode_i(eAxisNo)          P(eAxisNo).u16StopMode
#define u16StopPlanMode_i(eAxisNo)      P(eAxisNo).u16StopPlanMode
#define u16EmStopTrqDigRef_i(eAxisNo)   P(eAxisNo).u16EmStopTrqDigRef

#define u16TrqRefSrc_i(eAxisNo)         P(eAxisNo).u16TrqRefSrc
#define s16TrqDigRef_i(eAxisNo)         P(eAxisNo).s16TrqDigRef
#define u16MultMotionEn_i(eAxisNo)      P(eAxisNo).u16MultMotionEn
#define s16TrqDigRefs_i(eAxisNo, Index) (&P(eAxisNo).s16TrqDigRef00)[Index]
#define u16TrqMulRefSel_i(eAxisNo)      P(eAxisNo).u16TrqMulRefSel

#define u16TrqLimSrc_i(eAxisNo)         P(eAxisNo).u16TrqLimSrc
#define u16TrqLimFwd_i(eAxisNo)         P(eAxisNo).u16TrqLimFwd
#define u16TrqLimRev_i(eAxisNo)         P(eAxisNo).u16TrqLimRev

#define s16UaiSi0_i(eAxisNo)            D.s16UaiSi0
#define s16UaiSi1_i(eAxisNo)            D.s16UaiSi1
#define u16AiTrqCoeff0_i(eAxisNo)       P(eAxisNo).u16AiTrqCoeff0
#define u16AiTrqCoeff1_i(eAxisNo)       P(eAxisNo).u16AiTrqCoeff1

#define s16LogicTrqRef_o(eAxisNo)       P(eAxisNo).s16LogicTrqRef
#define s16LogicTrqLimFwd_o(eAxisNo)    P(eAxisNo).s16LogicTrqLimFwd
#define s16LogicTrqLimRev_o(eAxisNo)    P(eAxisNo).s16LogicTrqLimRev

#define s32DrvTrqFb_i(eAxisNo)          P(eAxisNo).s32DrvTrqFb

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static void _TrqRefSel(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo)
{
    s16 s16TrqRef = 0;

    if (u16StopCmd_i(eAxisNo) && (stop_mode_e)u16StopMode_i(eAxisNo) == STOP_MODE_DB_TRQ)  // 反向转矩停机
    {
        if (s32DrvTrqFb_i(eAxisNo) > 0)
        {
            s16TrqRef = -u16EmStopTrqDigRef_i(eAxisNo);
        }
        else
        {
            s16TrqRef = u16EmStopTrqDigRef_i(eAxisNo);
        }
    }
    else
    {
        switch ((trq_ref_src_e)u16TrqRefSrc_i(eAxisNo))
        {
            case TRQ_REF_SRC_DIGITAL:
            {
                s16TrqRef = s16TrqDigRef_i(eAxisNo);
                break;
            }
            case TRQ_REF_SRC_MULTI_DIGITAL:
            {
                if (u16MultMotionEn_i(eAxisNo))  // 多段启动
                {
                    s16TrqRef = s16TrqDigRefs_i(eAxisNo, u16TrqMulRefSel_i(eAxisNo));
                }
                break;
            }
#if CONFIG_EXT_AI_NUM >= 1
            case TRQ_REF_SRC_AI_1:
            {
                s16TrqRef = (s16)((s32)s16UaiSi0_i(eAxisNo) * (s32)u16AiTrqCoeff0_i(eAxisNo) / 100);
                break;
            }
#endif
#if CONFIG_EXT_AI_NUM >= 2
            case TRQ_REF_SRC_AI_2:
            {
                s16TrqRef = (s16)((s32)s16UaiSi1_i(eAxisNo) * (s32)u16AiTrqCoeff1_i(eAxisNo) / 100);
                break;
            }
#endif
        }
    }

    // 逻辑层输出
    s16LogicTrqRef_o(eAxisNo) = (s16)s16TrqRef;
}

static void _TrqLimSel(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo)
{
    s16 s16LimFwd = 0;
    s16 s16LimRev = 0;

    switch ((spd_lim_src_e)u16TrqLimSrc_i(eAxisNo))
    {
        default:
        case TRQ_LIM_NONE:
        {
            s16LimFwd = s16LimRev = S16_MAX;
            break;
        }

        case TRQ_LIM_S:
        {
            s16LimFwd = (s16)u16TrqLimFwd_i(eAxisNo);
            s16LimRev = s16LimFwd;
            break;
        }

        case TRQ_LIM_M:
        {
            s16LimFwd = (s16)u16TrqLimFwd_i(eAxisNo);
            s16LimRev = (s16)u16TrqLimRev_i(eAxisNo);
            break;
        }

#if CONFIG_EXT_AI_NUM >= 1
        case TRQ_LIM_AI_S:
        {
            s16LimFwd = (s16)((s32)s16UaiSi0_i(eAxisNo) * (s32)u16AiTrqCoeff0_i(eAxisNo) / 100);
            s16LimRev = s16LimFwd;
            break;
        }
#endif

#if CONFIG_EXT_AI_NUM >= 2
        case TRQ_LIM_AI_M:
        {
            s16LimFwd = (s16)((s32)s16UaiSi0_i(eAxisNo) * (s32)u16AiTrqCoeff0_i(eAxisNo) / 100);
            s16LimRev = (s16)((s32)s16UaiSi1_i(eAxisNo) * (s32)u16AiTrqCoeff1_i(eAxisNo) / 100);
            break;
        }
#endif
    }

    // 逻辑层输出
    s16LogicTrqLimFwd_o(eAxisNo) = s16LimFwd;
    s16LogicTrqLimRev_o(eAxisNo) = s16LimRev;
}

void TrqCtrlCreat(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo)
{
}

void TrqCtrlInit(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo)
{
}

void TrqCtrlCycle(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo)
{
    // 转矩限制
    _TrqLimSel(pTrqCtrl, eAxisNo);
}

void TrqCtrlIsr(trq_ctrl_t* pTrqCtrl, axis_e eAxisNo)
{
    // 转矩指令
    _TrqRefSel(pTrqCtrl, eAxisNo);
}
