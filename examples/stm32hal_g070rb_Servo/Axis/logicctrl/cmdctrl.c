#include "cmdctrl.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG            "cmdctrl"
#define LOG_LOCAL_LEVEL          LOG_LEVEL_INFO

#define u32CommCmd_i(eAxisNo)    P(eAxisNo).u32CommCmd
#define u16AxisFSM_i(eAxisNo)    P(eAxisNo).u16AxisFSM
#define u16CtrlCmdSrc_i(eAxisNo) P(eAxisNo).u16CtrlCmdSrc

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
}

void CmdCtrlInit(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo)
{
}

void CmdCtrlCycle(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo)
{
    switch (u16CtrlCmdSrc_i(eAxisNo))
    {
        case CTRL_CMD_SRC_DI:
        {
            break;
        }
        case CTRL_CMD_SRC_COMM:
        {
            u32 u32CommCmdCur = u32CommCmd_i(eAxisNo);

            if (pCmdCtrl->u32CommCmdPre ^ u32CommCmdCur)
            {
                ctrlword_t* pCtrlWord = (ctrlword_t*) &u32CommCmdCur;

                if (pCtrlWord->ServoOn)
                {
                    u16AxisFSM_i(eAxisNo) = AXIS_STATE_ENABLE;
                    PWM_Start(eAxisNo);
                }
                else
                {
                    u16AxisFSM_i(eAxisNo) = AXIS_STATE_STANDBY;
                    PWM_Stop(eAxisNo);
                }

                pCmdCtrl->u32CommCmdPre = u32CommCmdCur;
            }

            break;
        }
        default:
        {
            break;
        }
    }
}

void CmdCtrlIsr(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo)
{
}
