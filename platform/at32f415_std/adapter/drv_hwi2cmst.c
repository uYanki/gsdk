#include "i2cdefs.h"

#if CONFIG_HWI2C_MODULE_SW || 1

#include "i2c_application.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_i2cmst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

#define I2C_TIMEOUT     0xFFFF

#define ThrowError_(eStatus)                                          \
    do {                                                              \
        switch (eStatus)                                              \
        {                                                             \
            case I2C_OK: return ERR_NONE;                             \
            case I2C_ERR_STEP_1: return ThrowError(ERR_BUSY, "busy"); \
            default: return ThrowError(ERR_TIMEOUT, "timeout");       \
        }                                                             \
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
    i2c_handle_type* hwi2c = (i2c_handle_type*)(pHandle->I2Cx);

    if (
#ifdef I2C1
        hwi2c->i2cx == I2C1 ||
#endif
#ifdef I2C2
        hwi2c->i2cx == I2C2 ||
#endif
#ifdef I2C3
        hwi2c->i2cx == I2C3 ||
#endif
#ifdef I2C4
        hwi2c->i2cx == I2C4 ||
#endif
#ifdef I2C5
        hwi2c->i2cx == I2C5 ||
#endif
        0)
    {
        return ERR_NONE;
    }

    // invalid instance
    return ThrowError(ERR_INVALID_VALUE, "unknown instance");
}

static inline bool HWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags)
{
    i2c_handle_type* hi2c = (i2c_handle_type*)(pHandle->I2Cx);

    hi2c->error_code = I2C_OK;

    /* wait for the busy flag to be reset */
    if (i2c_wait_flag(hi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, I2C_TIMEOUT) != I2C_OK)
    {
        return false;
    }

    /* ack acts on the current byte */
    i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_CURRENT);

    /* send slave address */
    if (i2c_master_write_addr(hi2c, u16SlvAddr << 1, I2C_TIMEOUT) != I2C_OK)
    {
        /* generate stop condtion */
        i2c_stop_generate(hi2c->i2cx);

        return false;
    }

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return true;
}

static inline err_t HWI2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_handle_type* hwi2c = (i2c_handle_type*)(pHandle->I2Cx);

    ThrowError_(i2c_memory_read(hwi2c,CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_16BIT_MEMADDR)DR) ? I2C_MEM_ADDR_WIDIH_16 : I2C_MEM_ADDR_WIDIH_8,
                                u16SlvAddr << 1, u16MemAddr, pu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_handle_type* hwi2c = (i2c_handle_type*)(pHandle->I2Cx);

    ThrowError_(i2c_memory_write(hwi2c,CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_16BIT_MEMADDR)DR) ? I2C_MEM_ADDR_WIDIH_16 : I2C_MEM_ADDR_WIDIH_8,
                                 u16SlvAddr << 1, u16MemAddr, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_handle_type* hwi2c = (i2c_handle_type*)(pHandle->I2Cx);

    ThrowError_(i2c_master_receive(hwi2c, u16SlvAddr << 1, (uint8_t*)pu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_handle_type* hwi2c = (i2c_handle_type*)(pHandle->I2Cx);

    ThrowError_(i2c_master_transmit(hwi2c, u16SlvAddr << 1, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

#endif
