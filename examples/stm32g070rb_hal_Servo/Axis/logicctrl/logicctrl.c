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
    P(eAxisNo).u16AxisFSM = AXIS_STATE_INITIAL;

    PosCtrlCreat(&pLogicCtrl->sPosCtrl, eAxisNo);
    TrqCtrlCreat(&pLogicCtrl->sTrqCtrl, eAxisNo);
    SpdCtrlCreat(&pLogicCtrl->sSpdCtrl, eAxisNo);
    CmdCtrlCreat(&pLogicCtrl->sCmdCtrl, eAxisNo);
    HomingCreat(&pLogicCtrl->sHoming, eAxisNo);
}

void LogicCtrlInit(logic_ctrl_t* pLogicCtrl)
{
    PosCtrlInit(&pLogicCtrl->sPosCtrl);
    TrqCtrlInit(&pLogicCtrl->sTrqCtrl);
    SpdCtrlInit(&pLogicCtrl->sSpdCtrl);
    CmdCtrlInit(&pLogicCtrl->sCmdCtrl);
    HomingInit(&pLogicCtrl->sHoming);
}

void LogicCtrlCycle(logic_ctrl_t* pLogicCtrl)
{
    PosCtrlCycle(&pLogicCtrl->sPosCtrl);
    TrqCtrlCycle(&pLogicCtrl->sTrqCtrl);
    SpdCtrlCycle(&pLogicCtrl->sSpdCtrl);
    CmdCtrlCycle(&pLogicCtrl->sCmdCtrl);
    HomingCycle(&pLogicCtrl->sHoming);
}

void LogicCtrlIsr(logic_ctrl_t* pLogicCtrl)
{
    PosCtrlIsr(&pLogicCtrl->sPosCtrl);
    TrqCtrlIsr(&pLogicCtrl->sTrqCtrl);
    SpdCtrlIsr(&pLogicCtrl->sSpdCtrl);
    CmdCtrlIsr(&pLogicCtrl->sCmdCtrl);
    HomingIsr(&pLogicCtrl->sHoming);
}
