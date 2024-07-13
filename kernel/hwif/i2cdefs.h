#ifndef __I2C_DEFS_H__
#define __I2C_DEFS_H__

#include "gsdk.h"
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief slave address size
 */
#define I2C_FLAG_SLVADDR_SIZE_Pos (0U)
#define I2C_FLAG_SLVADDR_SIZE_Len (1U)
#define I2C_FLAG_SLVADDR_SIZE_Msk BITMASK16(I2C_FLAG_SLVADDR_SIZE_Pos, I2C_FLAG_SLVADDR_SIZE_Len)
#define I2C_FLAG_7BIT_SLVADDR     (0U << I2C_FLAG_SLVADDR_SIZE_Pos)
#define I2C_FLAG_10BIT_SLVADDR    (1U << I2C_FLAG_SLVADDR_SIZE_Pos)

/**
 * @brief memory address size
 */
#define I2C_FLAG_MEMADDR_SIZE_Pos (I2C_FLAG_SLVADDR_SIZE_Pos + I2C_FLAG_SLVADDR_SIZE_Len)
#define I2C_FLAG_MEMADDR_SIZE_Len (1U)
#define I2C_FLAG_MEMADDR_SIZE_Msk BITMASK16(I2C_FLAG_MEMADDR_SIZE_Pos, I2C_FLAG_MEMADDR_SIZE_Len)
#define I2C_FLAG_8BIT_MEMADDR     (0U << I2C_FLAG_MEMADDR_SIZE_Pos)
#define I2C_FLAG_16BIT_MEMADDR    (1U << I2C_FLAG_MEMADDR_SIZE_Pos)

/**
 * @brief signal control
 */
#define I2C_FLAG_SIGNAL_Pos         (I2C_FLAG_MEMADDR_SIZE_Pos + I2C_FLAG_MEMADDR_SIZE_Len)
#define I2C_FLAG_SIGNAL_Len         (4U)
#define I2C_FLAG_SIGNAL_Msk         BITMASK16(I2C_FLAG_SIGNAL_Pos, I2C_FLAG_SIGNAL_Len)
#define I2C_FLAG_SIGNAL_DEFAULT     (0U << I2C_FLAG_SIGNAL_Pos)    /*!< Transfer starts with a start signal, stops with a stop signal. */
#define I2C_FLAG_SIGNAL_NOSTART     (BV(0) << I2C_FLAG_SIGNAL_Pos) /*!< Don't send a start condition, address, and sub address */
#define I2C_FLAG_SIGNAL_NORESTART   (BV(1) << I2C_FLAG_SIGNAL_Pos) /*!< Don't send a repeated start condition */
#define I2C_FLAG_SIGNAL_NOSTOP      (BV(2) << I2C_FLAG_SIGNAL_Pos) /*!< Don't send a stop condition. */
#define I2C_FLAG_SIGNAL_IGNORE_NACK (BV(3) << I2C_FLAG_SIGNAL_Pos)

/**
 * @brief word endian
 */
#define I2C_FLAG_WORD_ENDIAN_Pos    (I2C_FLAG_SIGNAL_Pos + I2C_FLAG_SIGNAL_Len)
#define I2C_FLAG_WORD_ENDIAN_Len    (1U)
#define I2C_FLAG_WORD_ENDIAN_Msk    BITMASK16(I2C_FLAG_WORD_ENDIAN_Pos, I2C_FLAG_WORD_ENDIAN_Len)
#define I2C_FLAG_WORD_BIG_ENDIAN    (0 << I2C_FLAG_WORD_ENDIAN_Pos) /*!< low byte at first */
#define I2C_FLAG_WORD_LITTLE_ENDIAN (1 << I2C_FLAG_WORD_ENDIAN_Pos) /*!< high byte at first */

/**
 * @brief i2c duty cycle
 */

typedef enum {
    I2C_DUTYCYCLE_33_67 = 84,   // 255*0.33
    I2C_DUTYCYCLE_50_50 = 127,  // 255*0.5
} i2c_duty_cycle_e;

/**
 * @brief i2c master
 */

typedef struct i2cmst_ops i2cmst_ops_t;

typedef struct {
    __IN const pin_t SDA;   // serial data line
    __IN const pin_t SCL;   // serial clock line
    __IN const void* I2Cx;  // hwi2c handler

#if (CONFIG_SWI2C_MODULE_SW)
    uint32_t u32ClockLowCycleUs;   // [swi2c only]
    uint32_t u32ClockHighCycleUs;  // [swi2c only]
#endif

    const i2cmst_ops_t* pOps;
} i2cmst_t;

struct i2cmst_ops {
    err_t (*Init)(i2cmst_t* pHandle, uint32_t u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle);
    bool (*IsDeviceReady)(i2cmst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags);
    err_t (*ReadBlock)(i2cmst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags);
    err_t (*WriteBlock)(i2cmst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags);
    err_t (*ReceiveBlock)(i2cmst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags);
    err_t (*TransmitBlock)(i2cmst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags);
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
