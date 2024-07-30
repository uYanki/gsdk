#ifndef __I2C_INA219_H__
#define __I2C_INA219_H__

#include "i2cmst.h"

/**
 * @ref https://kkgithub.com/jarzebski/Arduino-INA219
 */

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define INA219_ADDRESS 0x40

typedef enum {
    INA219_RANGE_16V = 0b0,
    INA219_RANGE_32V = 0b1,
} ina219_range_e;

typedef enum {
    INA219_GAIN_40MV  = 0b00,
    INA219_GAIN_80MV  = 0b01,
    INA219_GAIN_160MV = 0b10,
    INA219_GAIN_320MV = 0b11,
} ina219_gain_e;

typedef enum {
    INA219_BUS_RES_9BIT  = 0b0000,
    INA219_BUS_RES_10BIT = 0b0001,
    INA219_BUS_RES_11BIT = 0b0010,
    INA219_BUS_RES_12BIT = 0b0011,
} ina219_bus_res_e;

typedef enum {
    INA219_SHUNT_RES_9BIT_1S    = 0b0000,
    INA219_SHUNT_RES_10BIT_1S   = 0b0001,
    INA219_SHUNT_RES_11BIT_1S   = 0b0010,
    INA219_SHUNT_RES_12BIT_1S   = 0b0011,
    INA219_SHUNT_RES_12BIT_2S   = 0b1001,
    INA219_SHUNT_RES_12BIT_4S   = 0b1010,
    INA219_SHUNT_RES_12BIT_8S   = 0b1011,
    INA219_SHUNT_RES_12BIT_16S  = 0b1100,
    INA219_SHUNT_RES_12BIT_32S  = 0b1101,
    INA219_SHUNT_RES_12BIT_64S  = 0b1110,
    INA219_SHUNT_RES_12BIT_128S = 0b1111
} ina219_shunt_res_e;

typedef enum {
    INA219_MODE_POWER_DOWN     = 0b000,
    INA219_MODE_SHUNT_TRIG     = 0b001,
    INA219_MODE_BUS_TRIG       = 0b010,
    INA219_MODE_SHUNT_BUS_TRIG = 0b011,
    INA219_MODE_ADC_OFF        = 0b100,
    INA219_MODE_SHUNT_CONT     = 0b101,
    INA219_MODE_BUS_CONT       = 0b110,
    INA219_MODE_SHUNT_BUS_CONT = 0b111,
} ina219_mode_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;
    float32_t       currentLSB, powerLSB;
    float32_t       vShuntMax, vBusMax, rShunt;
} i2c_ina219_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

err_t              INA219_Init(i2c_ina219_t* pHandle);
err_t              INA219_Configure(i2c_ina219_t* pHandle, ina219_range_e eRange, ina219_gain_e eGain, ina219_bus_res_e eBusRes, ina219_shunt_res_e eShuntRes, ina219_mode_e eMode);
err_t              INA219_Calibrate(i2c_ina219_t* pHandle, float32_t f32RShuntValue, float32_t f32IMaxExpected);
float32_t          INA219_GetMaxPossibleCurrent(i2c_ina219_t* pHandle);
float32_t          INA219_GetMaxCurrent(i2c_ina219_t* pHandle);
float32_t          INA219_GetMaxShuntVoltage(i2c_ina219_t* pHandle);
float32_t          INA219_GetMaxPower(i2c_ina219_t* pHandle);
float32_t          INA219_ReadBusPower(i2c_ina219_t* pHandle);
float32_t          INA219_ReadShuntCurrent(i2c_ina219_t* pHandle);
float32_t          INA219_ReadShuntVoltage(i2c_ina219_t* pHandle);
float32_t          INA219_ReadBusVoltage(i2c_ina219_t* pHandle);
ina219_range_e     INA219_GetRange(i2c_ina219_t* pHandle);
ina219_gain_e      INA219_GetGain(i2c_ina219_t* pHandle);
ina219_bus_res_e   INA219_GetBusRes(i2c_ina219_t* pHandle);
ina219_shunt_res_e INA219_GetShuntRes(i2c_ina219_t* pHandle);
ina219_mode_e      INA219_GetMode(i2c_ina219_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void INA219_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif

