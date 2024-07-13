#ifndef __I2C_AS5600_H__
#define __I2C_AS5600_H__

#include "i2cmst.h"

/**
 * @brief magnetic rotation meter
 */

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define AS5600_ADDRESS_LOW  0x36
#define AS5600_ADDRESS_HIGH 0x37

typedef enum {
    AS5600_POWER_MODE_NOM  = 0x00,  // normal
    AS5600_POWER_MODE_LPM1 = 0x01,  // low power 1
    AS5600_POWER_MODE_LPM2 = 0x02,  // low power 2
    AS5600_POWER_MODE_LPM3 = 0x03,  // low power 3
} as5600_power_mode_e;

typedef enum {
    AS5600_HYSTERESIS_OFF  = 0x00,  // off
    AS5600_HYSTERESIS_1LSB = 0x01,  // 1 lsb
    AS5600_HYSTERESIS_2LSB = 0x02,  // 2 lsb
    AS5600_HYSTERESIS_3LSB = 0x03,  // 3 lsb
} as5600_hysteresis_e;

typedef enum {
    AS5600_OUTPUT_STAGE_ANALOG_FULL    = 0x00,  // full range from 0% to 100% between gnd and vdd
    AS5600_OUTPUT_STAGE_ANALOG_REDUCED = 0x01,  // reduced range from 10% to 90% between gnd and vdd
    AS5600_OUTPUT_STAGE_PWM            = 0x02,  // digital pwm
} as5600_output_stage_e;

typedef enum {
    AS5600_PWM_FREQUENCY_115HZ = 0x00,  // 115Hz
    AS5600_PWM_FREQUENCY_230HZ = 0x01,  // 230Hz
    AS5600_PWM_FREQUENCY_460HZ = 0x02,  // 460Hz
    AS5600_PWM_FREQUENCY_920HZ = 0x03,  // 920Hz
} as5600_pwm_frequency_e;

typedef enum {
    AS5600_SLOW_FILTER_16X = 0x00,  // 16x
    AS5600_SLOW_FILTER_8X  = 0x01,  // 8x
    AS5600_SLOW_FILTER_4X  = 0x02,  // 4x
    AS5600_SLOW_FILTER_2X  = 0x03,  // 2x
} as5600_slow_filter_e;

typedef enum {
    AS5600_FAST_FILTER_THRESHOLD_SLOW_FILTER_ONLY = 0x00,  // slow filter only
    AS5600_FAST_FILTER_THRESHOLD_6LSB             = 0x01,  // 6 lsb
    AS5600_FAST_FILTER_THRESHOLD_7LSB             = 0x02,  // 7 lsb
    AS5600_FAST_FILTER_THRESHOLD_9LSB             = 0x03,  // 9 lsb
    AS5600_FAST_FILTER_THRESHOLD_10LSB            = 0x07,  // 10 lsb
    AS5600_FAST_FILTER_THRESHOLD_18LSB            = 0x04,  // 18 lsb
    AS5600_FAST_FILTER_THRESHOLD_21LSB            = 0x05,  // 21 lsb
    AS5600_FAST_FILTER_THRESHOLD_24LSB            = 0x06,  // 24 lsb
} as5600_fast_filter_threshold_e;

typedef enum {
    AS5600_STATUS_MD = (1 << 5),  // magnet was detected
    AS5600_STATUS_ML = (1 << 4),  // agc maximum gain overflow, magnet too weak
    AS5600_STATUS_MH = (1 << 3),  // agc minimum gain overflow, magnet too strong
} as5600_status_e;

typedef enum {
    AS5600_BURN_CMD1    = 0x01,  // load the actual otp content command 1
    AS5600_BURN_CMD2    = 0x11,  // load the actual otp content command 2
    AS5600_BURN_CMD3    = 0x10,  // load the actual otp content command 3
    AS5600_BURN_ANGLE   = 0x80,  // angle
    AS5600_BURN_SETTING = 0x40,  // setting
} as5600_burn_e;

typedef struct {
    __IN i2cmst_t* hI2C;
    __IN uint8_t   u8SlvAddr;
} i2c_as5600_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

err_t AS5600_Init(i2c_as5600_t* pHandle);

err_t AS5600_DetectMagnet(i2c_as5600_t* pHandle, bool* pbExist);

err_t AS5600_ReadAngle(i2c_as5600_t* pHandle, uint16_t* pu16Position);
err_t AS5600_ReadRawAngle(i2c_as5600_t* pHandle, uint16_t* pu16Position);

err_t AS5600_SetStartPosition(i2c_as5600_t* pHandle, uint16_t u16Position);
err_t AS5600_GetStartPosition(i2c_as5600_t* pHandle, uint16_t* pu16Position);

err_t AS5600_SetStopPosition(i2c_as5600_t* pHandle, uint16_t u16Position);
err_t AS5600_GetStopPosition(i2c_as5600_t* pHandle, uint16_t* pu16Position);

err_t AS5600_SetMaxAngle(i2c_as5600_t* pHandle, uint16_t u16Angle);
err_t AS5600_GetMaxAngle(i2c_as5600_t* pHandle, uint16_t* pu16Angle);

err_t AS5600_SetWatchDog(i2c_as5600_t* pHandle, bool bEnable);
err_t AS5600_GetWatchDog(i2c_as5600_t* pHandle, bool* pbEnable);

err_t AS5600_SetFastFilterThreshold(i2c_as5600_t* pHandle, as5600_fast_filter_threshold_e eThreshold);
err_t AS5600_GetFastFilterThreshold(i2c_as5600_t* pHandle, as5600_fast_filter_threshold_e* peThreshold);

err_t AS5600_SetSlowFilter(i2c_as5600_t* pHandle, as5600_slow_filter_e eFilter);
err_t AS5600_GetSlowFilter(i2c_as5600_t* pHandle, as5600_slow_filter_e* peFilter);

err_t AS5600_SetPwmFrequency(i2c_as5600_t* pHandle, as5600_pwm_frequency_e eFreq);
err_t AS5600_GetPwmFrequency(i2c_as5600_t* pHandle, as5600_pwm_frequency_e* peFreq);

err_t AS5600_SetOutputStage(i2c_as5600_t* pHandle, as5600_output_stage_e eStage);
err_t AS5600_GetOutputStage(i2c_as5600_t* pHandle, as5600_output_stage_e* peStage);

err_t AS5600_SetHysteresis(i2c_as5600_t* pHandle, as5600_hysteresis_e eHysteresis);
err_t AS5600_GetHysteresis(i2c_as5600_t* pHandle, as5600_hysteresis_e* peHysteresis);

err_t AS5600_SetPowerMode(i2c_as5600_t* pHandle, as5600_power_mode_e eMode);
err_t AS5600_GetPowerMode(i2c_as5600_t* pHandle, as5600_power_mode_e* peMode);

err_t AS5600_GetStatus(i2c_as5600_t* pHandle, uint8_t* pu8Status);

err_t AS5600_GetAGC(i2c_as5600_t* pHandle, uint8_t* pu8AGC);
err_t AS5600_GetMagnitude(i2c_as5600_t* pHandle, uint16_t* pu16Magnitude);

err_t AS5600_SetBurn(i2c_as5600_t* pHandle, as5600_burn_e eBurn);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void AS5600_Test(i2cmst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
