#include "gsdk.h"
#include "hwif.h"

#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "mouse_class.h"
#include "mouse_desc.h"

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

otg_core_type otg_core_struct;
static bool   m_bMousePressed = false;

static const pin_t btn = {BUTTON_PIN};

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

    PIN_SetMode(&btn, PIN_MODE_INPUT_FLOATING, PIN_PULL_NONE);

    DbgUartInit(115200);
    DelayInit();

    USBD_Init();

    while (1)
    {
        if (PIN_ReadLevel(&btn) == PIN_LEVEL_HIGH)
        {
            if (usbd_connect_state_get(&otg_core_struct.dev) == USB_CONN_STATE_CONFIGURED)
            {
                usb_hid_mouse_send(&otg_core_struct.dev, RIGHT_MOVE); /*  Û±Í”““∆ */
                m_bMousePressed = true;
            }
        }
        else if (m_bMousePressed == true)
        {
            DelayBlockMs(10);
            usb_hid_mouse_send(&otg_core_struct.dev, BUTTON_RELEASE);
            m_bMousePressed = false;
        }
    }
}

static void USBD_Init(void)
{
    {  // gpio

        gpio_init_type gpio_init_struct;

        crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
        gpio_default_para_init(&gpio_init_struct);

        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;
        gpio_init_struct.gpio_pull           = GPIO_PULL_NONE;

#ifdef USB_SOF_OUTPUT_ENABLE
        crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
        gpio_init_struct.gpio_pins = OTG_PIN_SOF;
        gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
#endif

        /* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
        gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
        gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif
    }

    {  // usb

        crm_periph_clock_enable(OTG_CLOCK, TRUE);

        switch (system_core_clock)  // set usb clock to 48MHz
        {
            case 48000000: crm_usb_clock_div_set(CRM_USB_DIV_1); break;    /* 48MHz */
            case 72000000: crm_usb_clock_div_set(CRM_USB_DIV_1_5); break;  /* 72MHz */
            case 96000000: crm_usb_clock_div_set(CRM_USB_DIV_2); break;    /* 96MHz */
            case 120000000: crm_usb_clock_div_set(CRM_USB_DIV_2_5); break; /* 120MHz */
            case 144000000: crm_usb_clock_div_set(CRM_USB_DIV_3); break;   /* 144MHz */
            default: break;
        }

        nvic_irq_enable(OTG_IRQ, 0, 0);
        usb_sts_type sts = usbd_init(&otg_core_struct, USB_FULL_SPEED_CORE_ID, USB_ID, &mouse_class_handler, &mouse_desc_handler);
        GSDK_ASSERT(sts == USB_OK, "usb init failed");
    }
}

void usb_delay_ms(uint32_t ms)
{
    /* user can define self delay function */
    DelayBlockMs(ms);
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
