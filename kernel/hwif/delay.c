#include "delay.h"

#if CONFIG_ARM_DWT_SW
#include "arm_dwt.h"
#endif

#if CONFIG_ARM_TSG_SW
#include "arm_tsg.h"
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void DelayInit(void)
{
#if CONFIG_ARM_DWT_SW
    DWT_Init();
#elif CONFIG_ARM_TSG_SW
    TSG_Init();
#endif
}

void DelayBlock(tick_t TickWait)
{
    if (TickWait == 0)
    {
        TickWait = 1;
    }

#if CONFIG_ARM_DWT_SW

    if (nTickWait < 1000)
    {
        DWT_DelayUS(nTickWait);
        return;
    }

#elif CONFIG_ARM_TSG_SW

    if (nTickWait < 1000)
    {
        TSG_DelayUS(nTickWait);
        return;
    }

#endif

    tick_t TickEnd = GetTickUs() + TickWait;

    while (GetTickUs() < TickEnd)
    {
    }
}

bool DelayNonBlock(const tick_t* cpTickStart, tick_t TickWait)
{
    if ((GetTickUs() - (*cpTickStart)) < TickWait)
    {
        return false;  // waiting
    }
    else
    {
        return true;  // time is up
    }
}
