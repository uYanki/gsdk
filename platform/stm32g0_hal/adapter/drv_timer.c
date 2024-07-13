#include "gsdk.h"
#include "delay.h"

#ifndef CONFIG_TIMEBASE_SOURCE
#define CONFIG_TIMEBASE_SOURCE TIMEBASE_TIM_UP
#endif

#define LOG_LOCAL_TAG   "drv_tim"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief CONFIG_TIMEBASE_SOURCE
 */
#define TIMEBASE_SYSTICK 0 /*<! SysTick, counter down */
#define TIMEBASE_TIM_UP  1 /*<! TIM, counter up */
// #define TIMEBASE_TIM_DOWN 2 /*<! TIM, counter down */

#if (CONFIG_TIMEBASE_SOURCE == TIMEBASE_SYSTICK)

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

tick_t GetTickUs(void)
{
    /**
     * @note SysTick 为 24-bit 递减计数器，当 SysTick->VAL 减到 0 时会将 SysTick->LOAD 里的值赋到 SysTick->VAL 里.
     */

    tick_t TimeUs   = 0;
    tick_t PeriodUs = 1000 / uwTickFreq;

    TimeUs += PeriodUs * HAL_GetTick();
    TimeUs += PeriodUs * (SysTick->LOAD - SysTick->VAL) / SysTick->LOAD;

    return TimeUs;
}

#elif (CONFIG_TIMEBASE_SOURCE == TIMEBASE_TIM_UP)

#include "tim.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define htimx htim14

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static volatile tick_t m_Tick = 0U;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief redirect DelayInit()
 */
void $Sub$$DelayInit()
{
    $Super$$DelayInit();

    // clkin = 64MHz
    // psc = 4 => tick = 16MHz = 1/16 us
    // arr = 16000 => irq freq = arr * psc = 1KHz;
    // GetTick100ns(){ n * tick * 16 / 10 }

    MX_TIM14_Init();

    htimx.Instance               = TIM14;  // APB2 64MHz
    htimx.Init.Prescaler         = 4 - 1;
    htimx.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htimx.Init.Period            = 16000 - 1;
    htimx.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htimx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_Base_Init(&htimx);

    HAL_NVIC_SetPriority(TIM14_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM14_IRQn);

    HAL_TIM_Base_Start_IT(&htimx);
}

tick_t GetTick100ns(void)
{
    return (tick_t)(m_Tick) * (tick_t)(10000) + (tick_t)(htimx.Instance->CNT * 16 / 10);
}

tick_t GetTickUs(void)
{
    return GetTick100ns() / 10;
}

/**
 * @brief redirect HAL_TIM_PeriodElapsedCallback()
 */
void $Sub$$HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    extern void $Super$$HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim);

    if (htim == &htimx)  // period = 1ms
    {
        m_Tick++;  // IncTick()
    }

    $Super$$HAL_TIM_PeriodElapsedCallback(htim);
}

#endif
