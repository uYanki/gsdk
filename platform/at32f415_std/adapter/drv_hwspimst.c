#include "spimst.h"

/**
 * @test 全双工收发(有bug)，半双工接收 待测试
 */

#if CONFIG_HWSPI_MODULE_SW

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_hwspi"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static err_t HWSPI_Master_Init(spi_mst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags);
static err_t HWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size);
static err_t HWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size);
static err_t HWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const spimst_ops_t g_hwSpiOps = {
    .Init                 = HWSPI_Master_Init,
    .TransmitBlock        = HWSPI_Master_TransmitBlock,
    .ReceiveBlock         = HWSPI_Master_ReceiveBlock,
    .TransmitReceiveBlock = HWSPI_Master_TransmitReceiveBlock,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

static err_t HWSPI_Master_Init(spi_mst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags)
{
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    // 暂不支持 GPIO REMAP !!!

    if (hwspi == SPI1)
    {
        /* SPI1: PA4/CS, PA5/SCLK, PA6/MISO, PA7/MOSI */
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
        crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
    }
    else if (hwspi == SPI2)
    {
        /* SPI2: PB12/CS, PB13/SCLK, PB14/MISO, PB15/MOSI */
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
    }
    else
    {
        return MakeError(ERR_INVALID_VALUE, "unknown instance");
    }

    gpio_init_type gpio_init_struct = {
        .gpio_out_type       = GPIO_OUTPUT_PUSH_PULL,
        .gpio_pull           = GPIO_PULL_UP,
        .gpio_mode           = GPIO_MODE_MUX,
        .gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER,
    };

    spi_init_type spi_init_struct = {0};

    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;

    if (u32ClockSpeedHz >= 50000000)
    {
        spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_2;  // 50M
    }
    else if (u32ClockSpeedHz >= 25000000)
    {
        spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_4;  // 25M
    }
    else
    {
        spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;  // 12.5M
    }

    switch (u16Flags & SPI_FLAG_FIRSTBIT_Msk)
    {
        case SPI_FLAG_LSBFIRST: spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_LSB; break;
        case SPI_FLAG_MSBFIRST: spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB; break;
    }

    switch (u16Flags & SPI_FLAG_CPOL_Msk)
    {
        case SPI_FLAG_CPOL_LOW: spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW; break;
        case SPI_FLAG_CPOL_HIGH: spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH; break;
    }

    switch (u16Flags & SPI_FLAG_CPHA_Msk)
    {
        case SPI_FLAG_CPHA_1EDGE: spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE; break;
        case SPI_FLAG_CPHA_2EDGE: spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE; break;
    }

    switch (u16Flags & SPI_FLAG_DATAWIDTH_Msk)
    {
        case SPI_FLAG_DATAWIDTH_8B: spi_init_struct.frame_bit_num = SPI_FRAME_8BIT; break;
        case SPI_FLAG_DATAWIDTH_16B: spi_init_struct.frame_bit_num = SPI_FRAME_16BIT; break;
        case SPI_FLAG_DATAWIDTH_32B: return MakeError(ERR_NOT_SUPPORTED, "unsupported datawidth");
    }

    switch (u16Flags & SPI_FLAG_CS_MODE_Msk)
    {
        case SPI_FLAG_NONE_CS:
        case SPI_FLAG_SOFT_CS:
        {
            spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
            break;
        }
        case SPI_FLAG_HADR_CS:
        {
            spi_init_struct.cs_mode_selection = SPI_CS_HARDWARE_MODE;

            gpio_init_struct.gpio_pins = pHandle->CS.Pin;
            gpio_init(pHandle->CS.Pin, &gpio_init_struct);

            break;
        }
    }

    switch (u16Flags & SPI_FLAG_WIRE_Msk)
    {
        case SPI_FLAG_3WIRE:
        {
						gpio_init_struct.gpio_pins = pHandle->MOSI.Pin;
						gpio_init(pHandle->MOSI.Pin, &gpio_init_struct);

						gpio_init_struct.gpio_pins = pHandle->MISO.Pin;
						gpio_init(pHandle->MISO.Port, &gpio_init_struct);

            spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;

            break;
        }

        case SPI_FLAG_4WIRE:
        {
            gpio_init_struct.gpio_pins = pHandle->MOSI.Pin;
            gpio_init(pHandle->MOSI.Pin, &gpio_init_struct);

            gpio_init_struct.gpio_pins = pHandle->MISO.Pin;
            gpio_init(pHandle->MISO.Port, &gpio_init_struct);
            spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;

            break;
        }
    }

    gpio_init_struct.gpio_pins = pHandle->SCLK.Pin;
    gpio_init(pHandle->SCLK.Port, &gpio_init_struct);

    spi_init(hwspi, &spi_init_struct);
    spi_enable(hwspi, TRUE);

    return ERR_NONE;
}

static err_t HWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size)
{
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    if (CHKMSK16(pHandle->u16TimingConfig, SPI_FLAG_WIRE_Msk, SPI_FLAG_3WIRE))
    {
        spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_TX);
    }

    while (u16Size--)
    {
        while (spi_i2s_flag_get(hwspi, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(hwspi, *cpu8TxData++);
    }

    return ERR_NONE;
}

static err_t HWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size)
{
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    if (CHKMSK16(pHandle->u16TimingConfig, SPI_FLAG_WIRE_Msk, SPI_FLAG_3WIRE))
    {
        spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_RX);
    }

    while (u16Size--)
    {
        while (spi_i2s_flag_get(hwspi, SPI_I2S_RDBF_FLAG) == RESET);
        *pu8RxData++ = spi_i2s_data_receive(hwspi);
    }

    return ERR_NONE;
}

static err_t HWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size)
{
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    if (CHKMSK16(pHandle->u16TimingConfig, SPI_FLAG_WIRE_Msk, SPI_FLAG_3WIRE))
    {
        while (u16Size--)
        {
            spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_TX);
            spi_i2s_data_transmit(hwspi, *cpu8TxData++);
            while (spi_i2s_flag_get(hwspi, SPI_I2S_TDBE_FLAG) == RESET);

            spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_RX);
            while (spi_i2s_flag_get(hwspi, SPI_I2S_RDBF_FLAG) == RESET);
            *pu8RxData++ = spi_i2s_data_receive(hwspi);
        }
    }
    else
    {
        while (u16Size--)
        {
            spi_i2s_data_transmit(hwspi, *cpu8TxData++);
            while (spi_i2s_flag_get(hwspi, SPI_I2S_TDBE_FLAG) == RESET);

            while (spi_i2s_flag_get(hwspi, SPI_I2S_RDBF_FLAG) == RESET);
            *pu8RxData++ = spi_i2s_data_receive(hwspi);
        }
    }

    return ERR_NONE;
}

#endif
