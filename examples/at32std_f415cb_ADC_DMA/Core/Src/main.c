#include "gsdk.h"
#include "hwif.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "main"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

extern void DbgUartInit(uint32_t u32Baudrate);

static void ADC_Init(void);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static __IO uint16_t m_au16AdcConv[3] = {0};
static bool          m_bAdcSampSts    = false;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

int main(void)
{
    system_clock_config();

    cm_backtrace_init("demo", "at32", "V0.01");
    SEGGER_RTT_Init();

    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);

    DbgUartInit(115200);
    DelayInit();

    ADC_Init();

    while (1)
    {
        PeriodicTask(10 * UNIT_MS, adc_ordinary_software_trigger_enable(ADC1, TRUE));

        if (m_bAdcSampSts == true)
        {
            m_bAdcSampSts = false;

            GSDK_PRINTLN("%.5f,%.5f,%.5f",
                         3.3f * m_au16AdcConv[0] / 4096,
                         3.3f * m_au16AdcConv[1] / 4096,
                         3.3f * m_au16AdcConv[2] / 4096);
        }
    }
}

static void ADC_Init(void)
{
    {  // gpio
        gpio_init_type gpio_initstructure;
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

        gpio_default_para_init(&gpio_initstructure);
        gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
        gpio_initstructure.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5 | GPIO_PINS_6;
        gpio_init(GPIOA, &gpio_initstructure);
    }

    {  // dma
        dma_init_type dma_init_struct;
        crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
        dma_reset(DMA1_CHANNEL1);
        dma_default_para_init(&dma_init_struct);
        dma_init_struct.buffer_size           = 3;
        dma_init_struct.direction             = DMA_DIR_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_base_addr      = (uint32_t)m_au16AdcConv;
        dma_init_struct.memory_data_width     = DMA_MEMORY_DATA_WIDTH_HALFWORD;
        dma_init_struct.memory_inc_enable     = TRUE;
        dma_init_struct.peripheral_base_addr  = (uint32_t) & (ADC1->odt);
        dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
        dma_init_struct.peripheral_inc_enable = FALSE;
        dma_init_struct.priority              = DMA_PRIORITY_HIGH;
        dma_init_struct.loop_mode_enable      = TRUE;
        dma_init(DMA1_CHANNEL1, &dma_init_struct);

        dma_channel_enable(DMA1_CHANNEL1, TRUE);
    }

    {  // adc
        adc_base_config_type adc_base_struct;
        crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
        crm_adc_clock_div_set(CRM_ADC_DIV_6);
        nvic_irq_enable(ADC1_IRQn, 0, 0);

        adc_base_default_para_init(&adc_base_struct);
        adc_base_struct.sequence_mode           = TRUE;
        adc_base_struct.repeat_mode             = FALSE;
        adc_base_struct.data_align              = ADC_RIGHT_ALIGNMENT;
        adc_base_struct.ordinary_channel_length = 3;
        adc_base_config(ADC1, &adc_base_struct);
			
        adc_ordinary_channel_set(ADC1, ADC_CHANNEL_4, 1, ADC_SAMPLETIME_239_5);
        adc_ordinary_channel_set(ADC1, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_239_5);
        adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6, 3, ADC_SAMPLETIME_239_5);
        adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
        adc_dma_mode_enable(ADC1, TRUE);
			
        adc_voltage_monitor_enable(ADC1, ADC_VMONITOR_SINGLE_ORDINARY);
        adc_voltage_monitor_threshold_value_set(ADC1, 0xBBB, 0xAAA);
        adc_voltage_monitor_single_channel_select(ADC1, ADC_CHANNEL_5);
        adc_interrupt_enable(ADC1, ADC_VMOR_INT, TRUE);

        adc_enable(ADC1, TRUE);
        adc_calibration_init(ADC1);
        while (adc_calibration_init_status_get(ADC1));
        adc_calibration_start(ADC1);
        while (adc_calibration_status_get(ADC1));
    }
}

// called in ADC1_IRQHandler()
void ADC_ConvCpltCbk(void) 
{
    m_bAdcSampSts = true;
}
