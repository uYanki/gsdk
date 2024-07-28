#include "i2cdefs.h"

#if CONFIG_HWI2C_MODULE_SW

#include "i2c.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_i2cmst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

#define I2C_TIMEOUT     0xFF

#define MakeError_(eStatus)                       \
    do {                                          \
        switch (eStatus)                          \
        {                                         \
            case HAL_OK: return ERR_NONE;         \
            case HAL_BUSY: return ERR_BUSY;       \
            case HAL_TIMEOUT: return ERR_TIMEOUT; \
            default: return ERR_FAIL;             \
        }                                         \
    } while (0)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static err_t HWI2C_Master_Init(i2c_mst_t* pHandle, uint32_t u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle);
static bool  HWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags);
static err_t HWI2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags);
static err_t HWI2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags);
static err_t HWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags);
static err_t HWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const i2cmst_ops_t g_hwI2cOps = {
    .Init          = HWI2C_Master_Init,
    .IsDeviceReady = HWI2C_Master_IsDeviceReady,
    .ReadBlock     = HWI2C_Master_ReadBlock,
    .WriteBlock    = HWI2C_Master_WriteBlock,
    .ReceiveBlock  = HWI2C_Master_ReceiveBlock,
    .TransmitBlock = HWI2C_Master_TransmitBlock,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline err_t HWI2C_Master_Init(i2c_mst_t* pHandle, uint32_t u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle)
{
    I2C_HandleTypeDef* hwi2c = (I2C_HandleTypeDef*)(pHandle->I2Cx);

    if (
#ifdef I2C1
        hwi2c->Instance == I2C1 ||
#endif
#ifdef I2C2
        hwi2c->Instance == I2C2 ||
#endif
#ifdef I2C3
        hwi2c->Instance == I2C3 ||
#endif
#ifdef I2C4
        hwi2c->Instance == I2C4 ||
#endif
#ifdef I2C5
        hwi2c->Instance == I2C5 ||
#endif
        0)
    {
        return ERR_NONE;
    }

    // invalid instance
    return ERR_INVALID_VALUE;  // unknown instance
}

static inline bool HWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags)
{
    return HAL_I2C_IsDeviceReady(pHandle->I2Cx, u16SlvAddr << 1, 5, I2C_TIMEOUT) == HAL_OK;
}

static inline err_t HWI2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    uint16_t u16MemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_16BIT_MEMADDR) ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT;
    MakeError_(HAL_I2C_Mem_Read(pHandle->I2Cx, u16SlvAddr << 1, u16MemAddr, u16MemAddrSize, pu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    uint16_t u16MemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_16BIT_MEMADDR) ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT;
    MakeError_(HAL_I2C_Mem_Write(pHandle->I2Cx, u16SlvAddr << 1, u16MemAddr, u16MemAddrSize, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    MakeError_(HAL_I2C_Master_Transmit(pHandle->I2Cx, u16SlvAddr << 1, (uint8_t*)pu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    MakeError_(HAL_I2C_Master_Transmit(pHandle->I2Cx, u16SlvAddr << 1, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

#endif
