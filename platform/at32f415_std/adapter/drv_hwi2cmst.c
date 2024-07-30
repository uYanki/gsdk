#include "i2cdefs.h"

#if CONFIG_HWI2C_MODULE_SW || 1

#include "i2c_application.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_i2cmst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#define I2C_TIMEOUT     0xFF

#define MakeError_(eStatus)                     \
    do {                                        \
        switch (eStatus)                        \
        {                                       \
            case I2C_OK: return ERR_NONE;       \
            case I2C_ERR_BUSY: return ERR_BUSY; \
            default: return ERR_TIMEOUT;        \
        }                                       \
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
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);

    if (hwi2c == I2C1)
    {
        crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
    }
    else if (hwi2c == I2C2)
    {
        crm_periph_clock_enable(CRM_I2C2_PERIPH_CLOCK, TRUE);
    }
    else
    {
        return ERR_INVALID_VALUE;  // unknown instance
    }

    if (pHandle->SDA.Port == GPIOA || pHandle->SCL.Port == GPIOA)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    }
    else if (pHandle->SDA.Port == GPIOB || pHandle->SCL.Port == GPIOB)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    }

    gpio_init_type gpio_initstructure = {
        .gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN,
        .gpio_pull           = GPIO_PULL_UP,
        .gpio_mode           = GPIO_MODE_MUX,
        .gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE,
    };

    gpio_initstructure.gpio_pins = pHandle->SDA.Pin,
    gpio_init(pHandle->SDA.Port, &gpio_initstructure);

    gpio_initstructure.gpio_pins = pHandle->SCL.Pin,
    gpio_init(pHandle->SCL.Port, &gpio_initstructure);

    if (u32ClockFreqHz == 0)
    {
        u32ClockFreqHz = 100000;  // 100k
    }

    i2c_reset(hwi2c);
    i2c_init(hwi2c, I2C_FSMODE_DUTY_2_1, u32ClockFreqHz);
    i2c_own_address1_set(hwi2c, I2C_ADDRESS_MODE_7BIT, 0xA0);
    i2c_enable(hwi2c, TRUE);

    return ERR_NONE;
}

static inline bool HWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags)
{
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);

    /* wait for the busy flag to be reset */
    if (i2c_wait_flag(hwi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, I2C_TIMEOUT) != I2C_OK)
    {
        return false;
    }

    /* ack acts on the current byte */
    i2c_master_receive_ack_set(hwi2c, I2C_MASTER_ACK_CURRENT);

    /* send slave address */
    if (i2c_master_write_addr(hwi2c, u16SlvAddr << 1, I2C_TIMEOUT) != I2C_OK)
    {
        /* generate stop condtion */
        i2c_stop_generate(hwi2c);

        return false;
    }

    /* clear addr flag */
    i2c_flag_clear(hwi2c, I2C_ADDR7F_FLAG);

    /* generate stop condtion */
    i2c_stop_generate(hwi2c);

    return true;
}

static inline err_t HWI2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type*                  hwi2c        = (i2c_type*)(pHandle->I2Cx);
    i2c_mem_address_width_type eMemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_MEMADDR_16BIT) ? I2C_MEM_ADDR_WIDIH_16 : I2C_MEM_ADDR_WIDIH_8;
    MakeError_(i2c_memory_read(hwi2c, eMemAddrSize, u16SlvAddr << 1, u16MemAddr, pu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type*                  hwi2c        = (i2c_type*)(pHandle->I2Cx);
    i2c_mem_address_width_type eMemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_MEMADDR_16BIT) ? I2C_MEM_ADDR_WIDIH_16 : I2C_MEM_ADDR_WIDIH_8;
    MakeError_(i2c_memory_write(hwi2c, eMemAddrSize, u16SlvAddr << 1, u16MemAddr, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);
    MakeError_(i2c_master_receive(hwi2c, u16SlvAddr << 1, pu8Data, u16Size, I2C_TIMEOUT));
}

static inline err_t HWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);
    MakeError_(i2c_master_transmit(hwi2c, u16SlvAddr << 1, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

#endif
