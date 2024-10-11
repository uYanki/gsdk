#include "openloop.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "openloop"
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

void OpenLoopCreat(open_loop_t* pOpenLoop, axis_e eAxisNo)
{
    pOpenLoop->pu16AxisFSM_i = &P(eAxisNo).u16AxisFSM;

    pOpenLoop->ps16UqRef_i = &P(eAxisNo).s16OpenUqRef;
    pOpenLoop->ps16UdRef_i = &P(eAxisNo).s16OpenUdRef;

    pOpenLoop->pu16ElecAngInit_i   = &P(eAxisNo).u16OpenElecAngInit;
    pOpenLoop->ps16ElecAngInc_i    = &P(eAxisNo).s16OpenElecAngInc;
    pOpenLoop->pu16ElecAngIncPrd_i = &P(eAxisNo).u16OpenPeriod;
    pOpenLoop->u16ElecAngRef_o     = &P(eAxisNo).u16ElecAngleRef;

    pOpenLoop->ps16UqRef_o = &P(eAxisNo).s16Uq;
    pOpenLoop->ps16UdRef_o = &P(eAxisNo).s16Ud;
}

void OpenLoopInit(open_loop_t* pOpenLoop)
{
}

void OpenLoopEnter(open_loop_t* pOpenLoop)
{
    pOpenLoop->u16ElecAngIncCnt = 0;
}

void OpenLoopExit(open_loop_t* pOpenLoop)
{
}

void OpenLoopCycle(open_loop_t* pOpenLoop)
{
}

void OpenLoopIsr(open_loop_t* pOpenLoop)
{
    if (__get_u16(pOpenLoop->pu16AxisFSM_i) == AXIS_STATE_ENABLE)  // 轴使能后再执行
    {
        if (__get_u16(pOpenLoop->pu16ElecAngIncPrd_i) == 0)
        {
            __set_u16(pOpenLoop->u16ElecAngRef_o, __get_u16(pOpenLoop->pu16ElecAngInit_i));  // 锁定电角度
        }
        else if (__get_u16(pOpenLoop->pu16ElecAngIncPrd_i) == ++pOpenLoop->u16ElecAngIncCnt)
        {
            pOpenLoop->u16ElecAngIncCnt = 0;
            __inc_u16(pOpenLoop->u16ElecAngRef_o, (u16)__get_s16(pOpenLoop->ps16ElecAngInc_i));  // 递增电角度
        }

        __set_u16(pOpenLoop->ps16UqRef_o, __get_u16(pOpenLoop->ps16UqRef_i));
        __set_u16(pOpenLoop->ps16UdRef_o, __get_u16(pOpenLoop->ps16UdRef_i));
    }
}
