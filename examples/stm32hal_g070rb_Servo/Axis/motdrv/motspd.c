#include "motspd.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "motspd"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief 过速检测：反馈速度超过电机最高速度的1.2倍
 */
static void _ErrorDetect_OverSpeed(mot_spd_t* pMotSpd, axis_e eAxisNo)
{
}

void MotSpdCreat(mot_spd_t* pMotSpd, axis_e eAxisNo)
{
}

void MotSpdInit(mot_spd_t* pMotSpd, axis_e eAxisNo)
{
}

void MotSpdCycle(mot_spd_t* pMotSpd, axis_e eAxisNo)
{
    _ErrorDetect_OverSpeed(pMotSpd, eAxisNo);
}

void MotSpdIsr(mot_spd_t* pMotSpd, axis_e eAxisNo)
{
}
