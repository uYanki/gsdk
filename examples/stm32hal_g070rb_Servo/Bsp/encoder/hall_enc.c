#include "hall_enc.h"

// ST HAL 霍尔模式配置 https://zhuanlan.zhihu.com/p/126478231
// 霍尔位置的对应角度 https://zhuanlan.zhihu.com/p/462932301
// 霍尔传感器信号超前于转子π/6，因此，电角度 = 霍尔角度 - π/6。正向旋转时

#define SPD_RPM(a) (a)*10

#if CONFIG_HALL_ENC_SW

//-------------------------------------
// port

#include "gpio.h"

#define ENC_HALL_A_PIN GPIOA, GPIO_PIN_6
#define ENC_HALL_B_PIN GPIOA, GPIO_PIN_7
#define ENC_HALL_C_PIN GPIOB, GPIO_PIN_0

#define ENC_HALL_A()   (HAL_GPIO_ReadPin(ENC_HALL_A_PIN) == GPIO_PIN_SET)
#define ENC_HALL_B()   (HAL_GPIO_ReadPin(ENC_HALL_B_PIN) == GPIO_PIN_SET)
#define ENC_HALL_C()   (HAL_GPIO_ReadPin(ENC_HALL_C_PIN) == GPIO_PIN_SET)

//-------------------------------------

hall_state_e ReadHallState(void)
{
    u8 u8HallStateCur = 0;

#if 0  // Sensor Placement = DEGREES_60
    u8HallStateCur |= (ENC_HALL_B() ^ 1) << 2;
    u8HallStateCur |=  ENC_HALL_C() << 1;
    u8HallStateCur |=  ENC_HALL_A() << 0;
#else  // Sensor Placement = DEGREES_120
    u8HallStateCur |= ENC_HALL_C() << 2;
    u8HallStateCur |= ENC_HALL_B() << 1;
    u8HallStateCur |= ENC_HALL_A() << 0;
#endif

    return (hall_state_e)(u8HallStateCur);
}

static void HallEnc_ReadState(hall_enc_t* pEnc)
{
    hall_state_e eHallStateCur = 0, eHallStatePrev;

    do
    {
        eHallStatePrev = eHallStateCur;
        eHallStateCur  = pEnc->pfnReadHallState();

    } while (eHallStateCur != eHallStatePrev);

    pEnc->eHallState = eHallStateCur;

    // order: 513264 or 546231
    switch (pEnc->eHallState)
    {
        case HALL_STATE_5:
            pEnc->u16ElecAngle = HALL_ANGLE_30 * 1;  // 30
            break;
        case HALL_STATE_4:
            pEnc->u16ElecAngle = HALL_ANGLE_30 * 3;  // 90
            break;
        case HALL_STATE_6:
            pEnc->u16ElecAngle = HALL_ANGLE_30 * 5;  // 150
            break;
        case HALL_STATE_2:
            pEnc->u16ElecAngle = HALL_ANGLE_30 * 7;  // 210
            break;
        case HALL_STATE_3:
            pEnc->u16ElecAngle = HALL_ANGLE_30 * 9;  // 270
            break;
        case HALL_STATE_1:
            pEnc->u16ElecAngle = HALL_ANGLE_30 * 11;  // 330
            break;
        default: pEnc->u16HallError++; break;
    }
}

static hall_dir_e HallEnc_GetDir(hall_state_e eHallStatePrev, hall_state_e eHallStateCur)
{
    // order: 513264 or 546231

    static const uint8_t aTblCCW[] = {0x62, 0x23, 0x31, 0x15, 0x54, 0x46, 0x63, 0x21, 0x35, 0x14, 0x56, 0x42};
    static const uint8_t aTblCW[]  = {0x45, 0x51, 0x13, 0x32, 0x26, 0x64, 0x41, 0x53, 0x12, 0x36, 0x24, 0x65};

    uint8_t u8State = (((u8)eHallStatePrev & 0x0F) << 4) | ((u8)eHallStateCur & 0x0F);

    for (uint8_t i = 0; i < ARRAY_SIZE(aTblCW); ++i)
    {
        if (u8State == aTblCW[i])
        {
            return HALL_DIR_FWD;
        }
    }

    for (uint8_t i = 0; i < ARRAY_SIZE(aTblCCW); ++i)
    {
        if (u8State == aTblCCW[i])
        {
            return HALL_DIR_BACK;
        }
    }

    return HALL_DIR_NONE;
}

static void HallEnc_CalcSpd(hall_enc_t* pEnc)
{
#define SPEED_MEASURE_PERIOD_MS 250

    PeriodicTask(SPEED_MEASURE_PERIOD_MS * UNIT_MS, {
        u16 u16DeltaPos    = pEnc->u16EdgeCount;
        pEnc->u16EdgeCount = 0;

        pEnc->s16SpdFb = SPD_RPM(1000 * 60 * u16DeltaPos / pEnc->u16EncRes / SPEED_MEASURE_PERIOD_MS);

        if (u16DeltaPos != 0)
        {
            if (pEnc->eDirection == HALL_DIR_FWD)
            {
                pEnc->u16EncPos += u16DeltaPos;

                if (pEnc->u16EncPos > pEnc->u16EncRes)
                {
                    pEnc->s32EncTurns++;
                    pEnc->u16EncPos -= pEnc->u16EncRes;
                }

                pEnc->s64EncMultPos = pEnc->s32EncTurns * pEnc->u16EncRes + pEnc->u16EncPos;
            }
            else if (pEnc->eDirection == HALL_DIR_BACK)
            {
                pEnc->s16SpdFb = -pEnc->s16SpdFb;

                if (pEnc->u16EncPos >= u16DeltaPos)
                {
                    pEnc->u16EncPos -= u16DeltaPos;
                }
                else
                {
                    pEnc->s32EncTurns--;
                    pEnc->u16EncPos += pEnc->u16EncRes;
                    pEnc->u16EncPos -= u16DeltaPos;
                }

                pEnc->s64EncMultPos = pEnc->s32EncTurns * (s32)pEnc->u16EncRes + (s32)pEnc->u16EncPos;
            }
        }
    });

#undef SPEED_MEASURE_PERIOD_MS

    return;

    // POS_LOOP_FREQ -> 1s
    // POS_LOOP_FREQ/5 -> 0.2s

    /**
     * POS_LOOP_FREQ => 1s
     * POS_LOOP_FREQ/5 => 0.2s
     * RPS = (u16EdgeCount / (6 * MotPolePairs)) * (POS_LOOP_FREQ / u16Tick)
     * RPM = 60 * RPS
     */
}

void HallEncCreat(hall_enc_t* pEnc)
{
    pEnc->pfnReadHallState = ReadHallState;
}

void HallEncInit(hall_enc_t* pEnc)
{
    HallEnc_ReadState(pEnc);
}

// Call in PosLoopIsr
void HallEncIsr(hall_enc_t* pEnc)
{
    hall_state_e eHallStatePrev = pEnc->eHallState;
    hall_dir_e   eDirectionPrev = pEnc->eDirection;

    HallEnc_ReadState(pEnc);

    if (eHallStatePrev != pEnc->eHallState)
    {
        pEnc->eDirection = HallEnc_GetDir(eHallStatePrev, pEnc->eHallState);
        pEnc->u16EdgeCount++;

        if (eDirectionPrev != pEnc->eDirection)
        {
        }
    }

    HallEnc_CalcSpd(pEnc);
}

void HallEncCycle(hall_enc_t* pEnc)
{
}

#endif
