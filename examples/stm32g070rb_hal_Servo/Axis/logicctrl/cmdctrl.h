#ifndef __CMD_CTRL_H__
#define __CMD_CTRL_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u32 u32CommCmdPre;

    u32* pu32CommCmd_i;
    u16* pu16AxisFSM_io;
    u16* pu16CtrlCmdSrc_i;
} cmd_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void CmdCtrlCreat(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo);
void CmdCtrlInit(cmd_ctrl_t* pCmdCtrl);
void CmdCtrlCycle(cmd_ctrl_t* pCmdCtrl);
void CmdCtrlIsr(cmd_ctrl_t* pCmdCtrl);

#ifdef __cplusplus
}
#endif

#endif
