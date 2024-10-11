#include "motencident.h"

#define CONFIG_ELEC_ANGLE_STEP 64    // 电角度步进值，需为65536的整数因子
#define CONFIG_LOCK_MAX_TIMES  1600  // 零电角度锁定计数，等待编码器位置反馈稳定 (1600*0.125us=0.1s)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG          "encident"
#define LOG_LOCAL_LEVEL        LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MotEncIdentCreat(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo)
{
    pMotEncIdent->pu16AxisFSM_i     = &P(eAxisNo).u16AxisFSM;
    pMotEncIdent->pu32CommCmd_o     = &P(eAxisNo).u32CommCmd;
    pMotEncIdent->pu16CtrlCmdSrc_io = &P(eAxisNo).u16CtrlCmdSrc;

    pMotEncIdent->pu32EncPos_i = &P(eAxisNo).u32EncPos;
#if CONFIG_CURSAMP_SW
    pMotEncIdent->ps16IaPu_i = &P(eAxisNo).s16IaPu;
    pMotEncIdent->ps16IbPu_i = &P(eAxisNo).s16IbPu;
    pMotEncIdent->ps16IcPu_i = &P(eAxisNo).s16IcPu;
#endif

    pMotEncIdent->ps16UdRef_o      = &P(eAxisNo).s16Ud;
    pMotEncIdent->ps16UqRef_o      = &P(eAxisNo).s16Uq;
    pMotEncIdent->pu16ElecAngRef_o = &P(eAxisNo).u16ElecAngleRef;

    pMotEncIdent->pu16IdentCmd_i          = &P(eAxisNo).u16MotEncIdentEn;
    pMotEncIdent->pu16IdentDirMatched_o   = &P(eAxisNo).u16MotEncIdentDirMatched;
    pMotEncIdent->pu32IdentEncRes_o       = &P(eAxisNo).u16MotEncIdentRes;
    pMotEncIdent->pu32IdentEncOffset_o    = &P(eAxisNo).u16MotEncIdentOffset;
    pMotEncIdent->pu16IdentMotPolePairs_o = &P(eAxisNo).u16MotEncIdentPolePairs;

    pMotEncIdent->pu16IdentFSM_o     = &P(eAxisNo).u16MotEncIdentState;
    pMotEncIdent->pu16IdentErrType_o = &P(eAxisNo).u16MotEncIdentErr;
}

void MotEncIdentInit(mot_enc_ident_t* pMotEncIdent) {}

void MotEncIdentEnter(mot_enc_ident_t* pMotEncIdent)
{
    pMotEncIdent->pu32CommCmd_o->u32Bit.Enable  = false;
    pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable = false;

    pMotEncIdent->u16CtrlCmdSrcBak = __get_u16(pMotEncIdent->pu16CtrlCmdSrc_io);
    __set_u16(pMotEncIdent->pu16CtrlCmdSrc_io, CTRL_CMD_SRC_COMM);

    __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_INIT);
}

void MotEncIdentExit(mot_enc_ident_t* pMotEncIdent)
{
    pMotEncIdent->pu32CommCmd_o->u32Bit.Enable  = false;
    pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable = false;

    __set_u16(pMotEncIdent->pu16CtrlCmdSrc_io, pMotEncIdent->u16CtrlCmdSrcBak);
}

void MotEncIdentCycle(mot_enc_ident_t* pMotEncIdent)
{
    pMotEncIdent->pu32CommCmd_o->u32Bit.Enable = pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable;

    switch (__get_u16(pMotEncIdent->pu16IdentFSM_o))
    {
        case MOT_ENC_IDENT_STATE_INIT:  // 初始化
        {
            if (pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable)  // 使能信号
            {
                if (__get_u16(pMotEncIdent->pu16AxisFSM_i) == AXIS_STATE_ENABLE)  // 轴使能后再开始辨识
                {
                    pMotEncIdent->u16CycleTimes     = 0;
                    pMotEncIdent->u16EncPosIncTimes = 0;
                    pMotEncIdent->u16EncPosDecTimes = 0;
                    pMotEncIdent->u32EncPosMax      = 0;
                    pMotEncIdent->u32EncPosPre      = 0;
                    pMotEncIdent->u16LockTimes      = 0;
                    pMotEncIdent->u32EncPosCcwInit  = 0;
                    pMotEncIdent->u16RotTimes       = 0;
                    pMotEncIdent->u16CwRotTimes     = 0;
                    memset(pMotEncIdent->u32ZeroAngPos, 0, sizeof(pMotEncIdent->u32ZeroAngPos));

                    __set_u16(pMotEncIdent->pu16IdentDirMatched_o, MOT_ENC_ROT_DIR_NONE);
                    __set_u32(pMotEncIdent->pu32IdentEncRes_o, 0);
                    __set_u32(pMotEncIdent->pu32IdentEncOffset_o, 0);
                    __set_u16(pMotEncIdent->pu16IdentMotPolePairs_o, 0);
                    __set_u16(pMotEncIdent->pu16IdentErrType_o, MOT_ENC_IDENT_ERR_NONE);

                    __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_ADAPT_VOLT);
                }
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_ANALYSE:  // 状态分析
        {
            pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable = false;  // 退使能

            u32 u32ElecAngOffset = 0;

            for (u8 i = 0; i < pMotEncIdent->u16RotTimes; i++)
            {
                u32ElecAngOffset += pMotEncIdent->u32ZeroAngPos[i] % (__get_u32(pMotEncIdent->pu32IdentEncRes_o) / __get_u16(pMotEncIdent->pu16IdentMotPolePairs_o));
            }

            __set_u32(pMotEncIdent->pu32IdentEncOffset_o, u32ElecAngOffset / pMotEncIdent->u16RotTimes);

            __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_FINISH);  // 辨识完成

#if 1
            P(AXIS_0).u16MotPolePairs = __get_u16(pMotEncIdent->pu16IdentMotPolePairs_o);
            P(AXIS_0).u32EncRes       = __get_u32(pMotEncIdent->pu32IdentEncRes_o);
            P(AXIS_0).u32EncPosOffset = __get_u32(pMotEncIdent->pu32IdentEncOffset_o);
#endif

            break;
        }
    }

    switch (__get_u16(pMotEncIdent->pu16IdentFSM_o))
    {
        case MOT_ENC_IDENT_STATE_ERR:     // 功能错误退使能
        case MOT_ENC_IDENT_STATE_FINISH:  // 功能结束退使能
        {
            if (pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable)  // 退使能后再次辨识使能
            {
                __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_INIT);
            }

            break;
        }

        default:  // 功能辨识阶段意外断使能时
        {
            if (!pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable)
            {
                __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_ERR);  // 辨识失败
                __set_u16(pMotEncIdent->pu16IdentErrType_o, MOT_ENC_IDNET_ERR_BREAK);
            }

            break;
        }
    }
}

void MotEncIdentIsr(mot_enc_ident_t* pMotEncIdent)
{
    u32 u32EncPos = __get_u32(pMotEncIdent->pu32EncPos_i);

#if CONFIG_CURSAMP_SW  // TODO: 相电流超出限幅

    if (((s32)__get_s16(pMotEncIdent->ps16IaPu_i) >= +pMotEncIdent->s16IphMax) &&
        ((s32)__get_s16(pMotEncIdent->ps16IaPu_i) <= -pMotEncIdent->s16IphMax) &&
        ((s32)__get_s16(pMotEncIdent->ps16IbPu_i) >= +pMotEncIdent->s16IphMax) &&
        ((s32)__get_s16(pMotEncIdent->ps16IbPu_i) <= -pMotEncIdent->s16IphMax) &&
        ((s32)__get_s16(pMotEncIdent->ps16IcPu_i) >= +pMotEncIdent->s16IphMax) &&
        ((s32)__get_s16(pMotEncIdent->ps16IcPu_i) <= -pMotEncIdent->s16IphMax))
    {
        __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_ERR);
        __set_u16(pMotEncIdent->pu16IdentErrType_o, MOT_ENC_IDENT_ERR_PHASE_OC);
        AlmSet(ERR_SW_OC, );
        return;
    }

#endif

    switch (__get_u16(pMotEncIdent->pu16IdentFSM_o))
    {
        case MOT_ENC_IDENT_STATE_ADAPT_VOLT:  // 电压自适应
        {
            __set_u16(pMotEncIdent->pu16ElecAngRef_o, 0);

            if (pMotEncIdent->pu16IdentCmd_i->u16Bit.AxisLock)  // lock q-axis
            {
                __inc_s16(pMotEncIdent->ps16UqRef_o, 4);
                __set_s16(pMotEncIdent->ps16UdRef_o, 0);

#if CONFIG_CURSAMP_SW
#endif

                if (__get_s16(pMotEncIdent->ps16UqRef_o) > 5000)  // TODO: Q轴电流反馈进入区间稳定后再切入下一步
                {
                    __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_DIR_CONFIRM);
                }
            }
            else  // lock d-axis
            {
                __set_s16(pMotEncIdent->ps16UqRef_o, 0);
                __inc_s16(pMotEncIdent->ps16UdRef_o, 4);

#if CONFIG_CURSAMP_SW
#endif

                if (__get_s16(pMotEncIdent->ps16UdRef_o) > 5000)  // TODO: D轴电流反馈进入区间稳定后再切入下一步
                {
                    __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_DIR_CONFIRM);
                }
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_DIR_CONFIRM:  // 旋转方向辨识
        {
            if (++pMotEncIdent->u16CycleTimes < 1000)
            {
                if (pMotEncIdent->u16CycleTimes % 20 == 0)  // 每20个周期采样1次, 减少因编码器数据抖动而带来的误差
                {
                    u32 u32EncPos = __get_u32(pMotEncIdent->pu32EncPos_i);

                    if (u32EncPos > pMotEncIdent->u32EncPosPre)
                    {
                        pMotEncIdent->u16EncPosIncTimes++;  // 编码器位置递增次数
                    }
                    else if (u32EncPos < pMotEncIdent->u32EncPosPre)
                    {
                        pMotEncIdent->u16EncPosDecTimes++;  // 编码器位置递减次数
                    }

                    pMotEncIdent->u32EncPosPre = u32EncPos;
                }

                __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);
            }
            else
            {
                if (pMotEncIdent->u16EncPosIncTimes > pMotEncIdent->u16EncPosDecTimes)
                {
                    // 编码器递增方向与电角度递增方向相同, 电机相序正常
                    __set_u16(pMotEncIdent->pu16IdentDirMatched_o, MOT_ENC_ROT_DIR_SAME);
                }
                else
                {
                    // 编码器递增方向与电角度递增方向相反, 电机相序异常
                    __set_u16(pMotEncIdent->pu16IdentDirMatched_o, MOT_ENC_ROT_DIR_DIFF);

                    // TODO: 电机相序异常警告（解决方法: 调换任意两根相线）
                    // AlmSet(WRN_MOT_PHASE_WS, );
                }

                pMotEncIdent->u32EncPosPre = __get_u32(pMotEncIdent->pu32EncPos_i);
                __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_RES_IND);
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_RES_IND:  // 最大分辨率辨识 (主要用于多摩川编码器)
        {
            u32 u32EncPos = __get_u32(pMotEncIdent->pu32EncPos_i);

            if (pMotEncIdent->u32EncPosMax < u32EncPos)  // 记录最大位置
            {
                pMotEncIdent->u32EncPosMax = u32EncPos;
            }

            if (((__get_u16(pMotEncIdent->pu16IdentDirMatched_o) == MOT_ENC_ROT_DIR_SAME) && (((s32)pMotEncIdent->u32EncPosPre - (s32)u32EncPos) > 1000)) ||  // 检测到下降沿
                ((__get_u16(pMotEncIdent->pu16IdentDirMatched_o) == MOT_ENC_ROT_DIR_DIFF) && (((s32)u32EncPos - (s32)pMotEncIdent->u32EncPosPre) > 1000)))    // 检测到上升沿
            {
                u8 u8EncResBit = 0;

                while (pMotEncIdent->u32EncPosMax)
                {
                    pMotEncIdent->u32EncPosMax >>= 1;
                    u8EncResBit++;
                }

                __set_u32(pMotEncIdent->pu32IdentEncRes_o, 1UL << u8EncResBit);
                __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_RETURN);
            }
            else
            {
                pMotEncIdent->u32EncPosPre = u32EncPos;
                __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_RETURN:  // 电角度回零位
        {
            if (__get_u16(pMotEncIdent->pu16ElecAngRef_o) == 0)
            {
                if (++pMotEncIdent->u16LockTimes == CONFIG_LOCK_MAX_TIMES)  // 锁定在零电角度, 等待编码器位置反馈稳定
                {
                    pMotEncIdent->u16LockTimes = 0;

                    pMotEncIdent->u32EncPosCcwInit = __get_u32(pMotEncIdent->pu32EncPos_i);  // 正转起始位置
                    __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);       // 正转离开零电角度
                    __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_CCW);
                }
            }
            else
            {
                __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_CCW:  // 极对数识别
        {
            // 原理：N对极的电机，机械角度转1圈，电角度转N圈

            if (__get_u16(pMotEncIdent->pu16ElecAngRef_o) == 0)
            {
                if (++pMotEncIdent->u16LockTimes == CONFIG_LOCK_MAX_TIMES)
                {
                    pMotEncIdent->u16LockTimes = 0;

                    if (pMotEncIdent->u16RotTimes == ARRAY_SIZE(pMotEncIdent->u32ZeroAngPos))
                    {
                        // 极对数过多, 缓冲数组过小
                        __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_ERR);
                        __set_u16(pMotEncIdent->pu16IdentErrType_o, MOT_ENC_IDENT_ERR_POLE_PAIRS_TOO_MANY);
                        pMotEncIdent->pu16IdentCmd_i->u16Bit.Enable = false;
                        break;
                    }

                    pMotEncIdent->u32ZeroAngPos[pMotEncIdent->u16RotTimes++] = __get_u32(pMotEncIdent->pu32EncPos_i);  // 零电角度对应的编码器位置

                    u32 u32EncPosAbsErr = AbsDelta(pMotEncIdent->u32EncPosCcwInit, __get_u32(pMotEncIdent->pu32EncPos_i));

                    // 回到正转起点?
                    if ((__get_u32(pMotEncIdent->pu32IdentEncRes_o) / 10 > u32EncPosAbsErr) ||                                               // [0, u32EncRes*0.1)
                        (__get_u32(pMotEncIdent->pu32IdentEncRes_o) / 10 > (__get_u32(pMotEncIdent->pu32IdentEncRes_o) - u32EncPosAbsErr)))  // (u32EncRes*0.9, u32EncRes]
                    {
                        __set_u16(pMotEncIdent->pu16IdentMotPolePairs_o, pMotEncIdent->u16RotTimes);  // 旋转次数即极对数

                        if (__get_u16(pMotEncIdent->pu16IdentMotPolePairs_o) > (ARRAY_SIZE(pMotEncIdent->u32ZeroAngPos) / 2))
                        {
                            // 极对数过多, 直接进入数据分析阶段
                            __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_ANALYSE);
                        }
                        else
                        {
                            pMotEncIdent->u16CwRotTimes = pMotEncIdent->u16RotTimes;            // 设定反转次数
                            __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);  // 反转离开零电角度
                            __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_CW);
                        }
                    }
                    else
                    {
                        __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);  // 正转离开零电角度
                    }
                }
            }
            else
            {
                __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_CW: {
            if (__get_u16(pMotEncIdent->pu16ElecAngRef_o) == 0)
            {
                if (++pMotEncIdent->u16LockTimes == CONFIG_LOCK_MAX_TIMES)
                {
                    pMotEncIdent->u16LockTimes = 0;

                    pMotEncIdent->u32ZeroAngPos[pMotEncIdent->u16RotTimes++] = __get_u32(pMotEncIdent->pu32EncPos_i);

                    if (--pMotEncIdent->u16CwRotTimes == 0)  // 回到反转起点
                    {
                        __set_u16(pMotEncIdent->pu16IdentFSM_o, MOT_ENC_IDENT_STATE_ANALYSE);
                    }
                    else
                    {
                        __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);  // 反转离开零电角度
                    }
                }
            }
            else
            {
                __inc_u16(pMotEncIdent->pu16ElecAngRef_o, CONFIG_ELEC_ANGLE_STEP);
            }
        }

        default: {
            break;
        }
    }
}
