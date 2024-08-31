#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include "gsdk.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool      IsLeapYear(u32 year);
struct tm TS2UTC(time_t time);
struct tm TS2BJ(time_t time);
time_t    UTC2TS(struct tm* pTime);
time_t    BJ2TS(struct tm* pTime);
void      ShowTime(struct tm* pTime);

#if CONFIG_DEMOS_SW
void Timestamp_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
