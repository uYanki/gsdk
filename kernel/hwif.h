#ifndef __HWIF_H__
#define __HWIF_H__

#include "gdefs.h"
#include "i2cmst.h"
#include "spimst.h"
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SystemReset();

#ifdef __cplusplus
}
#endif

#endif
