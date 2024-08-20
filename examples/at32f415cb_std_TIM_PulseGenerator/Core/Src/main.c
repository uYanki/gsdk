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

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static const pin_t btn = {BUTTON_PIN};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void PulseOut_Demo(uint16_t u16PulseCnt)
{
    if (u16PulseCnt == 0)
    {
        return;
    }

    /* get system clock */
    crm_clocks_freq_type crm_clocks_freq_struct = {0};
    crm_clocks_freq_get(&crm_clocks_freq_struct);

    /* enable tmr1/gpioa/gpiob clock */
    crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    /* timer1 output pin(TMR1_CH1_PA8) Configuration */
    gpio_init_type gpio_init_struct = {
        .gpio_pins           = GPIO_PINS_8,
        .gpio_mode           = GPIO_MODE_MUX,
        .gpio_out_type       = GPIO_OUTPUT_PUSH_PULL,
        .gpio_pull           = GPIO_PULL_NONE,
        .gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER,
    };
    gpio_init(GPIOA, &gpio_init_struct);

    /* tmr1 configuration generate pwm signal*/
    /* compute the value to be set in arr regiter to generate signal frequency at 17.57 khz */
    uint16_t timer_period = (crm_clocks_freq_struct.sclk_freq / 17570) - 1;

    /* compute c1dt value to generate a duty cycle at 50% for channel 1 and 1c */
    uint16_t channel_pulse = (uint16_t)(((uint32_t)5 * (timer_period - 1)) / 10);

    /* tmr base configuration */
    tmr_repetition_counter_set(TMR1, u16PulseCnt - 1);
    tmr_base_init(TMR1, timer_period, 0);
    tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
    tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);

    /* channel1 configuration in output mode */
    tmr_output_config_type tmr_output_struct = {
        .oc_mode         = TMR_OUTPUT_CONTROL_PWM_MODE_A,
        .oc_output_state = TRUE,
        .oc_polarity     = TMR_OUTPUT_ACTIVE_HIGH,
        .oc_idle_state   = TRUE,
    };
    tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
    tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, channel_pulse);

    /* one cycle mode selection */
    tmr_one_cycle_mode_enable(TMR1, TRUE);

    /* ADVTMR output enable */
    tmr_output_enable(TMR1, TRUE);

    while (1)
    {
        /* press KEY to generate PWM output*/
        if (PIN_ReadLevel(&btn) == PIN_LEVEL_HIGH)
        {
            /* enable tmr1 */
            tmr_counter_enable(TMR1, TRUE);

            /*wait key release */
            while ((PIN_ReadLevel(&btn) == PIN_LEVEL_HIGH));
        }
    }
}

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

    PIN_SetMode(&btn, PIN_MODE_INPUT_FLOATING, PIN_PULL_NONE);

    // DbgUartInit(115200);
    // DelayInit();

    PulseOut_Demo(8);
}
