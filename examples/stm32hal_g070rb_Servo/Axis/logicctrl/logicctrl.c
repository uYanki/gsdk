#include "logicctrl.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "logicctrl"
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

void LogicCtrlCreat(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo)
{
    PosCtrlCreat(&pLogicCtrl->sPosCtrl, eAxisNo);
    TrqCtrlCreat(&pLogicCtrl->sTrqCtrl, eAxisNo);
    SpdCtrlCreat(&pLogicCtrl->sSpdCtrl, eAxisNo);
    CmdCtrlCreat(&pLogicCtrl->sCmdCtrl, eAxisNo);
    HomingCreat(&pLogicCtrl->sHoming, eAxisNo);
}

void LogicCtrlInit(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo)
{
    PosCtrlInit(&pLogicCtrl->sPosCtrl, eAxisNo);
    TrqCtrlInit(&pLogicCtrl->sTrqCtrl, eAxisNo);
    SpdCtrlInit(&pLogicCtrl->sSpdCtrl, eAxisNo);
    CmdCtrlInit(&pLogicCtrl->sCmdCtrl, eAxisNo);
    HomingInit(&pLogicCtrl->sHoming, eAxisNo);
}

void LogicCtrlCycle(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo)
{
    PosCtrlCycle(&pLogicCtrl->sPosCtrl, eAxisNo);
    TrqCtrlCycle(&pLogicCtrl->sTrqCtrl, eAxisNo);
    SpdCtrlCycle(&pLogicCtrl->sSpdCtrl, eAxisNo);
    CmdCtrlCycle(&pLogicCtrl->sCmdCtrl, eAxisNo);
    HomingCycle(&pLogicCtrl->sHoming, eAxisNo);
}

void LogicCtrlIsr(logic_ctrl_t* pLogicCtrl, axis_e eAxisNo)
{
    PosCtrlIsr(&pLogicCtrl->sPosCtrl, eAxisNo);
    TrqCtrlIsr(&pLogicCtrl->sTrqCtrl, eAxisNo);
    SpdCtrlIsr(&pLogicCtrl->sSpdCtrl, eAxisNo);
    CmdCtrlIsr(&pLogicCtrl->sCmdCtrl, eAxisNo);
    HomingIsr(&pLogicCtrl->sHoming, eAxisNo);
}
