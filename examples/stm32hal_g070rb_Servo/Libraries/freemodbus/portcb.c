
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

#include "gsdk.h"
#include "paratbl.h"
#include "paraattr.h"

#define CONFIG_A4_TRACE_SW  0
#define CONFIG_MSB_FIRST_SW 1  // tuner = 0

// register_group_t
typedef struct {
    u16  u16Offset;
    u16  u16Count;
    u16* u16Buffer;
} reg_grp_t;

device_para_t sDevicePara;
axis_para_t   aAxisPara[1];

#if CONFIG_A4_TRACE_SW
__IO s32 as32TraceBuffer[CONFIG_SAMP_CH_NUM * 1000] = {0};
#endif

static const reg_grp_t m_holding[] = {

	  {0,   sizeof(device_para_t) / sizeof(u16), (u16*)&sDevicePara            }, // 设备参数
    {300,   sizeof(axis_para_t) / sizeof(u16), (u16*)&aAxisPara[0]          }, // 轴参数
		// {500,   sizeof(aAxisPara) / sizeof(u16), (u16*)&aAxisPara[1]          }, // 轴参数
		
#if CONFIG_A4_TRACE_SW
    {800, 8000,                               (u16*)&as32TraceBuffer[0]}, // 曲线采样
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

    extern const para_attr_t sParaAttr[];

    for (u8GrpIdx = 0; u8GrpIdx < ARRAY_SIZE(m_holding); ++u8GrpIdx)
    {
        if ((m_holding[u8GrpIdx].u16Offset <= u16Address) && ((u16Address + u16Count) <= (m_holding[u8GrpIdx].u16Offset + m_holding[u8GrpIdx].u16Count + 1)))
        {
            u16  u16RegIdx  = u16Address - m_holding[u8GrpIdx].u16Offset;
            u16* pu16RegBuf = &(m_holding[u8GrpIdx].u16Buffer[u16RegIdx]);
					
	
			
					 para_attr_t * sParaAttr;
					
					if(u8GrpIdx == 0)
					{
						sParaAttr= aDeviceAttr;
					}
					else if(u8GrpIdx == 1)
					{
						sParaAttr= aAxisAttr;
					}

            switch (eMode)
            {
                case MB_REG_READ:
                {
                    while (u16Count > 0)
                    {
                        uint8_t u8Step, _u8Step;

                        switch (sParaAttr[u16RegIdx].uSubAttr.u32Bit.Length)
                        {
                            default:  // 错误访问
                            {
#if 0
                                // 上位机参数地址按字长递增时用
                                eStatus = MB_EIO;
                                goto __exit;
#else
                                u8Step = 1;
                                break;
#endif
                            }

                            case V_SIG:
                            {
                                u8Step = 1;
                                break;
                            }

                            case V_DOB0:
                            {
                                u8Step = 2;
                                break;
                            }

                            case V_QUD0:
                            {
                                u8Step = 4;
                                break;
                            }
                        }

                        if (u16Count < u8Step)  // 访问超限
                        {
                            eStatus = MB_EIO;
                            goto __exit;
                        }

                        _u8Step = u8Step;

                        while (_u8Step--)
                        {
                            *pu8Buffer++ = (u8)(pu16RegBuf[_u8Step] >> 8);
                            *pu8Buffer++ = (u8)(pu16RegBuf[_u8Step] & 0xFF);
                        }

                        pu16RegBuf += u8Step;
                        u16Count -= u8Step;
                        u16RegIdx += u8Step;
                    }

                    break;
                }
                case MB_REG_WRITE:
                {
                    while (u16Count > 0)
                    {
                        uint8_t u8Step, _u8Step;

                        switch (sParaAttr[u16RegIdx].uSubAttr.u32Bit.Length)
                        {
                            default:  // 错误访问
                            {
#if 0
                                // 上位机参数地址按字长递增时用
                                eStatus = MB_EIO;
                                goto __exit;
#else
                                u8Step = 1;
                                break;
#endif
                            }

                            case V_SIG:
                            {
                                u8Step = 1;
                                break;
                            }

                            case V_DOB0:
                            {
                                u8Step = 2;
                                break;
                            }

                            case V_QUD0:
                            {
                                u8Step = 4;
                                break;
                            }
                        }

                        if (u16Count < u8Step)  // 访问超限
                        {
                            eStatus = MB_EIO;
                            goto __exit;
                        }

                        switch (sParaAttr[u16RegIdx].uSubAttr.u32Bit.Mode)
                        {
                            default:
                            case V_RO:  // 只读
                            {
                                eStatus = MB_EIO;
                                goto __exit;
                            }
                            case V_RW_M0:
                            case V_RW_M1:
                            case V_RW_M2:
                            {
                                _u8Step = u8Step;

                                while (_u8Step--)
                                {
                                    pu16RegBuf[_u8Step] = *pu8Buffer++ << 8;
                                    pu16RegBuf[_u8Step] |= *pu8Buffer++;
                                }

                                break;
                            }
                        }

                        pu16RegBuf += u8Step;
                        u16Count -= u8Step;
                        u16RegIdx += u8Step;
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

__exit:
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
