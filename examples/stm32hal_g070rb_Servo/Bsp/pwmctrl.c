#include "./pwmctrl.h"

#include "tim.h"

#include "gconf.h"
#include "paratbl.h"

// space vertor
#if (CONFIG_AXIS_NUM >= 1)
#define AXIS0_TIM htim1
#endif
#if (CONFIG_AXIS_NUM >= 2)
#define AXIS1_TIM
#endif

#define htim &htim1

void PWM_Start(axis_e eAxisNo)
{
    u16 u16Duty = P(eAxisNo).u16PwmDutyMax >> 1;

    switch (eAxisNo)
    {
#if (CONFIG_AXIS_NUM >= 1)
        case AXIS_0:
        {
            PWM_SetDuty(u16Duty, u16Duty, u16Duty, eAxisNo);
            HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(htim, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(htim, TIM_CHANNEL_2);
            HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(htim, TIM_CHANNEL_3);
#if 0
            // 不要写成这种形式！！
            HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(htim, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);
#endif
            break;
        }
#endif
#if (CONFIG_AXIS_NUM >= 2)
        case AXIS_1:
        {
            break;
        }
#endif
        default: break;
    }
}

void PWM_Stop(axis_e eAxisNo)
{
    u16 u16Duty = P(eAxisNo).u16PwmDutyMax >> 1;

    switch (eAxisNo)
    {
#if (CONFIG_AXIS_NUM >= 1)
        case AXIS_0:
        {
            PWM_SetDuty(u16Duty, u16Duty, u16Duty, eAxisNo);
            HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Stop(htim, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Stop(htim, TIM_CHANNEL_2);
            HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Stop(htim, TIM_CHANNEL_3);
            break;
        }
#endif
#if (CONFIG_AXIS_NUM >= 2)
        case AXIS_1:
        {
            break;
        }
#endif
        default: break;
    }
}

void PWM_SetDuty(u16 u16DutyA, u16 u16DutyB, u16 u16DutyC, axis_e eAxisNo)
{
    switch (eAxisNo)
    {
#if (CONFIG_AXIS_NUM >= 1)
        case AXIS_0:
        {
            __HAL_TIM_SetCompare(&AXIS0_TIM, TIM_CHANNEL_1, u16DutyA);
            __HAL_TIM_SetCompare(&AXIS0_TIM, TIM_CHANNEL_2, u16DutyB);
            __HAL_TIM_SetCompare(&AXIS0_TIM, TIM_CHANNEL_3, u16DutyC);
            break;
        }
#endif
#if (CONFIG_AXIS_NUM >= 2)
        case AXIS_1:
        {
            break;
        }
#endif
        default: break;
    }
}
