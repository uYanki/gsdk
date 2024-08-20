#ifndef __APPL_H__
#define __APPL_H__

#include "paratbl.h"

#include "openloop.h"
#include "motencident.h"
#include "functest.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    u16             u16AppSelPre;
    open_loop_t     sOpenLoop;
    mot_enc_ident_t sMotEncIdent;
    func_test_t     sFuncTest;
} app_mgr_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void AppMgrCreat(app_mgr_t* pAppMgr, axis_e eAxisNo);
void AppMgrInit(app_mgr_t* pAppMgr, axis_e eAxisNo);
void AppMgrCycle(app_mgr_t* pAppMgr, axis_e eAxisNo);
void AppMgrIsr(app_mgr_t* pAppMgr, axis_e eAxisNo);

#ifdef __cplusplus
}
#endif

#endif
