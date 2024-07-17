#ifndef __PWM_CTRL_H__
#define __PWM_CTRL_H__

#include "gsdk.h"
#include "paratbl.h"

void PWM_Start(axis_e eAxisNo);
void PWM_Stop(axis_e eAxisNo);
void PWM_SetDuty(u16 u16DutyA, u16 u16DutyB, u16 u16DutyC, axis_e eAxisNo);

#endif
