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
} cmd_ctrl_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void CmdCtrlCreat(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo);
void CmdCtrlInit(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo);
void CmdCtrlCycle(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo);
void CmdCtrlIsr(cmd_ctrl_t* pCmdCtrl, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
