#include "appmgr.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG        "appmgr"
#define LOG_LOCAL_LEVEL      LOG_LEVEL_INFO

#define u16AppSel_i(eAxisNo) P(eAxisNo).u16AppSel

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
    OpenLoopCreat(&pAppMgr->sOpenLoop, eAxisNo);
    FuncTestCreat(&pAppMgr->sFuncTest, eAxisNo);
    EncIdentCreat(&pAppMgr->sEncIdent, eAxisNo);
}

void AppMgrInit(app_mgr_t* pAppMgr, axis_e eAxisNo)
{
    OpenLoopInit(&pAppMgr->sOpenLoop, eAxisNo);
    FuncTestInit(&pAppMgr->sFuncTest, eAxisNo);
    EncIdentInit(&pAppMgr->sEncIdent, eAxisNo);
}

void AppMgrCycle(app_mgr_t* pAppMgr, axis_e eAxisNo)
{
    if (pAppMgr->u16AppSelPre != u16AppSel_i(eAxisNo))
    {
        // 切换 APP

        switch (pAppMgr->u16AppSelPre)
        {
            case AXIS_APP_GENERIC:
            {
                break;
            }

            case AXIS_APP_OPENLOOP:
            {
                OpenLoopExit(&pAppMgr->sOpenLoop, eAxisNo);
                break;
            }

            case AXIS_APP_FUNCTEST:
            {
                FuncTestExit(&pAppMgr->sFuncTest, eAxisNo);
                break;
            }

            case AXIS_APP_ENCIDENT:
            {
                EncIdentExit(&pAppMgr->sEncIdent, eAxisNo);
                break;
            }

            default:
            {
                break;
            }
        }

        switch (u16AppSel_i(eAxisNo))
        {
            case AXIS_APP_GENERIC:
            {
                break;
            }

            case AXIS_APP_OPENLOOP:
            {
                OpenLoopEnter(&pAppMgr->sOpenLoop, eAxisNo);
                break;
            }

            case AXIS_APP_FUNCTEST:
            {
                FuncTestEnter(&pAppMgr->sFuncTest, eAxisNo);
                break;
            }

            case AXIS_APP_ENCIDENT:
            {
                EncIdentEnter(&pAppMgr->sEncIdent, eAxisNo);
                break;
            }

            default:
            {
                break;
            }
        }

        pAppMgr->u16AppSelPre = u16AppSel_i(eAxisNo);
    }
    else
    {
        switch (u16AppSel_i(eAxisNo))
        {
            case AXIS_APP_GENERIC:
            {
                break;
            }

            case AXIS_APP_OPENLOOP:
            {
                OpenLoopCycle(&pAppMgr->sOpenLoop, eAxisNo);
                break;
            }

            case AXIS_APP_FUNCTEST:
            {
                FuncTestCycle(&pAppMgr->sFuncTest, eAxisNo);
                break;
            }

            case AXIS_APP_ENCIDENT:
            {
                EncIdentCycle(&pAppMgr->sEncIdent, eAxisNo);
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

void AppMgrIsr(app_mgr_t* pAppMgr, axis_e eAxisNo)
{
    switch (u16AppSel_i(eAxisNo))
    {
        case AXIS_APP_GENERIC:
        {
            break;
        }

        case AXIS_APP_OPENLOOP:
        {
            OpenLoopIsr(&pAppMgr->sOpenLoop, eAxisNo);
            break;
        }

        case AXIS_APP_FUNCTEST:
        {
            FuncTestIsr(&pAppMgr->sFuncTest, eAxisNo);
            break;
        }

        case AXIS_APP_ENCIDENT:
        {
            EncIdentIsr(&pAppMgr->sEncIdent, eAxisNo);
            break;
        }

        default:
        {
            break;
        }
    }
}
