#ifndef __HALL_ENC_H__
#define __HALL_ENC_H__

#include "gsdk.h"

#define CONFIG_HALL_ENC_SW 1

#if CONFIG_HALL_ENC_SW

#if 0
#define HALL_ANGLE_30 30u
#else
#define HALL_ANGLE_30 65535u / 12
// #define HALL_ANGLE_30 IncEncRes / 12
// #define HALL_ANGLE_30 AbsEncRes / 12
#endif

typedef enum {
    HALL_DIR_BACK = -1,
    HALL_DIR_NONE = 0,
    HALL_DIR_FWD  = +1,
} hall_dir_e;

typedef enum {
    HALL_STATE_0 = 0,
    HALL_STATE_1 = 1,
    HALL_STATE_2 = 2,
    HALL_STATE_3 = 3,
    HALL_STATE_4 = 4,
    HALL_STATE_5 = 5,
    HALL_STATE_6 = 6,
    HALL_STATE_7 = 0,
} hall_state_e;

typedef struct {
    s16 s16SpdFb;  // [0.1RPM]

    __OUT hall_state_e eHallState;
    hall_dir_e         eDirection;

    u16 u16ElecAngle;
    u16 u16EdgeCount;
    u8  u8PhaseShift;

    u16 u16HallError;

    __OUT u16 u16EncRes;
    __OUT u16 u16EncPos;
    __OUT s32 s32EncTurns;
    __OUT s64 s64EncMultPos;

    hall_state_e (*pfnReadHallState)(void);

} hall_enc_t;

void HallEnc_Creat(hall_enc_t* pEnc);
void HallEnc_Init(hall_enc_t* pEnc);
void HallEnc_Isr(hall_enc_t* pEnc);
void HallEnc_Cycle(hall_enc_t* pEnc);

#endif
#endif
