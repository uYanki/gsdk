#include "spi_ad770x.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "ad770x"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline err_t AD770x_WriteByte(spi_ad770x_t* pHandle, uint8_t u8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    ERROR_CHECK_RETURN(SPI_Master_TransmitByte(pHandle->hSPI, u8Data));
    SPI_Master_Deselect(pHandle->hSPI);
    return ERR_NONE;
}

static inline err_t AD770x_ReadByte(spi_ad770x_t* pHandle, uint8_t* pu8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    ERROR_CHECK_RETURN(SPI_Master_ReceiveByte(pHandle->hSPI, pu8Data));
    SPI_Master_Deselect(pHandle->hSPI);
    return ERR_NONE;
}

static inline err_t AD770x_ReadBlock(spi_ad770x_t* pHandle, uint8_t* pu8Data, uint8_t u8Size)
{
    SPI_Master_Select(pHandle->hSPI);
    ERROR_CHECK_RETURN(SPI_Master_ReceiveBlock(pHandle->hSPI, pu8Data, u8Size));
    SPI_Master_Deselect(pHandle->hSPI);
    return ERR_NONE;
}

//---------------------------------------------------------------------------

static inline err_t AD770x_Reset(spi_ad770x_t* pHandle)
{
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_LOW);
    DelayBlockMs(20);
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_HIGH);
    DelayBlockMs(20);

    return ERR_NONE;
}

/**
 * @brief 同步 spi 接口，以防模块失步
 */
static inline err_t AD770x_SyncSpi(spi_ad770x_t* pHandle)
{
    uint8_t u8Count = 100, u8DummyByte = 0xff;

    SPI_Master_Select(pHandle->hSPI);

    while (u8Count--)
    {
        ERROR_CHECK_RETURN(AD770x_WriteByte(pHandle, u8DummyByte));
    }

    SPI_Master_Deselect(pHandle->hSPI);

    // 复位后等待 500us 再访问
    DelayBlockMs(1);

    return ERR_NONE;
}

static inline err_t AD770x_SetOperation(
    spi_ad770x_t*      pHandle,
    ad770x_channel_e   eChannel,
    ad770x_register_e  eRegister,
    ad770x_operation_e eOperation)
{
    return AD770x_WriteByte(pHandle, ((uint8_t)eRegister | (uint8_t)eOperation | (uint8_t)eChannel) & 0x7F);
}

err_t AD770x_Init(spi_ad770x_t* pHandle)
{
    ERROR_CHECK_RETURN(AD770x_Reset(pHandle));
    ERROR_CHECK_RETURN(AD770x_SyncSpi(pHandle));
    return ERR_NONE;
}

err_t AD770x_ConfigChannel(
    spi_ad770x_t*        pHandle,
    ad770x_channel_e     eChannel,
    ad770x_mode_e        eMode,
    ad770x_clock_e       eClock,
    ad770x_gain_e        eGain,
    ad770x_polarity_e    ePolarity,
    ad770x_update_rate_e eRate)
{
    ERROR_CHECK_RETURN(AD770x_SetOperation(pHandle, eChannel, AD770X_REG_CLOCK, AD770X_WRITE));
    ERROR_CHECK_RETURN(AD770x_WriteByte(pHandle, ((uint8_t)eClock | (uint8_t)eRate) & 0x1F));

    ERROR_CHECK_RETURN(AD770x_SetOperation(pHandle, eChannel, AD770X_REG_SETUP, AD770X_WRITE));

    // 包含禁用 内部缓冲器 和 过滤器同步
    ERROR_CHECK_RETURN(AD770x_WriteByte(pHandle, (uint8_t)eMode | (uint8_t)eGain | (uint8_t)ePolarity | (0 << 1) | (0 << 0)));

    return ERR_NONE;
}

err_t AD770x_IsDataReady(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, bool* pbReady)
{
    uint8_t u8State;

    ERROR_CHECK_RETURN(AD770x_SetOperation(pHandle, eChannel, AD770X_REG_CMM, AD770X_READ));
    ERROR_CHECK_RETURN(AD770x_ReadByte(pHandle, &u8State));

    *pbReady = (u8State & 0x80) ? true : false;

    return ERR_NONE;
}

/**
 * @brief 读取满标度寄存器
 * @note  数据是自动采集? 还是在发送读 REG_DATA 才开始采集的？
 *
 */
err_t AD770x_ReadAdc(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint16_t* pu16Value)
{
    uint8_t au8Buff[2];
    bool    bIsReady;

    ERROR_CHECK_RETURN(AD770x_SetOperation(pHandle, eChannel, AD770X_REG_DATA, AD770X_READ));

    do {  // 发了立即读，还是轮询状态位 ???
        ERROR_CHECK_RETURN(AD770x_IsDataReady(pHandle, eChannel, &bIsReady));
    } while (bIsReady == false);

    ERROR_CHECK_RETURN(AD770x_ReadBlock(pHandle, au8Buff, 2));

    *pu16Value = (au8Buff[0] << 8) | au8Buff[1];

    return ERR_NONE;
}

/**
 * @brief 读取零标度寄存器
 */
err_t AD770x_ReadZeroCal(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint32_t* pu32Value)
{
    uint8_t au8Buff[3];
    ERROR_CHECK_RETURN(AD770x_SetOperation(pHandle, eChannel, AD770X_REG_OFFSET, AD770X_READ));
    ERROR_CHECK_RETURN(AD770x_ReadBlock(pHandle, au8Buff, 3));
    *pu32Value = (au8Buff[2] << 16) | (au8Buff[1] << 8) | au8Buff[0];
    return ERR_NONE;
}

/**
 * @brief 读取满标度寄存器
 */
err_t AD770x_ReadFullCal(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint32_t* pu32Value)
{
    uint8_t au8Buff[3];
    ERROR_CHECK_RETURN(AD770x_SetOperation(pHandle, eChannel, AD770X_REG_GAIN, AD770X_READ));
    ERROR_CHECK_RETURN(AD770x_ReadBlock(pHandle, au8Buff, 3));
    *pu32Value = (au8Buff[2] << 16) | (au8Buff[1] << 8) | au8Buff[0];
    return ERR_NONE;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#define CONFIG_AD770X_VREF        2.5f

void AD7705_Test(void)
{
#if defined(BOARD_STM32F407VET6_XWS)

    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5},
        .MOSI = {GPIOA, GPIO_PIN_4},
        .SCLK = {GPIOA, GPIO_PIN_6},
        .CS   = {GPIOA, GPIO_PIN_3},
    };

    spi_ad770x_t ad770x = {
        .hSPI = &spi,
        .RST  = {GPIOA, GPIO_PIN_0},
    };

#endif

    SPI_Master_Init(&spi, 10000, SPI_DUTYCYCLE_50_50, AD770X_SPI_TIMING | SPI_FLAG_SOFT_CS);

    AD770x_Init(&ad770x);

    AD770x_ConfigChannel(
        &ad770x,
        AD7705_CHANNEL_AIN1P_AIN1N,
        AD770X_MODE_CALIBRATION_SELF,
        AD770X_CLOCK_4_9152_MHZ,
        AD770X_GAIN_2,
        AD770X_POLARITY_BIPOLAR,
        AD770X_UPDATE_RATE_50_HZ);
		
    AD770x_ConfigChannel(
        &ad770x,
        AD7705_CHANNEL_AIN2P_AIN2N,
        AD770X_MODE_CALIBRATION_SELF,
        AD770X_CLOCK_4_9152_MHZ,
        AD770X_GAIN_8,
        AD770X_POLARITY_BIPOLAR,
        AD770X_UPDATE_RATE_60_HZ);

    while (1)
    {
        uint16_t  u16ChannelData[2] = {0};
        float32_t f32ChannelData[2] = {0.f};

        AD770x_ReadAdc(&ad770x, AD7705_CHANNEL_AIN1P_AIN1N, &u16ChannelData[0]);
        AD770x_ReadAdc(&ad770x, AD7705_CHANNEL_AIN2P_AIN2N, &u16ChannelData[1]);
				
        f32ChannelData[0] = u16ChannelData[0] * CONFIG_AD770X_VREF / 65535.f;
        f32ChannelData[1] = u16ChannelData[1] * CONFIG_AD770X_VREF / 65535.f;

        PRINTLN("%.2f,%.2f", f32ChannelData[0], f32ChannelData[1]);
    }
}

#endif
