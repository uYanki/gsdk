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
#define u32EncRes_i(eAxisNo)       (1 << 14)  // P(eAxisNo).u32EncRes
#define s64EncMultPos_o(eAxisNo)   P(eAxisNo).s64EncMultPos
#define s64DrvPosFb_o(eAxisNo)     P(eAxisNo).s64DrvPosFb

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
{}

static void AbsEncIsr(abs_enc_t* pAbsEnc, axis_e eAxisNo)
{
    AbsEncSyncSamp(eAxisNo);

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

    // P(eAxisNo).s32SpdDigRef14 = s64EncMultPos_o(eAxisNo);
}
