#include "i2c_ina219.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG    "ina219"
#define LOG_LOCAL_LEVEL  LOG_LEVEL_DEBUG

#define CMD_READ         (0x01)

#define REG_CONFIG       (0x00)
#define REG_SHUNTVOLTAGE (0x01)
#define REG_BUSVOLTAGE   (0x02)
#define REG_POWER        (0x03)
#define REG_CURRENT      (0x04)
#define REG_CALIBRATION  (0x05)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static err_t INA219_ReadWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data);
static err_t INA219_WriteWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static err_t INA219_ReadWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t* u16Data)
{
    return I2C_Master_ReadWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_7BIT_SLVADDR | I2C_FLAG_8BIT_MEMADDR | I2C_FLAG_WORD_LITTLE_ENDIAN);
}

static err_t INA219_WriteWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_7BIT_SLVADDR | I2C_FLAG_8BIT_MEMADDR | I2C_FLAG_WORD_LITTLE_ENDIAN);
}

static err_t INA219_ReadWordBits(i2c_ina219_t* pHandle, uint16_t u16MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint16_t* pu16BitsValue)
{
    return I2C_Master_ReadWordBits(pHandle->hI2C, pHandle->u8SlvAddr, u16MemAddr, u8StartBit, u8BitsCount, pu16BitsValue, I2C_FLAG_7BIT_SLVADDR | I2C_FLAG_8BIT_MEMADDR | I2C_FLAG_WORD_LITTLE_ENDIAN);
}

err_t INA219_Init(i2c_ina219_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_7BIT_SLVADDR) == false)
    {
        return ERR_NOT_EXIST;  // device doesn't exist
    }

    return ERR_NONE;
}

err_t INA219_Configure(i2c_ina219_t* pHandle, ina219_range_e eRange, ina219_gain_e eGain, ina219_bus_res_e eBusRes, ina219_shunt_res_e eShuntRes, ina219_mode_e eMode)
{
    uint16_t u16Config = 0;

    u16Config |= (eRange << 13 | eGain << 11 | eBusRes << 7 | eShuntRes << 3 | eMode);

    switch (eRange)
    {
        case INA219_RANGE_32V: pHandle->vBusMax = 32.0f; break;
        case INA219_RANGE_16V: pHandle->vBusMax = 16.0f; break;
        default: return ERR_INVALID_VALUE;
    }

    switch (eGain)
    {
        case INA219_GAIN_320MV: pHandle->vShuntMax = 0.32f; break;
        case INA219_GAIN_160MV: pHandle->vShuntMax = 0.16f; break;
        case INA219_GAIN_80MV: pHandle->vShuntMax = 0.08f; break;
        case INA219_GAIN_40MV: pHandle->vShuntMax = 0.04f; break;
        default: return ERR_INVALID_VALUE;
    }

    INA219_WriteWord(pHandle, REG_CONFIG, u16Config);

    return ERR_NONE;
}

err_t INA219_Calibrate(i2c_ina219_t* pHandle, float32_t f32RShuntValue, float32_t f32IMaxExpected)
{
    uint16_t u16CalibrationValue;
    pHandle->rShunt = f32RShuntValue;

    float32_t iMaxPossible, minimumLSB;

    iMaxPossible = pHandle->vShuntMax / pHandle->rShunt;

    minimumLSB = f32IMaxExpected / 32767;

    pHandle->currentLSB = (uint16_t)(minimumLSB * 100000000.f);
    pHandle->currentLSB /= 100000000.f;
    pHandle->currentLSB /= 0.0001f;
    pHandle->currentLSB = ceil(pHandle->currentLSB);
    pHandle->currentLSB *= 0.0001f;

    pHandle->powerLSB = pHandle->currentLSB * 20;

    u16CalibrationValue = (uint16_t)((0.04096f) / (pHandle->currentLSB * pHandle->rShunt));

    INA219_WriteWord(pHandle, REG_CALIBRATION, u16CalibrationValue);

    return ERR_NONE;
}

float32_t INA219_GetMaxPossibleCurrent(i2c_ina219_t* pHandle)
{
    return (pHandle->vShuntMax / pHandle->rShunt);
}

float32_t INA219_GetMaxCurrent(i2c_ina219_t* pHandle)
{
    float32_t f32MaxCurrent  = (pHandle->currentLSB * 32767);
    float32_t f32MaxPossible = INA219_GetMaxPossibleCurrent(pHandle);

    return MIN(f32MaxCurrent, f32MaxPossible);
}

float32_t INA219_GetMaxShuntVoltage(i2c_ina219_t* pHandle)
{
    float32_t f32MaxVoltage = INA219_GetMaxCurrent(pHandle) * pHandle->rShunt;

    return MIN(f32MaxVoltage, pHandle->vShuntMax);
}

float32_t INA219_GetMaxPower(i2c_ina219_t* pHandle)
{
    return INA219_GetMaxCurrent(pHandle) * pHandle->vBusMax;
}

float32_t INA219_ReadBusPower(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_POWER, &u16Data);

    return u16Data * pHandle->powerLSB;
}

float32_t INA219_ReadShuntCurrent(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_CURRENT, &u16Data);

    return u16Data * pHandle->currentLSB;
}

float32_t INA219_ReadShuntVoltage(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_SHUNTVOLTAGE, u16Data);

    return u16Data / 100000.f;
}

float32_t INA219_ReadBusVoltage(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_BUSVOLTAGE, &u16Data);

    return (u16Data >> 3) * 0.004f;
}

ina219_range_e INA219_GetRange(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 13, 1, &u16Data);

    return (ina219_range_e)u16Data;
}

ina219_gain_e INA219_GetGain(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 11, 2, &u16Data);

    return (ina219_gain_e)u16Data;
}

ina219_bus_res_e INA219_GetBusRes(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 7, 4, &u16Data);

    return (ina219_bus_res_e)u16Data;
}

ina219_shunt_res_e INA219_GetShuntRes(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 3, 4, &u16Data);

    return (ina219_shunt_res_e)u16Data;
}

ina219_mode_e INA219_GetMode(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 0, 3, &u16Data);

    return (ina219_mode_e)u16Data;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void INA219_Test(i2c_mst_t* hI2C)
{
    i2c_ina219_t ina219 = {
        .hI2C      = hI2C,
        .u8SlvAddr = INA219_ADDRESS,
    };

    // Default INA219 address is 0x40
    INA219_Init(&ina219);

    // Configure INA219
    INA219_Configure(&ina219, INA219_RANGE_32V, INA219_GAIN_320MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_BUS_CONT);

    // Calibrate INA219. Rshunt = 0.1 ohm, Max excepted current = 2A
    INA219_Calibrate(&ina219, 0.1f, 2.f);

    // Display configuration
    PRINTF("Mode:                 ");
    switch (INA219_GetMode(&ina219))
    {
        case INA219_MODE_POWER_DOWN: PRINTLN("Power-Down"); break;
        case INA219_MODE_SHUNT_TRIG: PRINTLN("Shunt Voltage, Triggered"); break;
        case INA219_MODE_BUS_TRIG: PRINTLN("Bus Voltage, Triggered"); break;
        case INA219_MODE_SHUNT_BUS_TRIG: PRINTLN("Shunt and Bus, Triggered"); break;
        case INA219_MODE_ADC_OFF: PRINTLN("ADC Off"); break;
        case INA219_MODE_SHUNT_CONT: PRINTLN("Shunt Voltage, Continuous"); break;
        case INA219_MODE_BUS_CONT: PRINTLN("Bus Voltage, Continuous"); break;
        case INA219_MODE_SHUNT_BUS_CONT: PRINTLN("Shunt and Bus, Continuous"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Range:                ");
    switch (INA219_GetRange(&ina219))
    {
        case INA219_RANGE_16V: PRINTLN("16V"); break;
        case INA219_RANGE_32V: PRINTLN("32V"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Gain:                 ");
    switch (INA219_GetGain(&ina219))
    {
        case INA219_GAIN_40MV: PRINTLN("+/- 40mV"); break;
        case INA219_GAIN_80MV: PRINTLN("+/- 80mV"); break;
        case INA219_GAIN_160MV: PRINTLN("+/- 160mV"); break;
        case INA219_GAIN_320MV: PRINTLN("+/- 320mV"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Bus resolution:       ");
    switch (INA219_GetBusRes(&ina219))
    {
        case INA219_BUS_RES_9BIT: PRINTLN("9-bit"); break;
        case INA219_BUS_RES_10BIT: PRINTLN("10-bit"); break;
        case INA219_BUS_RES_11BIT: PRINTLN("11-bit"); break;
        case INA219_BUS_RES_12BIT: PRINTLN("12-bit"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Shunt resolution:     ");
    switch (INA219_GetShuntRes(&ina219))
    {
        case INA219_SHUNT_RES_9BIT_1S: PRINTLN("9-bit / 1 sample"); break;
        case INA219_SHUNT_RES_10BIT_1S: PRINTLN("10-bit / 1 sample"); break;
        case INA219_SHUNT_RES_11BIT_1S: PRINTLN("11-bit / 1 sample"); break;
        case INA219_SHUNT_RES_12BIT_1S: PRINTLN("12-bit / 1 sample"); break;
        case INA219_SHUNT_RES_12BIT_2S: PRINTLN("12-bit / 2 samples"); break;
        case INA219_SHUNT_RES_12BIT_4S: PRINTLN("12-bit / 4 samples"); break;
        case INA219_SHUNT_RES_12BIT_8S: PRINTLN("12-bit / 8 samples"); break;
        case INA219_SHUNT_RES_12BIT_16S: PRINTLN("12-bit / 16 samples"); break;
        case INA219_SHUNT_RES_12BIT_32S: PRINTLN("12-bit / 32 samples"); break;
        case INA219_SHUNT_RES_12BIT_64S: PRINTLN("12-bit / 64 samples"); break;
        case INA219_SHUNT_RES_12BIT_128S: PRINTLN("12-bit / 128 samples"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTLN("Max possible current: %.5f A", INA219_GetMaxPossibleCurrent(&ina219));
    PRINTLN("Max current:          %.5f A", INA219_GetMaxCurrent(&ina219));
    PRINTLN("Max shunt voltage:    %.5f V", INA219_GetMaxShuntVoltage(&ina219));
    PRINTLN("Max power:            %.5f W", INA219_GetMaxPower(&ina219));

    while (1)
    {
        PRINTLN("Bus voltage:   %.5f V", INA219_ReadBusVoltage(&ina219));
        PRINTLN("Bus power:     %.5f W", INA219_ReadBusPower(&ina219));
        PRINTLN("Shunt voltage: %.5f V", INA219_ReadShuntVoltage(&ina219));
        PRINTLN("Shunt current: %.5f A", INA219_ReadShuntCurrent(&ina219));
        DelayBlockMs(1000);
    }
}

#endif