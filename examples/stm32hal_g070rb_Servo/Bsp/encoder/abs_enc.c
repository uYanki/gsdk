#include "absEnc.h"
#include "paratbl.h"

/**
 * @brief M/T
 */
s32 SpdCalc(abs_enc_t* pEnc, axis_e eAxisNo)
{
#define ZOOM 10

    s32        Z  = pEnc->u32EncRes;  // 编码器分辨率
    static s64 M1 = 0;                // 检测周期内编码器脉冲计数

    s32        f0 = 4000;  // 时钟脉冲频率 P(eAxisNo).u16LoopTick
    static s32 M2 = 0;     // 检测周期内时钟脉冲计数

    static s32 n = 0;

    if (f0 > 0 && ++M2 == 40)  // 10ms
    {
        M1 -= pEnc->s64MultPos;

        n = ZOOM * 60.f * f0 * (s32)M1 / Z / M2 / P(eAxisNo).u16MotPolePairs;

        M1 = pEnc->s64MultPos;
        M2 = 0;
    }

    return n;

#undef ZOOM
}

static void AbsEncUpdate(abs_enc_t* pEnc, axis_e eAxisNo)
{
    switch (pEnc->eState)
    {
        case ABS_ENC_STATE_INIT:
        {
            pEnc->s32SpdFb = 0;

            P(eAxisNo).s64EncPosInit = pEnc->s64PosInit;
            P(eAxisNo).s32EncPos     = pEnc->s32Pos;
            P(eAxisNo).s32EncTurns   = pEnc->s32Turns;
            P(eAxisNo).s64EncMultPos = pEnc->s64MultPos;

            break;
        }

        case ABS_ENC_STATE_NORMAL:
        {
#if CONFIG_ENC_FREQ_DIV_OUT_SW
            AbsFreqDivOut(pEnc, eAxisNo);  // 分频输出
#endif

            pEnc->s32SpdFb = SpdCalc(pEnc, eAxisNo);

            if ((abs_enc_mode_e)P(eAxisNo).u16AbsEncWorkMode == ABS_ENC_MODE_INC)
            {
                // 以增量位置模式工作
                pEnc->s64MultPos -= pEnc->s64PosInit;
            }

            P(eAxisNo).s32EncPos     = pEnc->s32Pos;
            P(eAxisNo).s32EncTurns   = pEnc->s32Turns;
            P(eAxisNo).s64EncMultPos = pEnc->s64MultPos;

            break;
        }

        default:
        {
            break;
        }
    }
}

#if __ABS_ENC_TFORMAT_SW

/**
 * @brief 异步读取
 */
static void AbsTformatSamp(abs_enc_t* pEnc, axis_e eAxisNo)
{
    abs_drv_t* pEncDrv = pEnc->pEncDrv;

    switch (pEnc->eState)
    {
        case ABS_ENC_STATE_INIT:  // 初始位置获取
        {
            if (pEnc->u16Tick++ == 0)
            {
                TFormatWrCmd(pEncDrv, TFORMAT_CMD_READ_POS);
            }
            else if (pEnc->u16Tick < 10)
            {
                switch (pEncDrv->eCommSts)
                {
                    case ABS_COMM_DRDY:  // 数据就绪
                    {
                        pEnc->s32Pos     = pEncDrv->u32Pos;                                                 // 单圈值
                        pEnc->s32Turns   = 0;                                                               // 圈数
                        pEnc->s64MultPos = (s64)pEnc->s32Turns * (s64)pEnc->u32EncRes + (s64)pEnc->s32Pos;  // 多圈值

                        pEnc->s32PosPre  = pEnc->s32Pos;
                        pEnc->s64PosInit = pEnc->s64MultPos;  // 初始位置

                        AbsEncUpdate(pEnc, eAxisNo);

                        pEnc->eState  = ABS_ENC_STATE_NORMAL;  // 状态切换
                        pEnc->u16Tick = 0;

                        break;
                    }

                    case ABS_COMM_CRC_ERR:  // 检验错误，重试
                    {
                        goto __error;
                    }

                    default:
                    {
                        break;
                    }
                }
            }
            else  // 超时, 编码器不存在
            {
                TFormatTimeout(pEncDrv);

                if (pEncDrv->u16CommErr == 5)  // 重试
                {
                    pEnc->eState = ABS_ENC_STATE_COMM_FAIL;  // 状态切换
                }

                goto __error;
            }

            break;
        }

        case ABS_ENC_STATE_NORMAL:  // 编码器正常
        {
            if (pEnc->u16Tick++ == 0)
            {
                TFormatWrCmd(pEncDrv, TFORMAT_CMD_READ_POS);
            }
            else if (pEnc->u16Tick < 5)
            {
                switch (pEncDrv->eCommSts)
                {
                    case ABS_COMM_DRDY:  // 数据就绪
                    {
                        // 单圈值
                        pEnc->s32Pos     = (s32)pEncDrv->u32Pos;
                        pEnc->s32PosDelt = pEnc->s32Pos - pEnc->s32PosPre;
                        pEnc->s32PosPre  = pEnc->s32Pos;

                        // 圈数(跳变沿检测)
                        if (pEnc->s32PosDelt > (s32)(pEnc->u32EncRes >> 2))
                        {
                            pEnc->s32Turns--;
                        }
                        else if (pEnc->s32PosDelt < -(s32)(pEnc->u32EncRes >> 2))
                        {
                            pEnc->s32Turns++;
                        }

                        // 多圈值
                        pEnc->s64MultPos = (s64)pEnc->s32Turns * (s64)pEnc->u32EncRes + (s64)pEnc->s32Pos;

                        AbsEncUpdate(pEnc, eAxisNo);

                        // 清零, 下次读
                        pEnc->u16Tick = 0;

                        break;
                    }

                    case ABS_COMM_CRC_ERR:
                    {  // 检验错误
                        goto __error;
                    }

                    default:
                    {
                        break;
                    }
                }
            }
            else  // 超时, 编码器通讯丢失
            {
                TFormatTimeout(pEncDrv);
                pEnc->eState = ABS_ENC_STATE_COMM_LOST;  // 状态切换
                goto __error;
            }

            break;
        }

        default:
        case ABS_ENC_STATE_COMM_FAIL:
        case ABS_ENC_STATE_COMM_LOST:  //
        {
            break;
        }
    }

    return;

__error:
    P(eAxisNo).u16EncComErrSum++;  // 总错误次数
    pEnc->u16Tick = 0;
}

#endif

/**
 * @brief 同步读取
 */
static void AbsSyncSamp(abs_enc_t* pEnc, axis_e eAxisNo, void (*AbsRdPos)(abs_drv_t* pEncDrv))
{
    abs_drv_t* pEncDrv = pEnc->pEncDrv;

    switch (pEnc->eState)
    {
        case ABS_ENC_STATE_INIT:  // 初始位置获取
        {
            AbsRdPos(pEncDrv);

            switch (pEncDrv->eCommSts)
            {
                case ABS_COMM_DRDY:
                {
                    pEnc->s32Pos     = pEncDrv->u32Pos;                                                 // 单圈值
                    pEnc->s32Turns   = 0;                                                               // 圈数
                    pEnc->s64MultPos = (s64)pEnc->s32Turns * (s64)pEnc->u32EncRes + (s64)pEnc->s32Pos;  // 多圈值

                    pEnc->s32PosPre  = pEnc->s32Pos;
                    pEnc->s64PosInit = pEnc->s64MultPos;  // 初始位置

                    AbsEncUpdate(pEnc, eAxisNo);

                    pEnc->eState = ABS_ENC_STATE_NORMAL;  // 状态切换
                    break;
                }

                case ABS_COMM_TIMEOUT:  // 通信超时
                {
                    if (pEncDrv->u16CommErr == 5)
                    {
                        pEnc->eState = ABS_ENC_STATE_COMM_FAIL;  // 通信失败
                    }

                    goto __error;
                }

                case ABS_COMM_CRC_ERR:
                {
                    goto __error;
                }

                default:
                case ABS_COMM_WAIT:
                {
                    break;
                }
            }

            break;
        }
        case ABS_ENC_STATE_NORMAL:  //
        {
            AbsRdPos(pEncDrv);

            switch (pEncDrv->eCommSts)
            {
                case ABS_COMM_DRDY:
                {
                    pEnc->s32Pos     = (s32)pEncDrv->u32Pos;  // 单圈值
                    pEnc->s32PosDelt = pEnc->s32Pos - pEnc->s32PosPre;
                    pEnc->s32PosPre  = pEnc->s32Pos;

                    // 圈数(跳变沿检测)
                    if (pEnc->s32PosDelt > (s32)(pEnc->u32EncRes >> 2))
                    {
                        pEnc->s32Turns--;
                    }
                    else if (pEnc->s32PosDelt < -(s32)(pEnc->u32EncRes >> 2))
                    {
                        pEnc->s32Turns++;
                    }

                    // 多圈值
                    pEnc->s64MultPos = (s64)pEnc->s32Turns * (s64)pEnc->u32EncRes + (s64)pEnc->s32Pos;

                    AbsEncUpdate(pEnc, eAxisNo);

                    break;
                }

                case ABS_COMM_TIMEOUT:  // 通信超时
                {
                    if (pEncDrv->u16CommErr == 5)
                    {
                        pEnc->eState = ABS_ENC_STATE_COMM_LOST;  // 通信丢失
                    }

                    goto __error;
                }

                case ABS_COMM_CRC_ERR:
                {
                    goto __error;
                }

                default:
                case ABS_COMM_WAIT:
                {
                    break;
                }
            }

            break;
        }

        default:
        case ABS_ENC_STATE_COMM_FAIL:
        case ABS_ENC_STATE_COMM_LOST:
        {
            break;
        }
    }

    return;

__error:
    P(eAxisNo).u16EncComErrSum++;  // 总错误次数
}

void AbsEncCreat(abs_enc_t* pEnc, axis_e eAxisNo)
{}

void AbsEncInit(abs_enc_t* pEnc, axis_e eAxisNo)
{}

void AbsEncCycle(abs_enc_t* pEnc, axis_e eAxisNo)
{
    if (pEnc->u16CmdPre ^ P(eAxisNo).u16EncCmd)  // 上升沿触发
    {
        if (CHKBIT(P(eAxisNo).u16EncCmd, ABS_ENC_CMD_TURN_CLR) && !CHKBIT(pEnc->u16CmdPre, ABS_ENC_CMD_TURN_CLR))
        {
            P(eAxisNo).s32EncTurns = 0;
            pEnc->s32Turns         = 0;
        }

        if (CHKBIT(P(eAxisNo).u16EncCmd, ABS_ENC_CMD_ERR_CLR) && !CHKBIT(pEnc->u16CmdPre, ABS_ENC_CMD_ERR_CLR))
        {
            P(eAxisNo).u16EncErrCode   = 0;
            P(eAxisNo).u16EncComErrSum = 0;
            pEnc->pEncDrv->u16CommErr  = 0;
        }

        if (CHKBIT(P(eAxisNo).u16EncCmd, ABS_ENC_CMD_STATE_RST) && !CHKBIT(pEnc->u16CmdPre, ABS_ENC_CMD_STATE_RST))
        {
            pEnc->eState = ABS_ENC_STATE_INIT;
        }

        pEnc->u16CmdPre = P(eAxisNo).u16EncCmd;
    }
}

void AbsEncIsr(abs_enc_t* pEnc, axis_e eAxisNo)
{
    switch ((enc_type_e)P(eAxisNo).u16EncType)
    {
#if __ABS_ENC_AS5600_SW
        case ABS_ENC_AS5600:
        {
            AbsSyncSamp(pEnc, eAxisNo, As5600RdPos);
            break;
        }
#endif
#if __ABS_ENC_AS5047_SW
        case ABS_ENC_AS5047:
        {
            AbsSyncSamp(pEnc, eAxisNo, As5047RdPos);
            break;
        }
#endif
#if __ABS_ENC_MT6701_SW
        case ABS_ENC_MT6701:
        {
            AbsSyncSamp(pEnc, eAxisNo, Mt6701RdPos);
            break;
        }
#endif
#if __ABS_ENC_MT6816_SW
        case ABS_ENC_MT6816:
        {
            AbsSyncSamp(pEnc, eAxisNo, Mt6816RdPos);
            break;
        }
#endif
#if __ABS_ENC_MT6835_SW
        case ABS_ENC_MT6835:
        {
            AbsSyncSamp(pEnc, eAxisNo, Mt6835RdPos);
            break;
        }
#endif
#if __ABS_ENC_TFORMAT_SW
        case ABS_ENC_TFORMAT:
        {
            AbsTformatSamp(pEnc, eAxisNo);
            break;
        }
#endif
        default:
        {
            // AlmUpdate()
            break;
        }
    }
}
