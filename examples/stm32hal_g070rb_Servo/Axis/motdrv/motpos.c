#include "motpos.h"
#include "spi_mt6701.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG              "motpos"
#define LOG_LOCAL_LEVEL            LOG_LEVEL_INFO

#define u16EncCmd_i(eAxisNo)       P(eAxisNo).u16EncCmd
#define s32EncTurns_i(eAxisNo)     P(eAxisNo).s32EncTurns
#define u16EncErrCode_i(eAxisNo)   P(eAxisNo).u16EncErrCode
#define u16EncComErrSum_i(eAxisNo) P(eAxisNo).u16EncComErrSum

#define s32EncTurns_o(eAxisNo)     P(eAxisNo).s32EncTurns
#define u32EncPos_io(eAxisNo)      P(eAxisNo).u32EncPos
#define u32EncPosInit_i(eAxisNo)   P(eAxisNo).u32EncPosInit
#define u32EncRes_i(eAxisNo)       P(eAxisNo).u32EncRes
#define s64EncMultPos_o(eAxisNo)   P(eAxisNo).s64EncMultPos
#define s64DrvPosFb_o(eAxisNo)     P(eAxisNo).s64DrvPosFb
#define u16MotPolePairs_i(eAxisNo) P(eAxisNo).u16MotPolePairs
#define u32EncPosOffset_i(eAxisNo) P(eAxisNo).u32EncPosOffset  // 机械角偏置
#define u16ElecAngleFb_o(eAxisNo)  P(eAxisNo).u16ElecAngleFb   // 电角度反馈

typedef union {
    u16 u16All;
    struct {
        u16 ErrorClear : 1;
        u16 TurnsReset : 1;
        u16 StateReset : 1;
        u16 _Resv      : 13;
    } u16Bit;
} enc_cmd_u;

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

extern spi_mt6701_t mt6701;

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static void AbsEncSyncSamp(axis_e eAxisNo)
{
    u32EncPos_io(eAxisNo) = MT6701_ReadAngle(&mt6701);
}

static void AbsEncCreat(abs_enc_t* pAbsEnc, axis_e eAxisNo)
{}

static void AbsEncInit(abs_enc_t* pAbsEnc, axis_e eAxisNo)
{
    AbsEncSyncSamp(eAxisNo);
}

static void AbsEncCycle(abs_enc_t* pAbsEnc, axis_e eAxisNo)
{
    PeriodicTask(250 * UNIT_US, {
        // 1. 位置采样

        AbsEncSyncSamp(eAxisNo);

        // 2. 计算电角度

        u32 u32EncPosOffset = u32EncPosOffset_i(eAxisNo);  // 机械角偏置
        u32 u32EncPos       = u32EncPos_io(eAxisNo);

        if (u32EncPos >= u32EncPosOffset)
        {
            u32EncPos -= u32EncPosOffset;
        }
        else
        {
            u32EncPos = u32EncRes_i(eAxisNo) - u32EncPosOffset + u32EncPos;
        }

        // 电角度标幺 [0,u32EncRes) => [0,65536)
        u32 u32ElecAng = u32EncPos * u16MotPolePairs_i(eAxisNo);

        if (u32EncRes_i(eAxisNo) <= 65536)
        {
            u32ElecAng *= 65536 / u32EncRes_i(eAxisNo);
        }
        else
        {
            u32ElecAng /= u32EncRes_i(eAxisNo) / 65536;
        }

        u16ElecAngleFb_o(eAxisNo) = (u16)u32ElecAng;
    });
}

// 正方向定义: 编码器递增方向和电角度递增方向相同。若方向相反，则任意调换电机的两根相线
static void AbsEncIsr(abs_enc_t* pAbsEnc, axis_e eAxisNo)
{
    // 1. 单圈值

    s32 s32PosPreCur = u32EncPos_io(eAxisNo);

    // 2. 圈数(跳变沿检测)

    pAbsEnc->s32PosDelt = s32PosPreCur - pAbsEnc->s32PosPre;
    pAbsEnc->s32PosPre  = s32PosPreCur;

    if (pAbsEnc->s32PosDelt > (s32)(u32EncRes_i(eAxisNo) >> 2))
    {
        s32EncTurns_o(eAxisNo)--;
    }
    else if (pAbsEnc->s32PosDelt < -(s32)(u32EncRes_i(eAxisNo) >> 2))
    {
        s32EncTurns_o(eAxisNo)++;
    }

    // 3. 多圈值

    s64EncMultPos_o(eAxisNo) = (s64)s32EncTurns_o(eAxisNo) * (s64)u32EncRes_i(eAxisNo) + (s64)u32EncPos_io(eAxisNo);

    PeriodicTask(500 * UNIT_US, {
        static s64 PosPre = 0;

        s32 Spd = 2000 * 10 * 60 * (s32)(s64EncMultPos_o(eAxisNo) - PosPre) / (s32)u32EncRes_i(eAxisNo);  // 0.1 rpm

        P(eAxisNo).s32DrvSpdFb = P(eAxisNo).s32DrvSpdFb * 0.15 + Spd * 0.85;

        PosPre = s64EncMultPos_o(eAxisNo);
    });
}

void MotPosCreat(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncCreat(&pMotPos->sIncEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncCreat(&pMotPos->sAbsEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncCreat(&pMotPos->sRotEnc, eAxisNo);
#endif
}

void MotPosInit(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncInit(&pMotPos->sIncEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncInit(&pMotPos->sAbsEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncInit(&pMotPos->sRotEnc, eAxisNo);
#endif
}

void MotPosCycle(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncCycle(&pMotPos->sIncEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncCycle(&pMotPos->sAbsEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncCycle(&pMotPos->sRotEnc, eAxisNo);
#endif

    if (pMotPos->u16CmdPre ^ u16EncCmd_i(eAxisNo))  // 边沿触发
    {
        enc_cmd_u* pEncCmdPre = (enc_cmd_u*)&pMotPos->u16CmdPre;
        enc_cmd_u* pEncCmdCur = (enc_cmd_u*)&u16EncCmd_i(eAxisNo);

        // 编码器多圈值复位
        if (pEncCmdCur->u16Bit.TurnsReset && !pEncCmdPre->u16Bit.TurnsReset)
        {
            s32EncTurns_i(eAxisNo) = 0;
        }

        // 编码器故障状态复位
        if (pEncCmdCur->u16Bit.ErrorClear && !pEncCmdPre->u16Bit.ErrorClear)
        {
            u16EncErrCode_i(eAxisNo)   = 0;
            u16EncComErrSum_i(eAxisNo) = 0;
#if (CONFIG_ENCODER_TYPE == ENC_ABS)
            // AbsEncErrClr(&pMotPos->sAbsEnc);
#endif
        }

        if (pEncCmdCur->u16Bit.StateReset && !pEncCmdPre->u16Bit.StateReset)
        {
            // pMotPos->eState = ABS_ENC_STATE_INIT;
        }

        pMotPos->u16CmdPre = pEncCmdCur->u16All;
    }

#if (CONFIG_ENCODER_TYPE == ENC_ABS)
    // AbsEncWorkMode
#endif
}

void MotPosIsr(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncIsr(&pMotPos->sIncEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncIsr(&pMotPos->sAbsEnc, eAxisNo);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncIsr(&pMotPos->sRotEnc, eAxisNo);
#endif

    P(eAxisNo).s64DrvPosFb = s64EncMultPos_o(eAxisNo);
}
