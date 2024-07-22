#include "motencident.h"

#define CONFIG_ELEC_ANGL_STEP 64    // 电角度步进值，需为65536的整数因子
#define CONFIG_LOCK_MAX_TIMES 1600  // 零电角度锁定计数，等待编码器位置反馈稳定 (1600*0.125us=0.1s)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "encident"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief 辨识状态
 */
typedef enum {
    MOT_ENC_IDENT_STATE_INIT,         ///< 初始化阶段
    MOT_ENC_IDENT_STATE_ADAPT_VOLT,   ///< 电压自适应阶段
    MOT_ENC_IDENT_STATE_DIR_CONFIRM,  ///< 修正电机正方向
    MOT_ENC_IDENT_STATE_RES_IND,      ///< 编码器分辨率辨识阶段
    MOT_ENC_IDENT_STATE_RETURN,       ///< 电机电角度回初阶段
    MOT_ENC_IDENT_STATE_CCW,          ///< 电机往正方向旋转（初始以电角度自增为正方向）
    MOT_ENC_IDENT_STATE_CW,           ///< 电机往反方向旋转（初始以电角度自减为正方向）
    MOT_ENC_IDENT_STATE_ANALYSE,      ///< 计算编码器安装偏置
    MOT_ENC_IDENT_STATE_ERR,          ///< 辨识过程中出错，错误处理阶段
    MOT_ENC_IDENT_STATE_FINISH,       ///< 辨识结束
} mot_enc_ident_state_e;

/**
 * @brief 方向匹配
 */
typedef enum {
    MOT_ENC_ROT_DIR_NONE,
    MOT_ENC_ROT_DIR_SAME,  ///< 编码器位置递增方向和电角度递增方向相同
    MOT_ENC_ROT_DIR_DIFF,  ///< 编码器位置递增方向和电角度递增方向相反
} mot_enc_ident_dir_e;

/**
 * @brief 报警类型
 */
typedef enum {
    MOT_ENC_IDENT_ERR_NONE,                 ///< 无报警
    MOT_ENC_IDENT_ERR_PHASE_OC,             ///< 单相过流
    MOT_ENC_IDENT_ERR_IDIQ_OC,              ///< ID/IQ 过流
    MOT_ENC_IDNET_ERR_BREAK,                ///< 未完成辨识流程意外断使能
    MOT_ENC_IDENT_ERR_POLE_PAIRS_TOO_MANY,  ///< 电机极对数过多
} mot_enc_ident_err_e;

/**
 * @brief 控制字
 */
typedef union {
    struct {
        uint16_t Enable   : 1;
        uint16_t AxisLock : 1;  ///< 0: d-axis, 1: q-axis
        uint16_t _Resv    : 15;
    } u16Bit;
    uint16_t u16All;
} app_ctrlword_u;

//

#define u16AxisFSM_i(eAxisNo)           P(eAxisNo).u16AxisFSM

#define u32CommCmd_o(eAxisNo)           P(eAxisNo).u32CommCmd
#define u16CtrlCmdSrc_io(eAxisNo)       P(eAxisNo).u16CtrlCmdSrc

#define s16IaPu_i(eAxisNo)              1000
#define s16IbPu_i(eAxisNo)              1000
#define s16IcPu_i(eAxisNo)              1000
#define u32EncPos_i(eAxisNo)            P(eAxisNo).u32EncPos
#define s16UdRef_o(eAxisNo)             P(eAxisNo).s16Ud
#define s16UqRef_o(eAxisNo)             P(eAxisNo).s16Uq
#define u16ElecAngRef_o(eAxisNo)        P(eAxisNo).u16ElecAngleRef

#define u16IdentCmd_i(eAxisNo)          P(eAxisNo).u16MotEncIdentEn
#define u16IdentDirMatched_o(eAxisNo)   P(eAxisNo).u16MotEncIdentDirMatched
#define u32IdentEncRes_o(eAxisNo)       P(eAxisNo).u16MotEncIdentRes
#define u32IdentEncOffset_o(eAxisNo)    P(eAxisNo).u16MotEncIdentOffset
#define u16IdentMotPolePairs_o(eAxisNo) P(eAxisNo).u16MotEncIdentPolePairs
#define u32IdentFSM_o(eAxisNo)          P(eAxisNo).u16MotEncIdentState
#define u16IdentErrType_o(eAxisNo)      P(eAxisNo).u16MotEncIdentErr

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static u16 s_u16CtrlCmdSrcBak;  // 控制模式

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MotEncIdentCreat(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo) {}

void MotEncIdentInit(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo) {}

void MotEncIdentEnter(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo)
{
    ctrlword_u*     pCommCmd  = (ctrlword_u*)&u32CommCmd_o(eAxisNo);
    app_ctrlword_u* pIdentCmd = (app_ctrlword_u*)&u16IdentCmd_i(eAxisNo);

    pCommCmd->u32Bit.Enable  = false;
    pIdentCmd->u16Bit.Enable = false;

    s_u16CtrlCmdSrcBak        = u16CtrlCmdSrc_io(eAxisNo);
    u16CtrlCmdSrc_io(eAxisNo) = CTRL_CMD_SRC_COMM;

    u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_INIT;
}

void MotEncIdentExit(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo)
{
    ctrlword_u*     pCommCmd  = (ctrlword_u*)&u32CommCmd_o(eAxisNo);
    app_ctrlword_u* pIdentCmd = (app_ctrlword_u*)&u16IdentCmd_i(eAxisNo);

    pCommCmd->u32Bit.Enable  = false;
    pIdentCmd->u16Bit.Enable = false;

    u16CtrlCmdSrc_io(eAxisNo) = s_u16CtrlCmdSrcBak;
}

void MotEncIdentCycle(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo)
{
    ctrlword_u*     pCommCmd  = (ctrlword_u*)&u32CommCmd_o(eAxisNo);
    app_ctrlword_u* pIdentCmd = (app_ctrlword_u*)&u16IdentCmd_i(eAxisNo);

    pCommCmd->u32Bit.Enable = pIdentCmd->u16Bit.Enable;

    switch (u32IdentFSM_o(eAxisNo))
    {
        case MOT_ENC_IDENT_STATE_INIT:
        {
            // 使能信号

            if (pIdentCmd->u16Bit.Enable)
            {
                if (u16AxisFSM_i(eAxisNo) == AXIS_STATE_ENABLE)  // 轴使能后再开始辨识
                {
#if 1
                    memset(pMotEncIdent, 0, sizeof(mot_enc_ident_t));
#else
                    pMotEncIdent->u16CycleTimes     = 0;
                    pMotEncIdent->u16EncPosIncTimes = 0;
                    pMotEncIdent->u16EncPosDecTimes = 0;
                    pMotEncIdent->u32EncPosMax      = 0;
                    pMotEncIdent->u32EncPosPre      = 0;
                    pMotEncIdent->u16LockTimes      = 0;
                    pMotEncIdent->u32EncPosCcwInit  = 0;
                    pMotEncIdent->u16RotTimes       = 0;
                    pMotEncIdent->u16CwRotTimes     = 0;
                    memset(pMotEncIdent->u32ZeroAngPos, 0, pMotEncIdent->u32ZeroAngPos);
#endif

                    u16IdentDirMatched_o(eAxisNo)   = 0;
                    u32IdentEncRes_o(eAxisNo)       = 0;
                    u32IdentEncOffset_o(eAxisNo)    = 0;
                    u16IdentMotPolePairs_o(eAxisNo) = 0;
                    u16IdentErrType_o(eAxisNo)      = MOT_ENC_IDENT_ERR_NONE;

                    u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_ADAPT_VOLT;
                }
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_ANALYSE:  // 编码器安装偏置
        {
            pIdentCmd->u16Bit.Enable = false;  // 退使能

            uint32_t u32ElecAngOffset = 0;

            for (uint8_t i = 0; i < pMotEncIdent->u16RotTimes; i++)
            {
                u32ElecAngOffset += pMotEncIdent->u32ZeroAngPos[i] % (u32IdentEncRes_o(eAxisNo) / u16IdentMotPolePairs_o(eAxisNo));
            }

            u32IdentEncOffset_o(eAxisNo) = u32ElecAngOffset / pMotEncIdent->u16RotTimes;

            u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_FINISH;  // 辨识完成

            break;
        }
    }

    switch (u32IdentFSM_o(eAxisNo))
    {
        case MOT_ENC_IDENT_STATE_ERR:     // 功能错误退使能
        case MOT_ENC_IDENT_STATE_FINISH:  // 功能结束退使能
        {
            if (pIdentCmd->u16Bit.Enable)  // 退使能后再次辨识使能
            {
                u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_INIT;
            }

            break;
        }

        default:  // 功能辨识阶段意外断使能时
        {
            if (!pIdentCmd->u16Bit.Enable)
            {
                u32IdentFSM_o(eAxisNo)     = MOT_ENC_IDENT_STATE_ERR;
                u16IdentErrType_o(eAxisNo) = MOT_ENC_IDNET_ERR_BREAK;
            }

            break;
        }
    }
}

void MotEncIdentIsr(mot_enc_ident_t* pMotEncIdent, axis_e eAxisNo)
{
    app_ctrlword_u* pIdentCmd = (app_ctrlword_u*)&u16IdentCmd_i(eAxisNo);

#if 0  // TODO: 相电流超出限幅

    if (((s32)s16IaPu_i(eAxisNo) >= +pMotEncIdent->s16IphMax) &&
        ((s32)s16IaPu_i(eAxisNo) <= -pMotEncIdent->s16IphMax) &&
        ((s32)s16IbPu_i(eAxisNo) >= +pMotEncIdent->s16IphMax) &&
        ((s32)s16IbPu_i(eAxisNo) <= -pMotEncIdent->s16IphMax) &&
        ((s32)s16IcPu_i(eAxisNo) >= +pMotEncIdent->s16IphMax) &&
        ((s32)s16IcPu_i(eAxisNo) <= -pMotEncIdent->s16IphMax))
    {
        u32IdentFSM_o(eAxisNo)     = MOT_ENC_IDENT_STATE_ERR;
        u16IdentErrType_o(eAxisNo) = MOT_ENC_IDENT_ERR_PHASE_OC; 
        AlmSet(ERR_SW_OC, eAxisNo);
        return;
    }

#endif

    switch (u32IdentFSM_o(eAxisNo))
    {
        case MOT_ENC_IDENT_STATE_ADAPT_VOLT:  // 电压自适应
        {
            u16ElecAngRef_o(eAxisNo) = 0;

            if (pIdentCmd->u16Bit.AxisLock)  // lock q-axis
            {
                s16UqRef_o(eAxisNo) += 4;
                s16UdRef_o(eAxisNo) = 0;

                if (s16UqRef_o(eAxisNo) > 5000)  // TODO: Q轴电流反馈进入区间稳定后再切入下一步
                {
                    u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_DIR_CONFIRM;
                }
            }
            else  // lock d-axis
            {
                s16UqRef_o(eAxisNo) = 0;
                s16UdRef_o(eAxisNo) += 4;

                if (s16UdRef_o(eAxisNo) > 5000)  // TODO: D轴电流反馈进入区间稳定后再切入下一步
                {
                    u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_DIR_CONFIRM;
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
                    uint32_t u32EncPos = u32EncPos_i(eAxisNo);

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

                u16ElecAngRef_o(eAxisNo) += CONFIG_ELEC_ANGL_STEP;
            }
            else
            {
                if (pMotEncIdent->u16EncPosIncTimes > pMotEncIdent->u16EncPosDecTimes)
                {
                    // 编码器递增方向与电角度递增方向相同, 电机相序正常
                    u16IdentDirMatched_o(eAxisNo) = MOT_ENC_ROT_DIR_SAME;
                }
                else
                {
                    // 编码器递增方向与电角度递增方向相反, 电机相序异常
                    u16IdentDirMatched_o(eAxisNo) = MOT_ENC_ROT_DIR_DIFF;

                    // TODO: 电机相序异常警告（解决方法: 调换任意两根相线）
                    // AlmSet(WRN_MOT_PHASE_WS, eAxisNo);
                }

                pMotEncIdent->u32EncPosPre = u32EncPos_i(eAxisNo);
                u32IdentFSM_o(eAxisNo)     = MOT_ENC_IDENT_STATE_RES_IND;
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_RES_IND:  // 最大分辨率辨识 (主要用于多摩川编码器)
        {
            uint32_t u32EncPos = u32EncPos_i(eAxisNo);

            if (pMotEncIdent->u32EncPosMax < u32EncPos)  // 记录最大位置
            {
                pMotEncIdent->u32EncPosMax = u32EncPos;
            }

            if (((u16IdentDirMatched_o(eAxisNo) == MOT_ENC_ROT_DIR_SAME) && (((s32)pMotEncIdent->u32EncPosPre - (s32)u32EncPos) > 1000)) ||  // 检测到下降沿
                ((u16IdentDirMatched_o(eAxisNo) == MOT_ENC_ROT_DIR_DIFF) && (((s32)u32EncPos - (s32)pMotEncIdent->u32EncPosPre) > 1000)))    // 检测到上升沿
            {
                uint8_t u8EncResBit = 0;

                while (pMotEncIdent->u32EncPosMax)
                {
                    pMotEncIdent->u32EncPosMax >>= 1;
                    u8EncResBit++;
                }

                u32IdentEncRes_o(eAxisNo) = 1UL << u8EncResBit;
                u32IdentFSM_o(eAxisNo)    = MOT_ENC_IDENT_STATE_RETURN;
            }
            else
            {
                pMotEncIdent->u32EncPosPre = u32EncPos;
                u16ElecAngRef_o(eAxisNo) += CONFIG_ELEC_ANGL_STEP;
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_RETURN:  // 电角度回零位
        {
            if (u16ElecAngRef_o(eAxisNo) == 0)
            {
                if (++pMotEncIdent->u16LockTimes == CONFIG_LOCK_MAX_TIMES)  // 锁定在零电角度, 等待编码器位置反馈稳定
                {
                    pMotEncIdent->u16LockTimes = 0;

                    pMotEncIdent->u32EncPosCcwInit = u32EncPos_i(eAxisNo);  // 正转起始位置
                    u16ElecAngRef_o(eAxisNo) += CONFIG_ELEC_ANGL_STEP;      // 正转离开零电角度
                    u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_CCW;
                }
            }
            else
            {
                u16ElecAngRef_o(eAxisNo) += CONFIG_ELEC_ANGL_STEP;
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_CCW:  // 极对数识别
        {
            // 原理：N对极的电机，机械角度转1圈，电角度转N圈

            if (u16ElecAngRef_o(eAxisNo) == 0)
            {
                if (++pMotEncIdent->u16LockTimes == CONFIG_LOCK_MAX_TIMES)
                {
                    pMotEncIdent->u16LockTimes = 0;

                    if (pMotEncIdent->u16RotTimes == ARRAY_SIZE(pMotEncIdent->u32ZeroAngPos))
                    {
                        // TODO 报警：极对数过多, 位置缓存不足
                        u32IdentFSM_o(eAxisNo)     = MOT_ENC_IDENT_STATE_ERR;
                        u16IdentErrType_o(eAxisNo) = MOT_ENC_IDENT_ERR_POLE_PAIRS_TOO_MANY;
                        pIdentCmd->u16Bit.Enable   = false;
                        break;
                    }

                    pMotEncIdent->u32ZeroAngPos[pMotEncIdent->u16RotTimes++] = u32EncPos_i(eAxisNo);  // 零电角度对应的编码器位置

                    uint32_t u32EncPosAbsErr = AbsDelta(pMotEncIdent->u32EncPosCcwInit, u32EncPos_i(eAxisNo));

                    // 回到正转起点?
                    if ((u32IdentEncRes_o(eAxisNo) / 10 > u32EncPosAbsErr) ||                              // [0, u32EncRes*0.1)
                        (u32IdentEncRes_o(eAxisNo) / 10 > (u32IdentEncRes_o(eAxisNo) - u32EncPosAbsErr)))  // (u32EncRes*0.9, u32EncRes]
                    {
                        u16IdentMotPolePairs_o(eAxisNo) = pMotEncIdent->u16RotTimes;  // 旋转次数即极对数

                        if (u16IdentMotPolePairs_o(eAxisNo) > (ARRAY_SIZE(pMotEncIdent->u32ZeroAngPos) / 2))
                        {
                            // 极对数过多, 直接进入数据分析阶段
                            u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_ANALYSE;
                        }
                        else
                        {
                            pMotEncIdent->u16CwRotTimes = pMotEncIdent->u16RotTimes;  // 设定反转次数
                            u16ElecAngRef_o(eAxisNo) -= CONFIG_ELEC_ANGL_STEP;        // 反转离开零电角度
                            u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_CW;
                        }
                    }
                    else
                    {
                        u16ElecAngRef_o(eAxisNo) += CONFIG_ELEC_ANGL_STEP;  // 正转离开零电角度
                    }
                }
            }
            else
            {
                u16ElecAngRef_o(eAxisNo) += CONFIG_ELEC_ANGL_STEP;
            }

            break;
        }

        case MOT_ENC_IDENT_STATE_CW:
        {
            if (u16ElecAngRef_o(eAxisNo) == 0)
            {
                if (++pMotEncIdent->u16LockTimes == CONFIG_LOCK_MAX_TIMES)
                {
                    pMotEncIdent->u16LockTimes = 0;

                    pMotEncIdent->u32ZeroAngPos[pMotEncIdent->u16RotTimes++] = u32EncPos_i(eAxisNo);

                    if (--pMotEncIdent->u16CwRotTimes == 0)  // 回到反转起点
                    {
                        u32IdentFSM_o(eAxisNo) = MOT_ENC_IDENT_STATE_ANALYSE;
                    }
                    else
                    {
                        u16ElecAngRef_o(eAxisNo) -= CONFIG_ELEC_ANGL_STEP;  // 反转离开零电角度
                    }
                }
            }
            else
            {
                u16ElecAngRef_o(eAxisNo) -= CONFIG_ELEC_ANGL_STEP;
            }
        }

        default:
        {
            break;
        }
    }
}
