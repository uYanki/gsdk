#include "appmgr.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "appmgr"
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

void AppMgrCreat(app_mgr_t* pAppMgr, axis_e eAxisNo)
{
    pAppMgr->u16AppSelPre = AXIS_APP_INVAILD;
    pAppMgr->pu16AppSel_i = &P(eAxisNo).u16AppSel;

    OpenLoopCreat(&pAppMgr->sOpenLoop, eAxisNo);
    FuncTestCreat(&pAppMgr->sFuncTest, eAxisNo);
    MotEncIdentCreat(&pAppMgr->sMotEncIdent, eAxisNo);
}

void AppMgrInit(app_mgr_t* pAppMgr)
{
    OpenLoopInit(&pAppMgr->sOpenLoop);
    FuncTestInit(&pAppMgr->sFuncTest);
    MotEncIdentInit(&pAppMgr->sMotEncIdent);
}

void AppMgrCycle(app_mgr_t* pAppMgr)
{
    if (pAppMgr->u16AppSelPre != __get_u16(pAppMgr->pu16AppSel_i))
    {
        // 切换 APP

        switch (pAppMgr->u16AppSelPre)
        {
            case AXIS_APP_GENERIC: {
                break;
            }

            case AXIS_APP_OPENLOOP: {
                OpenLoopExit(&pAppMgr->sOpenLoop);
                break;
            }

            case AXIS_APP_FUNCTEST: {
                FuncTestExit(&pAppMgr->sFuncTest);
                break;
            }

            case AXIS_APP_ENCIDENT: {
                MotEncIdentExit(&pAppMgr->sMotEncIdent);
                break;
            }

            default: {
                break;
            }
        }

        switch (__get_u16(pAppMgr->pu16AppSel_i))
        {
            case AXIS_APP_GENERIC: {
                break;
            }

            case AXIS_APP_OPENLOOP: {
                OpenLoopEnter(&pAppMgr->sOpenLoop);
                break;
            }

            case AXIS_APP_FUNCTEST: {
                FuncTestEnter(&pAppMgr->sFuncTest);
                break;
            }

            case AXIS_APP_ENCIDENT: {
                MotEncIdentEnter(&pAppMgr->sMotEncIdent);
                break;
            }

            default: {
                break;
            }
        }

        pAppMgr->u16AppSelPre = __get_u16(pAppMgr->pu16AppSel_i);
    }
    else
    {
        switch (__get_u16(pAppMgr->pu16AppSel_i))
        {
            case AXIS_APP_GENERIC: {
                break;
            }

            case AXIS_APP_OPENLOOP: {
                OpenLoopCycle(&pAppMgr->sOpenLoop);
                break;
            }

            case AXIS_APP_FUNCTEST: {
                FuncTestCycle(&pAppMgr->sFuncTest);
                break;
            }

            case AXIS_APP_ENCIDENT: {
                MotEncIdentCycle(&pAppMgr->sMotEncIdent);
                break;
            }

            default: {
                break;
            }
        }
    }
}

void AppMgrIsr(app_mgr_t* pAppMgr)
{
    switch (__get_u16(pAppMgr->pu16AppSel_i))
    {
        case AXIS_APP_GENERIC: {
            break;
        }

        case AXIS_APP_OPENLOOP: {
            OpenLoopIsr(&pAppMgr->sOpenLoop);
            break;
        }

        case AXIS_APP_FUNCTEST: {
            FuncTestIsr(&pAppMgr->sFuncTest);
            break;
        }

        case AXIS_APP_ENCIDENT: {
            MotEncIdentIsr(&pAppMgr->sMotEncIdent);
            break;
        }

        default: {
            break;
        }
    }
}
