/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gsdk.h"
#include "hwif.h"
#include "flexbtn.h"
#include "i2c_ssd1306.h"
#include "mono_framebuf.h"
#include "paratbl.h"
#include "freemodbus/mb.h"
#include "encoder/encoder.h"
#include "gconf.h"
#include "adconv.h"
#include "pwmctrl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define __ENC_HALL_SW  1

#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 32

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LOG_LOCAL_TAG   "main"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static void EventCb(flexbtn_t* pHandle, flexbtn_event_e eEvent);
static bool IsPressed(flexbtn_t* pHandle);

typedef enum {
    BUTTON_PREV,
    BUTTON_OKAY,
    BUTTON_NEXT,
} button_id_e;

static pin_t keys[] = {
    {KEY1_PIN},
    {KEY2_PIN},
    {KEY3_PIN},
};

static flexbtn_t flexbtn[3] = {
    {
     .u8ID                         = BUTTON_PREV,
     .u16ShortPressStartTick       = FLEXBTN_MS_TO_SCAN_CNT(1500),
     .u16LongPressStartTick        = FLEXBTN_MS_TO_SCAN_CNT(3000),
     .u16LongHoldStartTick         = FLEXBTN_MS_TO_SCAN_CNT(4500),
     .u16MaxMultipleClicksInterval = FLEXBTN_MS_TO_SCAN_CNT(300),
     .pfnIsPressed                 = IsPressed,
     .pfnEventCb                   = EventCb,
     },

    {
     .u8ID                         = BUTTON_OKAY,
     .u16ShortPressStartTick       = FLEXBTN_MS_TO_SCAN_CNT(1000),
     .u16LongPressStartTick        = FLEXBTN_MS_TO_SCAN_CNT(2000),
     .u16LongHoldStartTick         = FLEXBTN_MS_TO_SCAN_CNT(3000),
     .u16MaxMultipleClicksInterval = FLEXBTN_MS_TO_SCAN_CNT(300),
     .pfnIsPressed                 = IsPressed,
     .pfnEventCb                   = EventCb,
     },

    {
     .u8ID                         = BUTTON_NEXT,
     .u16ShortPressStartTick       = FLEXBTN_MS_TO_SCAN_CNT(1500),
     .u16LongPressStartTick        = FLEXBTN_MS_TO_SCAN_CNT(3000),
     .u16LongHoldStartTick         = FLEXBTN_MS_TO_SCAN_CNT(4500),
     .u16MaxMultipleClicksInterval = FLEXBTN_MS_TO_SCAN_CNT(300),
     .pfnIsPressed                 = IsPressed,
     .pfnEventCb                   = EventCb,
     },
};

//----------

static uint8_t m_au8Framebuf[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = {0};

static mono_framebuf_t fb = {
    .pu8Buffer   = m_au8Framebuf,
    .u16Width    = SSD1306_WIDTH,
    .u16Height   = SSD1306_HEIGHT,
    ._u16CursorX = 0,
    ._u16CursorY = 0,
};

static i2c_mst_t i2c = {
    .SDA  = {OLED091_SDA_PIN},
    .SCL  = {OLED091_SCL_PIN},
    .I2Cx = &hi2c1,
};

static i2c_ssd1306_t ssd1306 = {
    .hI2C      = &i2c,
    .u8SlvAddr = SSD1306_ADDRESS_LOW,
    .u8Cols    = SSD1306_WIDTH,
    .u8Rows    = SSD1306_HEIGHT / 8,
};

static void SSD1306_Reset(void)
{
    DelayBlockMs(20);
    HAL_GPIO_WritePin(OLED091_RST_PIN, GPIO_PIN_RESET);
    DelayBlockMs(20);
    HAL_GPIO_WritePin(OLED091_RST_PIN, GPIO_PIN_SET);
}

//----------

void ParaTblInit(void);

//----------
#include "encoder/hall_enc.h"
#if __ENC_HALL_SW
static hall_enc_t sHallEnc = {
    .eHallState   = HALL_STATE_0,
    .eDirection   = HALL_DIR_FWD,
    // .u8Placement    = 120,
    .u8PhaseShift = 0,
    .u16ElecAngle = 0,
    .u16EdgeCount = 0,
    .u16HallError = 0,
};
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    D._Resv100 = ADConv[0];
    D._Resv101 = ADConv[1];
    D._Resv102 = ADConv[2];
    D._Resv103 = ADConv[3];
    // D._Resv104 = ADConv[4];
    // D._Resv105 = ADConv[5];

    D._Resv104 = GetCurU(AXIS_0);
    D._Resv105 = GetCurV(AXIS_0);
    D._Resv106 = GetCurW(AXIS_0);

    D.s16UaiPu0 = GetUai1();
    D.s16UaiPu1 = GetUai2();

    D.s16UaiSi0 = GetUai1();
    D.s16UaiSi1 = GetUai2();
    D.u16UcdcSi = D.u16UmdcSi = GetUai2();

    // AxisIsr(AXIS_1);
}

typedef enum {
    AXIS_INIT,
    AXIS_RUN,
    AXIS_IDLE,
} axis_state_e;

typedef enum {
    AXIS_APP_GENERIC,   // 通用控制
    AXIS_APP_OPENLOOP,  // 开环测试
    AXIS_APP_ENCIDENT,  // 编码器辨识
} axis_app_e;

typedef union {
    struct {
        uint32_t ServoOn : 1;
        uint32_t _Resv   : 31;
    } u32Bit;
    uint32_t u32All;
} ctrlword_u;

#include "motdrv.h"

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    SEGGER_RTT_Init();
    cm_backtrace_init("demo", "stm32", "V0.01");
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_TIM1_Init();
    MX_USART1_UART_Init();
    MX_USART3_UART_Init();
    MX_CRC_Init();
    MX_TIM6_Init();
    MX_USART2_UART_Init();
    MX_SPI1_Init();
    MX_TIM3_Init();
    MX_TIM14_Init();
    /* USER CODE BEGIN 2 */
    DelayInit();

    ParaTblInit();

    I2C_Master_Init(&i2c, 4e6, I2C_DUTYCYCLE_50_50);
    I2C_Master_ScanAddress(&i2c);

    // oled
    SSD1306_Reset();
    SSD1306_Init(&ssd1306);

    MonoFramebuf_FillRectangle(&fb, 10, 10, 20, 20, MONO_COLOR_WHITE);
    MonoFramebuf_FillRectangle(&fb, 20, 20, 20, 20, MONO_COLOR_XOR);
    MonoFramebuf_SetCursor(&fb, 0, 0);
    MonoFramebuf_PutString(&fb, "Servo", &g_Font_Conslons_8x16_CpuFlash, MONO_COLOR_WHITE, MONO_COLOR_BLACK);

    SSD1306_FillBuffer(&ssd1306, MonoFramebuf_GetBuffer(&fb));

    // key
    for (int i = 0; i < ARRAY_SIZE(flexbtn); ++i)
    {
        PIN_SetMode(&keys[i], PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
        FlexBtn_Attach(&flexbtn[i]);
    }

    // modbus(rtu)
    RO u8 ucSlaveID[] = {0xAA, 0xBB, 0xCC};
    eMBInit(MB_RTU, D.u16MbSlvId, 1, D.u32MbBaudrate, MB_PAR_EVEN /* P.u16MbParity */);
    eMBSetSlaveID(0x34, true, ucSlaveID, ARRAY_SIZE(ucSlaveID));
    eMBEnable();
    MbRtuRun();

    // hall enc
    HallEnc_Creat(&sHallEnc);
    HallEnc_Init(&sHallEnc);

    // #if __ENC_HALL_SW
    //     HAL_TIM_Base_Start(&TIM_HALL);
    // #elif __ENC_INC_SW
    //     HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2);
    // #endif

    // adc
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADConv[0], ARRAY_SIZE(ADConv));

    axis_e eAxisNo = AXIS_0;
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, (P(eAxisNo).u16PwmDutyMax >> 1) - 200);
    HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_4);

    P(eAxisNo).u16AxisFSM = AXIS_INIT;

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    sHallEnc.u16EncRes = 6 * P(AXIS_0).u16MotPolePairs;

    // P(AXIS_0).u32CommCmd = 1;

    while (1)
    {
        // led
        PeriodicTask(UNIT_S, HAL_GPIO_TogglePin(LED2_PIN));

        // key
        PeriodicTask(UNIT_S / CONFIG_FLEXBTN_SCAN_FREQ_HZ, FlexBtn_Cycle());

        // PeriodicTask(125 * UNIT_US, HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADConv[0], ARRAY_SIZE(ADConv)));

        // modbus
        eMBPoll();

        D.u32SysRunTime = GetTickMs();

        HallEnc_Isr(&sHallEnc);

        axis_e eAxisNo = AXIS_0;

        P(eAxisNo).u16HallState  = sHallEnc.eHallState;
        P(eAxisNo).u16EncRes     = sHallEnc.u16EncRes;
        P(eAxisNo).u16EncPos     = sHallEnc.u16EncPos;
        P(eAxisNo).s32EncTurns   = sHallEnc.s32EncTurns;
        P(eAxisNo).s64EncMultPos = sHallEnc.s64EncMultPos;
        P(eAxisNo).u16ElecAngle  = sHallEnc.u16ElecAngle;

        P(eAxisNo).s32UserSpdFb = sHallEnc.s16SpdFb;

        {
#define BIT_SERVO_ON 0

            static u32 u32CommCmdPre = 0;
            u32        u32CommCmdCur = P(eAxisNo).u32CommCmd;

            if (u32CommCmdPre ^ u32CommCmdCur)
            {
                ctrlword_u uCtrlWord;

                uCtrlWord.u32All = u32CommCmdCur;

                if (uCtrlWord.u32Bit.ServoOn)  // TODO: use union
                {
                    PWM_Start(eAxisNo);
                    P(eAxisNo).u16AxisFSM = AXIS_RUN;
                }
                else
                {
                    PWM_Stop(eAxisNo);
                    P(eAxisNo).u16AxisFSM = AXIS_IDLE;
                }

                u32CommCmdPre = u32CommCmdCur;
            }
        }

        if (P(eAxisNo).u16AxisFSM == AXIS_RUN)
        {
            switch (P(eAxisNo).u16AppSel)
            {
                case AXIS_APP_GENERIC:
                {
                    break;
                }
                case AXIS_APP_OPENLOOP:
                {
                    PeriodicTask(125 * UNIT_US, {
                        motdrv_t foc;

                        foc.Uq      = 8000;
                        foc.Ud      = 0;
                        foc.DutyMax = P(eAxisNo).u16PwmDutyMax;

                        P(eAxisNo).u16ElecAngle += 125;

                        MC_SinCos(&foc, P(eAxisNo).u16ElecAngle);
                        MC_InvPark(&foc);

                        switch (P(eAxisNo).s16Uq)
                        {
                            default:
                            case 0:
                                MC_InvClark(&foc);
                                MC_MinMaxU(&foc);
                                MC_ZeroSeqInj(&foc);
                                break;
                            case 1:
                                SVPWMx(&foc);
                                break;
                            case 2:
                                svm(&foc);
                                break;
                            case 3:
                                SVGEN_run(&foc, 0);
                                break;
                        }

                        PWM_SetDuty(foc.Ta, foc.Tb, foc.Tc, eAxisNo);
                    });

                    break;
                }
                case AXIS_APP_ENCIDENT:
                {
                    break;
                }
            }
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN            = 8;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

static bool IsPressed(flexbtn_t* pHandle)
{
    switch ((button_id_e)(pHandle->u8ID))
    {
        case BUTTON_PREV:
        case BUTTON_OKAY:
        case BUTTON_NEXT:
        {
            return PIN_ReadLevel(&keys[pHandle->u8ID]) == PIN_LEVEL_LOW;
        }

        default:
        {
            LOGW("unknown button id");
            break;
        }
    }

    return false;
}

static void EventCb(flexbtn_t* pHandle, flexbtn_event_e eEvent)
{
    if (flexbtn[BUTTON_NEXT].eEvent == FLEXBTN_EVENT_LONG_HOLD && flexbtn[BUTTON_OKAY].eEvent == FLEXBTN_EVENT_RELEASE)
    {
        LOGI("combined key event = %s + %s", FlexBtnEventStr(flexbtn[BUTTON_NEXT].eEvent), FlexBtnEventStr(flexbtn[BUTTON_OKAY].eEvent));
    }
    else
    {
        LOGI("key%d event = %s", pHandle->u8ID, FlexBtnEventStr(eEvent));
    }
}

void ParaTblInit(void)
{
    uint16_t* pParaTbl;

    //

    pParaTbl = (uint16_t*)&D;

    for (uint16_t i = 0; i < sizeof(device_para_t) / sizeof(u16); ++i)
    {
        pParaTbl[i] = sDeviceAttr[i].u16InitVal;
    }

    D.u32ChipDevID  = HAL_GetDEVID();
    D.u32ChipRevID  = HAL_GetREVID();
    D.u32ChipUIDw0  = HAL_GetUIDw0();
    D.u32ChipUIDw1  = HAL_GetUIDw1();
    D.u32ChipUIDw2  = HAL_GetUIDw2();
    D.u32HalVersion = HAL_GetHalVersion();

    //

    axis_e eAxisNo = AXIS_0;

    pParaTbl = (uint16_t*)&P(eAxisNo);

    for (uint16_t i = 0; i < sizeof(axis_para_t) / sizeof(u16); ++i)
    {
        pParaTbl[i] = sAxisAttr[i].u16InitVal;
    }

    // TIM freq / Carry freq / 2
    P(eAxisNo).u16PwmDutyMax = 64000000 / P(eAxisNo).u16CarryFreq / 2;
}

void AxisApp()
{
}

void OpenLoop_Creat(axis_e eAxisNo)
{
}

void OpenLoop_Init(axis_e eAxisNo)
{
}

void OpenLoop_Cycle(axis_e eAxisNo)
{
}

void OpenLoop_Isr(axis_e eAxisNo)
{
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
