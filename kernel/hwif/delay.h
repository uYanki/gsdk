#ifndef __DELAY_H__
#define __DELAY_H__

#include "gdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef uint64_t tick_t;

#define UNIT_US (tick_t)1
#define UNIT_MS (tick_t)1000
#define UNIT_S  (tick_t)1000000

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

__IMPL tick_t GetTickUs(void);

void DelayInit(void);
void DelayBlock(tick_t TickWait);
bool DelayNonBlock(const tick_t* cpTickStart, tick_t TickWait);

// clang-format off

static inline tick_t GetTickMs(void) { return GetTickUs() / (tick_t)1000; }
 
static inline void DelayBlockUs(tick_t TickWait) { DelayBlock(TickWait * UNIT_US); }
static inline void DelayBlockMs(tick_t TickWait) { DelayBlock(TickWait * UNIT_MS); }
static inline void DelayBlockS(tick_t TickWait)  { DelayBlock(TickWait * UNIT_S); }

static inline bool DelayNonBlockUs(const tick_t* cpTickStart, tick_t TickWait) { return DelayNonBlock( cpTickStart, TickWait * UNIT_US); }
static inline bool DelayNonBlockMs(const tick_t* cpTickStart, tick_t TickWait) { return DelayNonBlock( cpTickStart, TickWait * UNIT_MS); }
static inline bool DelayNonBlockS(const tick_t* cpTickStart, tick_t TickWait)  { return DelayNonBlock( cpTickStart, TickWait * UNIT_S); }

// clang-format on

#ifdef __cplusplus
}
#endif

#endif /* __DELAY_H__ */
