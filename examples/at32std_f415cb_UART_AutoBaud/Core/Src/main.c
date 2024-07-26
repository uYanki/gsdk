#include "gsdk.h"
#include "hwif.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "main"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/* usart define */
#define AU_UART          USART2
#define AU_UART_CRM_CLK  CRM_USART2_PERIPH_CLOCK
#define AU_UART_APB_FREQ crm_clocks_freq_struct.apb1_freq

/* usart tx pin define */
#define AU_UART_TX_PIN          GPIO_PINS_2
#define AU_UART_TX_GPIO         GPIOA
#define AU_UART_TX_GPIO_CRM_CLK CRM_GPIOA_PERIPH_CLOCK
#define AU_UART_TX_PIN_SOURCE   GPIO_PINS_SOURCE2

/* usart rx pin define */
#define AU_UART_RX_PIN          GPIO_PINS_3
#define AU_UART_RX_GPIO         GPIOA
#define AU_UART_RX_GPIO_CRM_CLK CRM_GPIOA_PERIPH_CLOCK
#define AU_UART_RX_PIN_SOURCE   GPIO_PINS_SOURCE3

/* exint define */
#define EXINT_SCFG_GPIO_PORT_SRC GPIO_PORT_SOURCE_GPIOA
#define EXINT_LINE_PORT          EXINT_LINE_3
#define EXINT_SCFG_PINS_SRC      GPIO_PINS_SOURCE3  // AU_UART_RX_PIN_SOURCE
#define EXINT_IRQN               EXINT3_IRQn
#define EXINT_IRQ_HANDLER        EXINT3_IRQHandler

/* tmr define */
#define AU_TMR          TMR2
#define AU_TMR_CRM_CLK  CRM_TMR2_PERIPH_CLOCK
#define AU_TMR_DIV      4
#define AU_TMR_PERIOD   65535
#define AU_TMR_APB_FREQ crm_clocks_freq_struct.apb1_freq
#define AU_TMR_APB_DIV  CRM->cfg_bit.apb1div

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

extern void DbgUartInit(uint32_t u32Baudrate);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

/* usart auto baud rate detect struct */
typedef struct _usart_auto_baudrate_struct {
    uint16_t tmr_cnt[16];          /* tmr counter value array */
    uint8_t  tmr_idx;              /* tmr counter value array index */
    uint16_t tmr_usart_freq_ratio; /* tmr clock and usart apb frequency ratio*/
    uint16_t usart_baudr_div;      /* usart baudrate division */
} usart_auto_baudrate_type;

usart_auto_baudrate_type au_baudrate;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief  this function handles for exint .
 * @param  none
 * @retval none
 */
void EXINT_IRQ_HANDLER(void)
{
    exint_flag_clear(EXINT_LINE_PORT);

    /* max support 10 bit */
    if (au_baudrate.tmr_idx < 10)
    {
        /* get tmr counter value */
        au_baudrate.tmr_cnt[au_baudrate.tmr_idx++] = AU_TMR->cval;
    }
}

/**
 * @brief  usart auto baud rate detection initialize.
 *         initialize tmr, exint.
 * @param  none
 * @retval none
 */
void au_usart_auto_baudrate_init(void)
{
    crm_clocks_freq_type crm_clocks_freq_struct = {0};
    uint32_t             tmr_apb_freq, usart_apb_freq;

    /* get system clock */
    crm_clocks_freq_get(&crm_clocks_freq_struct);

    /* usart apb frequency */
    usart_apb_freq = AU_UART_APB_FREQ;

    /* tmr apb frequency */
    if (AU_TMR_APB_DIV > 0)
    {
        tmr_apb_freq = crm_clocks_freq_struct.apb1_freq * 2;
    }
    else
    {
        tmr_apb_freq = crm_clocks_freq_struct.apb1_freq;
    }

    /*calculation tmr apb frequency and usart apb frequency ratio */
    if (tmr_apb_freq >= usart_apb_freq)
    {
        /* if tmr apb frequency greater than or equal to usart apb frequency */
        au_baudrate.tmr_usart_freq_ratio = tmr_apb_freq / usart_apb_freq;
    }
    else
    {
        /* if tmr apb frequency less than usart apb frequency */
        au_baudrate.tmr_usart_freq_ratio = usart_apb_freq / tmr_apb_freq | 0xFF00;
    }

    /* set tmr counter value array index to 0 */
    au_baudrate.tmr_idx = 0;

    /* timer config */
    crm_periph_clock_enable(AU_TMR_CRM_CLK, TRUE);
    tmr_base_init(AU_TMR, 65535, AU_TMR_DIV - 1);
    tmr_cnt_dir_set(AU_TMR, TMR_COUNT_UP);
    tmr_counter_enable(AU_TMR, TRUE);

    /* exti config */
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    exint_init_type exint_init_struct = {0};
    exint_init_struct.line_enable     = TRUE;
    exint_init_struct.line_mode       = EXINT_LINE_INTERRUPUT;
    exint_init_struct.line_select     = EXINT_LINE_PORT;
    exint_init_struct.line_polarity   = EXINT_TRIGGER_BOTH_EDGE;
    gpio_exint_line_config(EXINT_SCFG_GPIO_PORT_SRC, EXINT_SCFG_PINS_SRC);
    exint_init(&exint_init_struct);
   // nvic_irq_enable(EXINT_IRQN, 0, 0);
};

void au_usart_auto_baudrate_deinit(void)
{
    //    gpio_exint_line_deconfig(EXINT_SCFG_GPIO_PORT_SRC, EXINT_SCFG_PINS_SRC);

    //    exint_init_type exint_init_struct;
    //    exint_init_struct.line_enable   = FALSE;
    //    exint_init_struct.line_mode     = EXINT_LINE_INTERRUPUT;
    //    exint_init_struct.line_select   = EXINT_LINE_PORT;
    //    exint_init_struct.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
    //    exint_init(&exint_init_struct);

    exint_reset();
}

/**
 * @brief  calculate how many bits between the rising edges and falling edges.
 *         initialize tmr, usart, exint.
 * @param  start_tmr_cnt: calculate start tmr counter value
 * @param  end_tmr_cnt: calculate end tmr counter value
 * @param  one_bit_cnt: one bit need tmr counter value
 * @retval none
 */
uint8_t au_cal_number_of_bits(uint16_t start_tmr_cnt, uint16_t end_tmr_cnt, uint16_t one_bit_cnt)
{
    uint16_t cnt_interval;
    if (start_tmr_cnt >= end_tmr_cnt)
    {
        cnt_interval = 65535 - start_tmr_cnt + end_tmr_cnt;
    }
    else
    {
        cnt_interval = end_tmr_cnt - start_tmr_cnt;
    }

    if (cnt_interval <= one_bit_cnt)
    {
        return 1;
    }
    else
    {
        if ((cnt_interval % one_bit_cnt) > one_bit_cnt / 2)
        {
            return cnt_interval / one_bit_cnt + 1;
        }
        else
        {
            return cnt_interval / one_bit_cnt;
        }
    }
}

/**
 * @brief  usart auto baud rate detection funciotn.
 *         initialize tmr, usart, exint.
 * @param  none
 * @retval 0: auto baud rate detection not complete
 *           other value: detection complete, the value is receive data
 */
uint8_t au_baud_rate_detection(void)
{
    uint8_t data_bits[8] = {0}, data = 0, prev_bit_value = 1;
    uint8_t data_idx = 0, n_bits, total_bits = 0, i, tmr_cnt_idx = 2;

    if (au_baudrate.tmr_idx >= 2)
    {
        if (au_baudrate.tmr_cnt[0] >= au_baudrate.tmr_cnt[1])
        {
            au_baudrate.usart_baudr_div = 65535 - au_baudrate.tmr_cnt[0] + au_baudrate.tmr_cnt[1];
        }
        else
        {
            au_baudrate.usart_baudr_div = au_baudrate.tmr_cnt[1] - au_baudrate.tmr_cnt[0];
        }

        /*wait first byte complete*/
        DelayBlockMs(1);

        /* analyze data */
        while (total_bits < 8)
        {
            if (au_baudrate.tmr_idx >= tmr_cnt_idx && au_baudrate.tmr_idx > 2)
            {
                n_bits = au_cal_number_of_bits(au_baudrate.tmr_cnt[tmr_cnt_idx - 1],
                                               au_baudrate.tmr_cnt[tmr_cnt_idx],
                                               au_baudrate.usart_baudr_div);
                for (i = 0; i < n_bits && data_idx < 8; i++, data_idx++)
                {
                    data_bits[data_idx] = prev_bit_value;
                }
                total_bits += n_bits;
                prev_bit_value = ~prev_bit_value & 0x1;
            }
            else
            {
                if (tmr_cnt_idx <= 2)
                {
                    prev_bit_value = 0;
                }

                for (; data_idx < 8; data_idx++)
                {
                    data_bits[data_idx] = ~prev_bit_value & 0x1;
                }

                total_bits = 8;
            }
            tmr_cnt_idx++;
        }

        for (i = 0; i < 8; i++)
        {
            data |= data_bits[i] << i;
        }

        /* calculate usart baud rate div */
        if (au_baudrate.tmr_usart_freq_ratio & 0xFF00)
        {
            au_baudrate.usart_baudr_div = au_baudrate.usart_baudr_div *
                                          (au_baudrate.tmr_usart_freq_ratio & 0xFF) /
                                          AU_TMR_DIV;
        }
        else
        {
            au_baudrate.usart_baudr_div = au_baudrate.usart_baudr_div /
                                          (au_baudrate.tmr_usart_freq_ratio & 0xFF) *
                                          AU_TMR_DIV;
        }

        au_baudrate.tmr_idx = 0;

        /* disable exint interrupt */
        EXINT->inten &= ~EXINT_LINE_PORT;
        return data;
    }
    return 0;
}

uint8_t au_usart_init(void)
{
    {
        gpio_init_type gpio_init_struct = {0};

        crm_periph_clock_enable(AU_UART_TX_GPIO_CRM_CLK, TRUE);
        crm_periph_clock_enable(AU_UART_RX_GPIO_CRM_CLK, TRUE);

        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;

        gpio_init_struct.gpio_pins = AU_UART_TX_PIN;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(AU_UART_TX_GPIO, &gpio_init_struct);

        gpio_init_struct.gpio_pins = AU_UART_RX_PIN;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(AU_UART_RX_GPIO, &gpio_init_struct);
    }

    {
        crm_periph_clock_enable(AU_UART_CRM_CLK, TRUE);
			
        usart_init(AU_UART, 19200, USART_DATA_8BITS, USART_STOP_1_BIT);
        usart_transmitter_enable(AU_UART, TRUE);
        usart_receiver_enable(AU_UART, TRUE);

        /*config uart baudrate*/
       // AU_UART->baudr_bit.div = au_baudrate.usart_baudr_div;

        /* enable usart */
        usart_enable(AU_UART, TRUE);
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

    // DbgUartInit(115200);
    DelayInit();

    uint8_t data;
    uint8_t rx_flag = 0;

    /* auto baudrate init */
  //  au_usart_auto_baudrate_init();

    /* wait auto baud rate detection complete
     *  note: the detection character must starting with a bit at '1'
     *  example data: 0xx1, 0xx3, 0xx5, 0xx7...
     */
 //   while ((data = au_baud_rate_detection()) == 0);
		
		/* auto baudrate deinit */
   // au_usart_auto_baudrate_deinit();

    au_usart_init();

    /* send ack */
    const char* szAck = "hello world\n";
    while (*szAck)
    {
        while (usart_flag_get(AU_UART, USART_TDBE_FLAG) == RESET);
        usart_data_transmit(AU_UART, (uint16_t)(*szAck++));
    }

    while (1)
    {
        if (usart_flag_get(AU_UART, USART_RDBF_FLAG) != RESET)
        {
            data    = usart_data_receive(AU_UART);
            rx_flag = 1;
        }
        if (rx_flag == 1)
        {
            if (usart_flag_get(AU_UART, USART_TDBE_FLAG) != RESET)
            {
                usart_data_transmit(AU_UART, (uint16_t)data);
                rx_flag = 0;
            }
        }
    }
}
