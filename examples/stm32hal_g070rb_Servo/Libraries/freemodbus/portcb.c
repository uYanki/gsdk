
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

#include "gsdk.h"
#include "paratbl.h"

#define CONFIG_A4_TRACE_SW  0
#define CONFIG_MSB_FIRST_SW 1
#define CONFIG_TUNER_SW     1  // 合信伺服上位机

#if CONFIG_TUNER_SW
#undef CONFIG_MSB_FIRST_SW
#define CONFIG_MSB_FIRST_SW 1
#endif

// register_group_t
typedef struct {
    u16  u16Offset;
    u16  u16Count;
    u16* u16Buffer;
} reg_grp_t;

#if CONFIG_TUNER_SW

typedef struct {
    u16 SoftVer;
    u16 DrvType;
} cotrust_tuner_t;

cotrust_tuner_t sTuner = {
    .SoftVer = 22096,  // A4
    .DrvType = 0x0303,
};

#endif

para_table_t tbl;

#if CONFIG_A4_TRACE_SW
__IO s32 as32TraceBuffer[CONFIG_SAMP_CH_NUM * 1000] = {0};
#endif

static const reg_grp_t m_holding[] = {
#if CONFIG_TUNER_SW
    {180,   sizeof(cotrust_tuner_t) / sizeof(u16), (u16*)&sTuner            }, // 合信上位机
#endif

    {0,     sizeof(para_table_t) / sizeof(u16),     (u16*)&tbl       }, // 轴参数

#if CONFIG_A4_TRACE_SW
    {800,   8000,                                  (u16*)&as32TraceBuffer[0]}, // 曲线采样
#endif

  //  {10000, sizeof(drv_para_t) / sizeof(u16),      (u16*)&sDrvTbl           }, // 驱动器信息

#if 0
    {9000,  sizeof(dbg_tbl_t) / sizeof(u16),       (u16*)&sDbgTbl           }, // 调试用
#endif
};

//-----------------------------------------------------------------------------
//

eMBErrorCode eMBRegInputCB(u8* pu8Buffer, u16 u16Address, u16 u16Count)
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegHoldingCB(u8* pu8Buffer, u16 u16Address, u16 u16Count, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOREG;

    u8 u8GrpIdx;

    u16Address--;

    for (u8GrpIdx = 0; u8GrpIdx < ARRAY_SIZE(m_holding); ++u8GrpIdx)
    {
        if ((m_holding[u8GrpIdx].u16Offset <= u16Address) && ((u16Address + u16Count) <= (m_holding[u8GrpIdx].u16Offset + m_holding[u8GrpIdx].u16Count + 1)))
        {
            u16  u16RegIdx  = u16Address - m_holding[u8GrpIdx].u16Offset;
            u16* pu16RegBuf = &(m_holding[u8GrpIdx].u16Buffer[u16RegIdx]);

            switch (eMode)
            {
                case MB_REG_READ: {
#if CONFIG_A4_TRACE_SW
                    if (u8GrpIdx == 2)
                    {
                        while (u16Count > 1)
                        {
                            *pu8Buffer++ = ((u8*)pu16RegBuf)[3];
                            *pu8Buffer++ = ((u8*)pu16RegBuf)[2];
                            *pu8Buffer++ = ((u8*)pu16RegBuf)[1];
                            *pu8Buffer++ = ((u8*)pu16RegBuf)[0];
                            pu16RegBuf += 2;
                            u16Count -= 2;
                        }

                        break;
                    }
#endif
                    while (u16Count > 0)
                    {
#if CONFIG_MSB_FIRST_SW
                        *pu8Buffer++ = (u8)(*pu16RegBuf >> 8);
                        *pu8Buffer++ = (u8)(*pu16RegBuf & 0xFF);
#else
                        *pu8Buffer++ = (u8)(*pu16RegBuf & 0xFF);
                        *pu8Buffer++ = (u8)(*pu16RegBuf >> 8);
#endif
                        pu16RegBuf++;
                        u16Count--;
                    }
                    break;
                }
                case MB_REG_WRITE: {
#if CONFIG_TUNER_SW
                    if (u8GrpIdx == 0 && u16RegIdx == 0)
                    {
                        break;
                    }
#endif

                    while (u16Count > 0)
                    {
#if CONFIG_MSB_FIRST_SW
                        *pu16RegBuf = *pu8Buffer++ << 8;
                        *pu16RegBuf |= *pu8Buffer++;
#else
                        *pu16RegBuf |= *pu8Buffer++;
                        *pu16RegBuf = *pu8Buffer++ << 8;
#endif
                        pu16RegBuf++;
                        u16Count--;
                    }
                }
            }

            if (u16Count == 0)
            {
                eStatus = MB_ENOERR;
                break;
            }
        }
    }

    return eStatus;
}

eMBErrorCode eMBRegCoilsCB(u8* pu8Buffer, u16 u16Address, u16 u16NCoils, eMBRegisterMode eMode)
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB(u8* pu8Buffer, u16 u16Address, u16 u16NDiscrete)
{
    return MB_ENOREG;
}
