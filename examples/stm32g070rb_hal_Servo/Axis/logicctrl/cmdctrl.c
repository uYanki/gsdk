#include "cmdctrl.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "cmdctrl"
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

void CmdCtrlCreat(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo)
{
    pCmdCtrl->pu16AxisFSM_io = &P(eAxisNo).u16AxisFSM;

    pCmdCtrl->pu16CtrlCmdSrc_i = &P(eAxisNo).u16CtrlCmdSrc;
    pCmdCtrl->pu32CommCmd_i    = &P(eAxisNo).u32CommCmd;
}

void CmdCtrlInit(cmd_ctrl_t* pCmdCtrl)
{
}

void CmdCtrlCycle(cmd_ctrl_t* pCmdCtrl)
{
    switch (__get_u16(pCmdCtrl->pu16CtrlCmdSrc_i))
    {
        case CTRL_CMD_SRC_DI: {
            break;
        }
        case CTRL_CMD_SRC_COMM: {
            u32 u32CommCmdCur = __get_u32(pCmdCtrl->pu32CommCmd_i);

            if (pCmdCtrl->u32CommCmdPre ^ u32CommCmdCur)
            {
                ctrlword_u* pCtrlWord = (ctrlword_u*)&u32CommCmdCur;

                if (pCtrlWord->u32Bit.Enable)
                {
                    __set_u16(pCmdCtrl->pu16AxisFSM_io, AXIS_STATE_ENABLE);
                    PWM_Start(AXIS_0);
                }
                else
                {
                    __set_u16(pCmdCtrl->pu16AxisFSM_io, AXIS_STATE_STANDBY);
                    PWM_Stop(AXIS_0);
                }

                pCmdCtrl->u32CommCmdPre = u32CommCmdCur;
            }

            break;
        }
        default: {
            break;
        }
    }
}

void CmdCtrlIsr(cmd_ctrl_t* pCmdCtrl)
{
}
