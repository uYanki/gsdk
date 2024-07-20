#include "motpos.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG              "motpos"
#define LOG_LOCAL_LEVEL            LOG_LEVEL_INFO

#define u16EncCmd_i(eAxisNo)       P(eAxisNo).u16EncCmd
#define s32EncTurns_i(eAxisNo)     P(eAxisNo).s32EncTurns
#define u16EncErrCode_i(eAxisNo)   P(eAxisNo).u16EncErrCode
#define u16EncComErrSum_i(eAxisNo) P(eAxisNo).u16EncComErrSum

typedef struct {
    u16 ErrorClear : 1;
    u16 TurnsReset : 1;
    u16 StateReset : 1;
    u16 _Resv      : 13;
} enc_cmd_t;

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MotPosCreat(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncCreat(&pMotPos->sIncEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncCreat(&pMotPos->sAbsEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncCreat(&pMotPos->sRotEnc);
#endif
}

void MotPosInit(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncInit(&pMotPos->sIncEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncInit(&pMotPos->sAbsEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncInit(&pMotPos->sRotEnc);
#endif
}

void MotPosCycle(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncCycle(&pMotPos->sIncEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncCycle(&pMotPos->sAbsEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncCycle(&pMotPos->sRotEnc);
#endif

    if (pMotPos->u16CmdPre ^ u16EncCmd_i(eAxisNo))  // 边沿触发
    {
        enc_cmd_t* pEncCmdPre = (enc_cmd_t*)&pMotPos->u16CmdPre;
        enc_cmd_t* pEncCmdCur = (enc_cmd_t*)&u16EncCmd_i(eAxisNo);

        // 编码器多圈值复位
        if (pEncCmdCur->TurnsReset && !pEncCmdPre->TurnsReset)
        {
            s32EncTurns_i(eAxisNo) = 0;
        }

        // 编码器故障状态复位
        if (pEncCmdCur->ErrorClear && !pEncCmdPre->ErrorClear)
        {
            u16EncErrCode_i(eAxisNo)   = 0;
            u16EncComErrSum_i(eAxisNo) = 0;
#if (CONFIG_ENCODER_TYPE == ENC_ABS)
            // AbsEncErrClr(&pMotPos->sAbsEnc);
#endif
        }

        if (pEncCmdCur->StateReset && !pEncCmdPre->StateReset)
        {
            // pMotPos->eState = ABS_ENC_STATE_INIT;
        }

        pMotPos->u16CmdPre = u16EncCmd_i(eAxisNo);
    }

#if (CONFIG_ENCODER_TYPE == ENC_ABS)
    // AbsEncWorkMode
#endif
}

void MotPosIsr(mot_pos_t* pMotPos, axis_e eAxisNo)
{
#if (CONFIG_ENCODER_TYPE == ENC_INC)
    IncEncIsr(&pMotPos->sIncEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ABS)
    AbsEncIsr(&pMotPos->sAbsEnc);
#elif (CONFIG_ENCODER_TYPE == ENC_ROT)
    RotEncIsr(&pMotPos->sRotEnc);
#endif
}
