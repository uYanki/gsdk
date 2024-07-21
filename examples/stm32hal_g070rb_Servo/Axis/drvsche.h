#ifndef __DRV_SCHE_H__
#define __DRV_SCHE_H__

#include "axis.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void DrvScheCreat(void);
void DrvScheInit(void);
void DrvScheCycle(void);
void DrvScheIsr(void);

#ifdef __cplusplus
}
#endif

#endif
