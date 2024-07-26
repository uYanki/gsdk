#include "gsdk.h"
#include "hwif.h"

#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "winusb_class.h"
#include "winusb_desc.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "main"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

extern void DbgUartInit(uint32_t u32Baudrate);

static void USBD_Init(void);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

/* usb global struct define */
otg_core_type otg_core_struct;
uint8_t       usb_buffer[256];

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

    // DbgUartInit(115200);
    DelayInit();

    USBD_Init();

    uint16_t data_len;
    uint8_t  send_zero_packet = 0;
    uint32_t timeout;

    while (1)
    {
			#if 0
        /* get usb vcp receive data */
        data_len = usb_vcp_get_rxdata(&otg_core_struct.dev, usb_buffer);

        if (data_len > 0 || send_zero_packet == 1)
        {
            /* bulk transfer is complete when the endpoint does one of the following
               1 has transferred exactly the amount of data expected
               2 transfers a packet with a payload size less than wMaxPacketSize or transfers a zero-length packet
            */
            if (data_len > 0)
            {
                send_zero_packet = 1;
            }

            if (data_len == 0)
            {
                send_zero_packet = 0;
            }

            timeout = 5000000;
            do
            {
                /* send data to host */
                if (usb_vcp_send_data(&otg_core_struct.dev, usb_buffer, data_len) == SUCCESS)
                {
                    break;
                }
            } while (timeout--);
        }
			#endif
    }
}

/**
 * @brief  this function config usb.
 * @param  none
 * @retval none
 */
static void USBD_Init(void)
{
    {
        gpio_init_type gpio_init_struct;

        crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
        gpio_default_para_init(&gpio_init_struct);

        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;
        gpio_init_struct.gpio_pull           = GPIO_PULL_NONE;

        /* dp and dm */
        gpio_init_struct.gpio_pins = OTG_PIN_DP | OTG_PIN_DM;
        gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
    }

    {
        /* enable otgfs clock */
        crm_periph_clock_enable(OTG_CLOCK, TRUE);

        /* select usb 48m clcok source */
        switch (system_core_clock)
        {
            case 48000000: crm_usb_clock_div_set(CRM_USB_DIV_1); break;    /* 48MHz */
            case 72000000: crm_usb_clock_div_set(CRM_USB_DIV_1_5); break;  /* 72MHz */
            case 96000000: crm_usb_clock_div_set(CRM_USB_DIV_2); break;    /* 96MHz */
            case 120000000: crm_usb_clock_div_set(CRM_USB_DIV_2_5); break; /* 120MHz */
            case 144000000: crm_usb_clock_div_set(CRM_USB_DIV_3); break;   /* 144MHz */
            default: break;
        }

        /* enable otgfs irq */
        nvic_irq_enable(OTG_IRQ, 0, 0);

        /* init usb */
        usbd_init(&otg_core_struct,
                  USB_FULL_SPEED_CORE_ID,
                  USB_ID,
                  &cdc_class_handler,
                  &cdc_desc_handler);
    }
}

/**
 * @brief  this function handles otgfs interrupt.
 * @param  none
 * @retval none
 */
void OTG_IRQ_HANDLER(void)
{
    usbd_irq_handler(&otg_core_struct);
}

/**
 * @brief  usb delay millisecond function.
 * @param  ms: number of millisecond delay
 * @retval none
 */
void usb_delay_ms(uint32_t ms)
{
    /* user can define self delay function */
    DelayBlockMs(ms);
}
