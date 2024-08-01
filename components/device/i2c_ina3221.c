#include "i2c_ina3221.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG             "ina3221"
#define LOG_LOCAL_LEVEL           LOG_LEVEL_DEBUG

#define REG_CONF                  0
#define REG_CH1_SHUNTV            1
#define REG_CH1_BUSV              2
#define REG_CH2_SHUNTV            3
#define REG_CH2_BUSV              4
#define REG_CH3_SHUNTV            5
#define REG_CH3_BUSV              6
#define REG_CH1_CRIT_ALERT_LIM    7
#define REG_CH1_WARNING_ALERT_LIM 8
#define REG_CH2_CRIT_ALERT_LIM    9
#define REG_CH2_WARNING_ALERT_LIM 10
#define REG_CH3_CRIT_ALERT_LIM    11
#define REG_CH3_WARNING_ALERT_LIM 12
#define REG_SHUNTV_SUM            13
#define REG_SHUNTV_SUM_LIM        14
#define REG_MASK_ENABLE           15
#define REG_PWR_VALID_HI_LIM      16
#define REG_PWR_VALID_LO_LIM      17
#define REG_MANUF_ID              0xFE
#define REG_DIE_ID                0xFF

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

// Configuration register
typedef struct {
    uint16_t mode_shunt_en      : 1;
    uint16_t mode_bus_en        : 1;
    uint16_t mode_continious_en : 1;
    uint16_t shunt_conv_time    : 3;
    uint16_t bus_conv_time      : 3;
    uint16_t avg_mode           : 3;
    uint16_t ch3_en             : 1;
    uint16_t ch2_en             : 1;
    uint16_t ch1_en             : 1;
    uint16_t reset              : 1;
} ina3221_conf_reg_t;

// Mask/Enable register
typedef struct {
    uint16_t conv_ready          : 1;
    uint16_t timing_ctrl_alert   : 1;
    uint16_t pwr_valid_alert     : 1;
    uint16_t warn_alert_ch3      : 1;
    uint16_t warn_alert_ch2      : 1;
    uint16_t warn_alert_ch1      : 1;
    uint16_t shunt_sum_alert     : 1;
    uint16_t crit_alert_ch3      : 1;
    uint16_t crit_alert_ch2      : 1;
    uint16_t crit_alert_ch1      : 1;
    uint16_t crit_alert_latch_en : 1;
    uint16_t warn_alert_latch_en : 1;
    uint16_t shunt_sum_en_ch3    : 1;
    uint16_t shunt_sum_en_ch2    : 1;
    uint16_t shunt_sum_en_ch1    : 1;
    uint16_t reserved            : 1;
} ina3221_masken_reg_t;

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

// Value of Mask/Enable register.
ina3221_masken_reg_t _masken_reg;

static inline INA3221_ReadWord(i2c_ina3221_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data)
{
    return I2C_Master_ReadWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static inline INA3221_WriteWord(i2c_ina3221_t* pHandle, uint8_t u8MemAddr, uint16_t* u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

err_t INA3221_Init(i2c_ina3221_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NOT_EXIST;  // device doesn't exist
    }

    pHandle->u32ShuntRes[0] = 10;
    pHandle->u32ShuntRes[1] = 10;
    pHandle->u32ShuntRes[2] = 10;

    pHandle->u32FilterRes[0] = 0;
    pHandle->u32FilterRes[1] = 0;
    pHandle->u32FilterRes[2] = 0;

    return ERR_NONE;
}

void INA3221_SetShuntRes(i2c_ina3221_t* pHandle, uint32_t res_ch1, uint32_t res_ch2, uint32_t res_ch3)
{
    pHandle->u32ShuntRes[0] = res_ch1;
    pHandle->u32ShuntRes[1] = res_ch2;
    pHandle->u32ShuntRes[2] = res_ch3;
}

void INA3221_SetFilterRes(i2c_ina3221_t* pHandle, uint32_t res_ch1, uint32_t res_ch2, uint32_t res_ch3)
{
    pHandle->u32FilterRes[0] = res_ch1;
    pHandle->u32FilterRes[1] = res_ch2;
    pHandle->u32FilterRes[2] = res_ch3;
}

uint16_t INA3221_GetReg(i2c_ina3221_t* pHandle, uint8_t reg)
{
    uint16_t val = 0;
    INA3221_ReadWord(pHandle, reg, &val);
    return val;
}

void INA3221_Reset(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.reset = 1;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetModePowerDown(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.mode_bus_en        = 0;
    conf_reg.mode_continious_en = 0;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetModeContinious(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.mode_continious_en = 1;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetModeTriggered(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.mode_continious_en = 0;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetShuntMeasEnable(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.mode_shunt_en = 1;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetShuntMeasDisable(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.mode_shunt_en = 0;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetBusMeasEnable(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.mode_bus_en = 1;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetBusMeasDisable(i2c_ina3221_t* pHandle)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.mode_bus_en = 0;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetAveragingMode(i2c_ina3221_t* pHandle, ina3221_avg_mode_e mode)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.avg_mode = mode;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetBusConversionTime(i2c_ina3221_t* pHandle, ina3221_conv_time_e convTime)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.bus_conv_time = convTime;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetShuntConversionTime(i2c_ina3221_t* pHandle, ina3221_conv_time_e convTime)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
    conf_reg.shunt_conv_time = convTime;
    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetPwrValidUpLimit(i2c_ina3221_t* pHandle, int16_t voltagemV)
{
    INA3221_WriteWord(pHandle, REG_PWR_VALID_HI_LIM, (uint16_t*)&voltagemV);
}

void INA3221_SetPwrValidLowLimit(i2c_ina3221_t* pHandle, int16_t voltagemV)
{
    INA3221_WriteWord(pHandle, REG_PWR_VALID_LO_LIM, (uint16_t*)&voltagemV);
}

void INA3221_SetShuntSumAlertLimit(i2c_ina3221_t* pHandle, int32_t voltageuV)
{
    int16_t val = voltageuV / 20;
    INA3221_WriteWord(pHandle, REG_SHUNTV_SUM_LIM, (uint16_t*)&val);
}

void INA3221_SetCurrentSumAlertLimit(i2c_ina3221_t* pHandle, int32_t currentmA)
{
    int16_t shuntuV = currentmA * (int32_t)pHandle->u32ShuntRes[INA3221_CH1];
    INA3221_SetShuntSumAlertLimit(pHandle, shuntuV);
}

void INA3221_SetWarnAlertLatchEnable(i2c_ina3221_t* pHandle)
{
    ina3221_masken_reg_t masken_reg;

    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    masken_reg.warn_alert_latch_en = 1;
    INA3221_WriteWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221_SetWarnAlertLatchDisable(i2c_ina3221_t* pHandle)
{
    ina3221_masken_reg_t masken_reg;

    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    masken_reg.warn_alert_latch_en = 1;
    INA3221_WriteWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221_SetCritAlertLatchEnable(i2c_ina3221_t* pHandle)
{
    ina3221_masken_reg_t masken_reg;

    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    masken_reg.crit_alert_latch_en = 1;
    INA3221_WriteWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221_SetCritAlertLatchDisable(i2c_ina3221_t* pHandle)
{
    ina3221_masken_reg_t masken_reg;

    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    masken_reg.crit_alert_latch_en = 1;
    INA3221_WriteWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221_ReadFlags(i2c_ina3221_t* pHandle)
{
    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&_masken_reg);
}

bool INA3221_GetTimingCtrlAlertFlag(i2c_ina3221_t* pHandle)
{
    return _masken_reg.timing_ctrl_alert;
}

bool INA3221_GetPwrValidAlertFlag(i2c_ina3221_t* pHandle)
{
    return _masken_reg.pwr_valid_alert;
}

bool INA3221_GetCurrentSumAlertFlag(i2c_ina3221_t* pHandle)
{
    return _masken_reg.shunt_sum_alert;
}

bool INA3221_GetConversionReadyFlag(i2c_ina3221_t* pHandle)
{
    return _masken_reg.conv_ready;
}

uint16_t INA3221_GetManufID(i2c_ina3221_t* pHandle)
{
    uint16_t id = 0;
    INA3221_ReadWord(pHandle, REG_MANUF_ID, &id);
    return id;
}

uint16_t INA3221_GetDieID(i2c_ina3221_t* pHandle)
{
    uint16_t id = 0;
    INA3221_ReadWord(pHandle, REG_DIE_ID, &id);
    return id;
}

void INA3221_SetChannelEnable(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);

    switch (pHandle, channel)
    {
        case INA3221_CH1:
            conf_reg.ch1_en = 1;
            break;
        case INA3221_CH2:
            conf_reg.ch2_en = 1;
            break;
        case INA3221_CH3:
            conf_reg.ch3_en = 1;
            break;
    }

    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetChannelDisable(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    ina3221_conf_reg_t conf_reg;

    INA3221_ReadWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);

    switch (pHandle, channel)
    {
        case INA3221_CH1:
            conf_reg.ch1_en = 0;
            break;
        case INA3221_CH2:
            conf_reg.ch2_en = 0;
            break;
        case INA3221_CH3:
            conf_reg.ch3_en = 0;
            break;
    }

    INA3221_WriteWord(pHandle, REG_CONF, (uint16_t*)&conf_reg);
}

void INA3221_SetWarnAlertShuntLimit(i2c_ina3221_t* pHandle, ina3221_ch_e channel, int32_t voltageuV)
{
    uint8_t reg;
    int16_t val = 0;

    switch (pHandle, channel)
    {
        case INA3221_CH1:
            reg = REG_CH1_WARNING_ALERT_LIM;
            break;
        case INA3221_CH2:
            reg = REG_CH2_WARNING_ALERT_LIM;
            break;
        case INA3221_CH3:
            reg = REG_CH3_WARNING_ALERT_LIM;
            break;
    }

    val = voltageuV / 5;
    INA3221_WriteWord(pHandle, reg, (uint16_t*)&val);
}

void INA3221_SetCritAlertShuntLimit(i2c_ina3221_t* pHandle, ina3221_ch_e channel, int32_t voltageuV)
{
    uint8_t reg;
    int16_t val = 0;

    switch (pHandle, channel)
    {
        case INA3221_CH1:
            reg = REG_CH1_CRIT_ALERT_LIM;
            break;
        case INA3221_CH2:
            reg = REG_CH2_CRIT_ALERT_LIM;
            break;
        case INA3221_CH3:
            reg = REG_CH3_CRIT_ALERT_LIM;
            break;
    }

    val = voltageuV / 5;
    INA3221_WriteWord(pHandle, reg, (uint16_t*)&val);
}

void INA3221_SetWarnAlertCurrentLimit(i2c_ina3221_t* pHandle, ina3221_ch_e channel, int32_t currentmA)
{
    int32_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)pHandle->u32ShuntRes[channel];
    INA3221_SetWarnAlertShuntLimit(pHandle, channel, shuntuV);
}

void INA3221_SetCritAlertCurrentLimit(i2c_ina3221_t* pHandle, ina3221_ch_e channel, int32_t currentmA)
{
    int32_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)pHandle->u32ShuntRes[channel];
    INA3221_SetCritAlertShuntLimit(pHandle, channel, shuntuV);
}

void INA3221_SetCurrentSumEnable(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    ina3221_masken_reg_t masken_reg;

    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);

    switch (pHandle, channel)
    {
        case INA3221_CH1:
        {
            masken_reg.shunt_sum_en_ch1 = 1;
            break;
        }
        case INA3221_CH2:
        {
            masken_reg.shunt_sum_en_ch2 = 1;
            break;
        }
        case INA3221_CH3:
        {
            masken_reg.shunt_sum_en_ch3 = 1;
            break;
        }
    }

    INA3221_WriteWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221_SetCurrentSumDisable(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    ina3221_masken_reg_t masken_reg;

    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);

    switch (pHandle, channel)
    {
        case INA3221_CH1:
        {
            masken_reg.shunt_sum_en_ch1 = 0;
            break;
        }
        case INA3221_CH2:
        {
            masken_reg.shunt_sum_en_ch2 = 0;
            break;
        }
        case INA3221_CH3:
        {
            masken_reg.shunt_sum_en_ch3 = 0;
            break;
        }
    }

    INA3221_WriteWord(pHandle, REG_MASK_ENABLE, (uint16_t*)&masken_reg);
    _masken_reg = masken_reg;
}

int32_t INA3221_GetShuntVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    int32_t  res;
    uint8_t  reg;
    uint16_t val_raw = 0;

    switch (pHandle, channel)
    {
        case INA3221_CH1:
        {
            reg = REG_CH1_SHUNTV;
            break;
        }
        case INA3221_CH2:
        {
            reg = REG_CH2_SHUNTV;
            break;
        }
        case INA3221_CH3:
        {
            reg = REG_CH3_SHUNTV;
            break;
        }
    }

    INA3221_ReadWord(pHandle, reg, &val_raw);

    res = (int16_t)val_raw;
    res *= 5;  // 1 LSB = 5uV

    return res;
}

bool INA3221_GetWarnAlertFlag(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    switch (pHandle, channel)
    {
        case INA3221_CH1:
        {
            return _masken_reg.warn_alert_ch1;
        }
        case INA3221_CH2:
        {
            return _masken_reg.warn_alert_ch2;
        }
        case INA3221_CH3:
        {
            return _masken_reg.warn_alert_ch3;
        }
        default:
        {
            return -1;
        }
    }
}

bool INA3221_GetCritAlertFlag(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    switch (pHandle, channel)
    {
        case INA3221_CH1:
        {
            return _masken_reg.crit_alert_ch1;
        }
        case INA3221_CH2:
        {
            return _masken_reg.crit_alert_ch2;
        }
        case INA3221_CH3:
        {
            return _masken_reg.crit_alert_ch3;
        }
        default:
        {
            return -1;
        }
    }
}

int32_t INA3221_EstimateOffsetVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e channel, uint32_t busV)
{
    float32_t bias_in    = 10.0;                                    // Input bias current at IN– in uA
    float32_t r_in       = 0.670;                                   // Input resistance at IN– in MOhm
    uint32_t  adc_step   = 40;                                      // smallest shunt ADC step in uV
    float32_t shunt_res  = pHandle->u32ShuntRes[channel] / 1000.0;  // convert to Ohm
    float32_t filter_res = pHandle->u32FilterRes[channel];
    int32_t   offset     = 0.0;
    float32_t reminder;

    offset = (shunt_res + filter_res) * (busV / r_in + bias_in) - bias_in * filter_res;

    // Round the offset to the closest shunt ADC value
    reminder = offset % adc_step;
    if (reminder < adc_step / 2)
    {
        offset -= reminder;
    }
    else
    {
        offset += adc_step - reminder;
    }

    return offset;
}

float32_t INA3221_GetCurrent(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    int32_t   shunt_uV  = 0;
    float32_t current_A = 0;

    shunt_uV  = INA3221_GetShuntVoltage(pHandle, channel);
    current_A = shunt_uV / (int32_t)pHandle->u32ShuntRes[channel] / 1000.0;
    return current_A;
}

float32_t INA3221_GetCurrentCompensated(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    int32_t   shunt_uV  = 0;
    int32_t   bus_V     = 0;
    float32_t current_A = 0.0;
    int32_t   offset_uV = 0;

    shunt_uV  = INA3221_GetShuntVoltage(pHandle, channel);
    bus_V     = INA3221_GetVoltage(pHandle, channel);
    offset_uV = INA3221_EstimateOffsetVoltage(pHandle, channel, bus_V);

    current_A = (shunt_uV - offset_uV) / (int32_t)pHandle->u32ShuntRes[channel] / 1000.0;

    return current_A;
}

float32_t INA3221_GetVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e channel)
{
    float32_t voltage_V = 0.0;
    uint8_t   reg;
    uint16_t  val_raw = 0;

    switch (pHandle, channel)
    {
        case INA3221_CH1:
        {
            reg = REG_CH1_BUSV;
            break;
        }
        case INA3221_CH2:
        {
            reg = REG_CH2_BUSV;
            break;
        }
        case INA3221_CH3:
        {
            reg = REG_CH3_BUSV;
            break;
        }
    }

    INA3221_ReadWord(pHandle, reg, &val_raw);

    voltage_V = val_raw / 1000.0;

    return voltage_V;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void INA3221_Test(i2c_mst_t* hI2C)
{
    i2c_ina3221_t ina3221 = {
        .hI2C      = hI2C,
        .u8SlvAddr = INA3221_ADDRESS_GND,
    };

    INA3221_Init(&ina3221);
    INA3221_Reset(&ina3221);

    // Set shunt resistors to 10 mOhm for all channels
    INA3221_SetShuntRes(&ina3221, 10, 10, 10);

    // Set series filter resistors to 10 Ohm for all channels.
    // Series filter resistors introduce error to the current measurement.
    // The error can be estimated and depends on the resitor values and the bus voltage.
    // INA3221_SetFilterRes(&ina3221, 10, 10, 10);

    while (1)
    {
        float32_t af32Current[3];
        float32_t af32CurrentCompensated[3];
        float32_t af32Voltage[3];

        af32Current[0]            = INA3221_GetCurrent(&ina3221, INA3221_CH1);
        af32CurrentCompensated[0] = INA3221_GetCurrentCompensated(&ina3221, INA3221_CH1);
        af32Voltage[0]            = INA3221_GetVoltage(&ina3221, INA3221_CH1);

        af32Current[1]            = INA3221_GetCurrent(&ina3221, INA3221_CH2);
        af32CurrentCompensated[1] = INA3221_GetCurrentCompensated(&ina3221, INA3221_CH2);
        af32Voltage[1]            = INA3221_GetVoltage(&ina3221, INA3221_CH2);

        af32Current[2]            = INA3221_GetCurrent(&ina3221, INA3221_CH3);
        af32CurrentCompensated[2] = INA3221_GetCurrentCompensated(&ina3221, INA3221_CH3);
        af32Voltage[2]            = INA3221_GetVoltage(&ina3221, INA3221_CH3);
			
        LOGI("Channel 1:");
        LOGI(" - Current: %.3f A", af32Current[0]);
        LOGI(" - Compensated current: %.3f", af32CurrentCompensated[0]);
        LOGI(" - Voltage: %.3f V", af32Voltage[0]);

        LOGI("Channel 2:");
        LOGI(" - Current: %.3f A", af32Current[1]);
        LOGI(" - Compensated current: %.3f", af32CurrentCompensated[1]);
        LOGI(" - Voltage: %.3f V", af32Voltage[1]);

        LOGI("Channel 3:");
        LOGI(" - Current: %.3f A", af32Current[2]);
        LOGI(" - Compensated current: %.3f", af32CurrentCompensated[2]);
        LOGI(" - Voltage: %.3f V", af32Voltage[2]);
				
				LOGI("-------------------------------------");
				
        DelayBlockMs(1000);
    }
}

#endif
