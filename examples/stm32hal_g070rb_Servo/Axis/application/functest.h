#ifndef __FUNC_TEST_H__
#define __FUNC_TEST_H__

#include "paratbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16 _Resv;
} func_test_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void FuncTestCreat(func_test_t* pFuncTest, axis_e eAxisNo);
void FuncTestInit(func_test_t* pFuncTest, axis_e eAxisNo);
void FuncTestEnter(func_test_t* pFuncTest, axis_e eAxisNo);
void FuncTestExit(func_test_t* pFuncTest, axis_e eAxisNo);
void FuncTestCycle(func_test_t* pFuncTest, axis_e eAxisNo);
void FuncTestIsr(func_test_t* pFuncTest, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
