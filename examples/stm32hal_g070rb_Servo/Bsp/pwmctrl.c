#include "./pwmctrl.h"

#include "tim.h"

#include "gconf.h"
#include "paratbl.h"

// space vertor
#if (CONFIG_AXIS_NUM >= 1)
    #define AXIS1_TIM htim1
#endif
#if (CONFIG_AXIS_NUM >= 2)
    #define AXIS2_TIM
#endif

void PWM_Start(axis_e eAxisNo)
{
    u16 u16Duty = P(eAxisNo).u16PwmDutyMax >> 1;

    switch (eAxisNo)
    {
#if (CONFIG_AXIS_NUM >= 1)
        case AXIS_1 :
        {
        //    PWM_SetDuty(u16Duty, u16Duty, u16Duty, eAxisNo);
            HAL_TIM_PWM_Start(&AXIS1_TIM, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);
					  HAL_TIMEx_PWMN_Start(&AXIS1_TIM, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);
            break;
        }
#endif
#if (CONFIG_AXIS_NUM >= 2)
        case AXIS_2 :
        {
            break;
        }
#endif
        default : break;
    }
}

void PWM_Stop(axis_e eAxisNo)
{
    switch (eAxisNo)
    {
#if (CONFIG_AXIS_NUM >= 1)
        case AXIS_1 :
        {
            HAL_TIM_PWM_Stop(&AXIS1_TIM, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);
					  HAL_TIMEx_PWMN_Stop(&AXIS1_TIM, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);
            break;
        }
#endif
#if (CONFIG_AXIS_NUM >= 2)
        case AXIS_2 :
        {
            break;
        }
#endif
        default : break;
    }
}

void PWM_SetDuty(u16 u16DutyA, u16 u16DutyB, u16 u16DutyC, axis_e eAxisNo)
{
    switch (eAxisNo)
    {
#if (CONFIG_AXIS_NUM >= 1)
        case AXIS_1 :
        {
            __HAL_TIM_SetCompare(&AXIS1_TIM, TIM_CHANNEL_1, u16DutyA);
            __HAL_TIM_SetCompare(&AXIS1_TIM, TIM_CHANNEL_2, u16DutyB);
            __HAL_TIM_SetCompare(&AXIS1_TIM, TIM_CHANNEL_3, u16DutyC);
            break;
        }
#endif
#if (CONFIG_AXIS_NUM >= 2)
        case AXIS_2 :
        {
            break;
        }
#endif
        default : break;
    }
}
