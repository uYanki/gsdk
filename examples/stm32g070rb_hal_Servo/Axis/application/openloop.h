#ifndef __OPENLOOP_H__
#define __OPENLOOP_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    // 轴状态
    u16* pu16AxisFSM_i;

    // 电角度初值
    s16* pu16ElecAngInit_i;

    // 电角度增量
    s16* ps16ElecAngInc_i;
    u16* pu16ElecAngIncPrd_i;  // 递增间隔
    u16  u16ElecAngIncCnt;     // 间隔计数

    // DQ电压指令
    s16* ps16UdRef_i;
    s16* ps16UqRef_i;

    // 指令输出
    s16* ps16UdRef_o;
    s16* ps16UqRef_o;
    u16* u16ElecAngRef_o;
} open_loop_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void OpenLoopCreat(open_loop_t* pOpenLoop, axis_e eAxisNo);
void OpenLoopInit(open_loop_t* pOpenLoop);
void OpenLoopEnter(open_loop_t* pOpenLoop);
void OpenLoopExit(open_loop_t* pOpenLoop);
void OpenLoopCycle(open_loop_t* pOpenLoop);
void OpenLoopIsr(open_loop_t* pOpenLoop);

#ifdef __cplusplus
}
#endif

#endif
