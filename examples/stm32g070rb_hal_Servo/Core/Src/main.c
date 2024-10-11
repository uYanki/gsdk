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
#include "motdrv.h"

#include "pid.h"
#include "drvsche.h"
#include "arm_math.h"
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

// #include "encoder/absEnc.h"

// abs_drv_t AbsDrv;
// abs_enc_t AbsEnc;

#define ONE_ADC_VOLTAGE 806  // unit uV
#define AMP_GAIN        11.0

int phase_current_from_adcval(uint32_t ADCValue)
{
    int amp_gain = AMP_GAIN;

    int shunt_conductance = 500;  // 100 means 1 mOh, current sensing resistor

    int amp_out_volt = ONE_ADC_VOLTAGE * ADCValue;
    int shunt_volt   = amp_out_volt / amp_gain;
    int current      = (shunt_volt * 100) / shunt_conductance;  // unit mA
    return current;
}

static arm_pid_instance_q15 s_sSpdPi;
static arm_pid_instance_f32 s_sPosPi;
static bool                 s_bSpdInit = true;
static bool                 s_bPosInit = true;

#include "park.h"
#include "clarke.h"
#include "svpwm.h"

typedef struct {
    park_t    sPark;
    ipark_t   sRevPark;
    iclarke_t sRevClarke;
    clarke_t  sClarke;
    svpwm_t   sSvpwm;

    PID_t sIdPid;
    PID_t sIqPid;

    s16 s16Iab[2];
    s16 s16Idq[2];
    s16 s16Uph[3];

    q15 u16UmdcPu;

} mot_ctrl_t;

mot_ctrl_t sMotCtrl;

void MotCtrlCreat(mot_ctrl_t* pMotCtrl, axis_e eAxisNo)
{
    D.u16UmdcSi = 12;

    pMotCtrl->sClarke.q15PhA_i   = &P(eAxisNo).s16IaFbSi;
    pMotCtrl->sClarke.q15PhB_i   = &P(eAxisNo).s16IbFbSi;
    pMotCtrl->sClarke.q15PhC_i   = &P(eAxisNo).s16IcFbSi;
    pMotCtrl->sClarke.q15Alpha_o = &pMotCtrl->s16Iab[0];
    pMotCtrl->sClarke.q15Beta_o  = &pMotCtrl->s16Iab[1];

    pMotCtrl->sPark.q15Alpha_i   = pMotCtrl->sClarke.q15Alpha_o;
    pMotCtrl->sPark.q15Beta_i    = pMotCtrl->sClarke.q15Beta_o;
    pMotCtrl->sPark.u16ElecAng_i = &P(eAxisNo).u16ElecAngleFb;
    pMotCtrl->sPark.q15D_o       = &pMotCtrl->s16Idq[1];
    pMotCtrl->sPark.q15Q_o       = &pMotCtrl->s16Idq[0];

    pMotCtrl->sRevPark.q15D_i       = &P(eAxisNo).s16Ud;
    pMotCtrl->sRevPark.q15Q_i       = &P(eAxisNo).s16Uq;
    pMotCtrl->sRevPark.u16ElecAng_i = &P(eAxisNo).u16ElecAngleRef;
    pMotCtrl->sRevPark.q15Alpha_o   = &P(eAxisNo).s16Ualpha;
    pMotCtrl->sRevPark.q15Beta_o    = &P(eAxisNo).s16Ubeta;

    pMotCtrl->sRevClarke.q15Alpha_i = pMotCtrl->sRevPark.q15Alpha_o;
    pMotCtrl->sRevClarke.q15Beta_i  = pMotCtrl->sRevPark.q15Beta_o;
    pMotCtrl->sRevClarke.q15PhA_o   = &pMotCtrl->s16Uph[0];
    pMotCtrl->sRevClarke.q15PhB_o   = &pMotCtrl->s16Uph[1];
    pMotCtrl->sRevClarke.q15PhC_o   = &pMotCtrl->s16Uph[2];

    pMotCtrl->sSvpwm.q15Ua_i      = pMotCtrl->sRevClarke.q15PhA_o;
    pMotCtrl->sSvpwm.q15Ub_i      = pMotCtrl->sRevClarke.q15PhB_o;
    pMotCtrl->sSvpwm.q15Uc_i      = pMotCtrl->sRevClarke.q15PhC_o;
    pMotCtrl->sSvpwm.q15Udc_i     = &pMotCtrl->u16UmdcPu;
    pMotCtrl->sSvpwm.eSector_o    = &P(eAxisNo).u16Sector;
    pMotCtrl->sSvpwm.u16PwmCmpA_o = &P(eAxisNo).u16PwmaComp;
    pMotCtrl->sSvpwm.u16PwmCmpB_o = &P(eAxisNo).u16PwmbComp;
    pMotCtrl->sSvpwm.u16PwmCmpC_o = &P(eAxisNo).u16PwmcComp;
    pMotCtrl->sSvpwm.u16PwmPeriod = &P(eAxisNo).u16PwmDutyMax;
}

void MotCtrlIsr(mot_ctrl_t* pMotCtrl)
{
    pMotCtrl->u16UmdcPu = 32767;//GetUmdc() ;
	

#if 0

    Park(&pMotDrv->sPark);
    Clarke3(&pMotDrv->sClarke);

    IdPid(&pMotDrv->sIdPid);
    IqPid(&pMotDrv->sIqPid);

#endif

    RevPark(&pMotCtrl->sRevPark);
    RevClarke(&pMotCtrl->sRevClarke);
    Svpwm(&pMotCtrl->sSvpwm);

    PWM_SetDuty(
        *pMotCtrl->sSvpwm.u16PwmCmpA_o,
        *pMotCtrl->sSvpwm.u16PwmCmpB_o,
        *pMotCtrl->sSvpwm.u16PwmCmpC_o,
        AXIS_0);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    //  return;
    tick_t t = GetTick100ns();

    s16 _Resv100Pre = D._Resv100;
    s16 _Resv101Pre = D._Resv101;

    D._Resv100 = ADConv[0];
    D._Resv101 = ADConv[1];
    D._Resv102 = ADConv[2];
    D._Resv103 = ADConv[3];
    D._Resv104 = ADConv[4];
    D._Resv105 = ADConv[5];

    GetCurU(AXIS_0);

    //    D._Resv104 = GetCurU(AXIS_0);
    //    D._Resv105 = GetCurV(AXIS_0);
    //    D._Resv106 = GetCurW(AXIS_0);

    //    D.s16UaiPu0 = GetUai1();
    //    D.s16UaiPu1 = GetUai2();

    //    D.s16UaiSi0 = GetUai1();
    //    D.s16UaiSi1 = GetUai2();
    //    D.u16UcdcSi = D.u16UmdcSi = GetUai2();
    static int off[2] = {0};
    static int i      = 0;

#define TT 1000
    if (i < TT)
    {
        off[0] += (s16)D._Resv100;
        off[1] += (s16)D._Resv101;
        i++;
    }
    else if (i == TT)
    {
        off[0] /= TT;
        off[1] /= TT;
        i += 1;
    }
    else
    {
        mc_t foc = {0};

        foc.u16ElecAngle = P(0).u16ElecAngleFb;

        foc.Ia = ((D._Resv100 * 70 + _Resv100Pre * 30) / 100 - off[0]);  // mA
        foc.Ib = ((D._Resv101 * 70 + _Resv101Pre * 30) / 100 - off[1]);  // mA
        foc.Ic = -foc.Ia - foc.Ib;

        MC_Clark(&foc);
        MC_SinCos(&foc);
        MC_Park(&foc);

        D._Resv106 = foc.Ia;
        D._Resv107 = foc.Ib;
        D._Resv108 = foc.Ic;
        D._Resv109 = foc.Ialpha;
        D._Resv110 = foc.Ibeta;
        D._Resv111 = foc.Iq;
        D._Resv112 = foc.Id;
        D._Resv113++;  // 中断次数
        DrvScheIsr();
        P(0).s32SpdDigRef04 = GetTick100ns() - t;

        axis_e eAxisNo = AXIS_0;

        void MotDrv_Isr(mc_t * pMotDrv, axis_e eAxisNo);

        switch (P(eAxisNo).u16AppSel)
        {
            case AXIS_APP_GENERIC:
            {
                if (P(eAxisNo).u16AxisFSM == AXIS_STATE_ENABLE)
                {
                    PeriodicTask(125 * UNIT_US, {
                        mc_t foc = {0};

                        foc.DutyMax = P(eAxisNo).u16PwmDutyMax;
                        // foc.u16ElecAngle = sHallEnc.u16ElecAngle;  // hall

                        foc.u16ElecAngle = P(eAxisNo).u16ElecAngleFb;

                        P(eAxisNo).u16CtrlMode = CTRL_MODE_SPD;

                        switch ((ctrl_mode_e)P(eAxisNo).u16CtrlMode)
                        {
                            case CTRL_MODE_POS:
                            {
                                break;
                                static PID_t pid = {0};

                                pid.Kp = P(eAxisNo).u16PosLoopKp / 1000.f;
                                pid.Ki = P(eAxisNo).u16PosLoopKi / 1000.f;
                                pid.Kd = P(eAxisNo).u16PosLoopKd / 1000.f;
                                // arm_pid_init_f32(&s_sPosPi, s_bPosInit);

                                s_sSpdPi.Kp = P(eAxisNo).u16SpdLoopKp;
                                s_sSpdPi.Ki = P(eAxisNo).u16SpdLoopKi;
                                s_sSpdPi.Kd = P(eAxisNo).u16SpdLoopKd;
                                arm_pid_init_q15(&s_sSpdPi, s_bSpdInit);

                                s_bSpdInit = s_bPosInit = false;

                                //    q31_t q = arm_pid_f32(&s_sPosPi, -(P(eAxisNo).s32SpdDigRef00 - P(eAxisNo).s64EncMultPos));  // to 0.1rpm

                                pid.ref = P(eAxisNo).s32SpdDigRef00;
                                pid.fbk = P(eAxisNo).s64EncMultPos;
                                pid.Ts  = 125.f / 1e6;
                                PID_Handler_Tustin(&pid);

                                s32 q = pid.out;

                                P(eAxisNo).s32SpdDigRef02 = pid.out;

                                q      = q * 60 * 10 / P(eAxisNo).u32EncRes;
                                foc.Uq = arm_pid_q15(&s_sSpdPi, q - P(eAxisNo).s32DrvSpdFb);

                                foc.Uq = CLAMP(foc.Uq, -10000, 10000);

                                break;
                            }

                            case CTRL_MODE_SPD:
                            {
                                // qdAxis.pdf
                                P(eAxisNo).s16Uq = P(eAxisNo).s32DrvSpdRef;
                                break;
                            }

                            case CTRL_MODE_TRQ:
                            {
                                break;
                            }

                            default:
                            case CTRL_MODE_DS402:  // Get ElecAngle Offset
                            {
                                break;
                            }
                        }

                        MotCtrlIsr(&sMotCtrl);
                    });
                }

                break;
            }

            case AXIS_APP_OPENLOOP:
            case AXIS_APP_ENCIDENT:
            {
              
                    MotCtrlIsr(&sMotCtrl);
              
                break;
            }

            default:
            {
                // AlmUpdate()
                break;
            }
        }
    }
}

void MotDrv_Isr(mc_t* pMotDrv, axis_e eAxisNo)
{
    MC_SinCos(pMotDrv);
    MC_InvPark(pMotDrv);

    switch (P(eAxisNo)._Resv519)
    {
        default:
        case 0:
            MC_InvClark(pMotDrv);
            MC_MinMaxU(pMotDrv);
            MC_ZeroSeqInj(pMotDrv);
            break;
        case 1:
            SVPWMx(pMotDrv);
            break;
        case 2:
            svm(pMotDrv);
            break;
        case 3:
            SVGEN_run(pMotDrv, 0);
            break;
    }

    PWM_SetDuty(pMotDrv->Ta, pMotDrv->Tb, pMotDrv->Tc, eAxisNo);

    P(eAxisNo).u16ElecAngleRef = pMotDrv->u16ElecAngle;

    P(eAxisNo).s16Ualpha = pMotDrv->Ualpha;
    P(eAxisNo).s16Ubeta  = pMotDrv->Ubeta;

    P(eAxisNo).s16Ud = pMotDrv->Ud;
    P(eAxisNo).s16Uq = pMotDrv->Uq;

    P(eAxisNo).u16Sector = pMotDrv->eSector;

    P(eAxisNo).u16PwmaComp = pMotDrv->Ta;
    P(eAxisNo).u16PwmbComp = pMotDrv->Tb;
    P(eAxisNo).u16PwmcComp = pMotDrv->Tc;
}

#include "axis.h"
#include "axis_defs.h"

#include "spi_mt6701.h"
#include "i2c_as5600.h"

spi_mst_t spi = {
    .SPIx = &hspi1,
    .MISO = {GPIOB,             GPIO_PIN_4 }, /* DAT */
    .MOSI = {GPIOB,             GPIO_PIN_4 },
    .SCLK = {GPIOB,             GPIO_PIN_3 }, /* CLK */
    .CS   = {SPI1_CS_GPIO_Port, SPI1_CS_Pin}, /* CS */
};

spi_mt6701_t mt6701 = {
    .hSPI = &spi,
};

static i2c_mst_t i2c_ = {
    .SDA  = {SWI2C_SDA_GPIO_Port, SWI2C_SDA_Pin},
    .SCL  = {SWI2C_SCL_GPIO_Port, SWI2C_SCL_Pin},
    .I2Cx = nullptr,
};

i2c_as5600_t as5600 = {
    .hI2C      = &i2c_,
    .u8SlvAddr = AS5600_ADDRESS_LOW,
};

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
    // SEGGER_RTT_Init();
    cm_backtrace_init("demo", "stm32", "V0.01");
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    HAL_NVIC_DisableIRQ(SysTick_IRQn);
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

    SPI_Master_Init(&spi, 50000, SPI_DUTYCYCLE_50_50, MT6701_SPI_TIMING | SPI_FLAG_FAST_CLOCK_ENABLE | SPI_FLAG_SOFT_CS);
    MT6701_Init(&mt6701);

    I2C_Master_Init(&i2c_, 400000, I2C_DUTYCYCLE_50_50);
    AS5600_Init(&as5600);

#if 0

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

#endif

    // key
    for (int i = 0; i < ARRAY_SIZE(flexbtn); ++i)
    {
        PIN_SetMode(&keys[i], PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
        FlexBtn_Attach(&flexbtn[i]);
    }

    // modbus(rtu)
    RO u8 ucSlaveID[] = {0xAA, 0xBB, 0xCC};
    eMBInit(MB_RTU, D.u16MbSlvId, 1, D.u32MbBaudrate, MB_PAR_EVEN /* D.u16MbParity */);
    //  eMBInit(MB_RTU, 1, 1, 19200, MB_PAR_EVEN /* D.u16MbParity */);
    eMBSetSlaveID(0x34, true, ucSlaveID, ARRAY_SIZE(ucSlaveID));
    eMBEnable();
    MbRtuRun();

    MotCtrlCreat(&sMotCtrl, AXIS_0);

    // hall enc
    HallEnc_Creat(&sHallEnc);
    HallEnc_Init(&sHallEnc);

    // #if __ENC_HALL_SW
    //     HAL_TIM_Base_Start(&TIM_HALL);
    // #elif __ENC_INC_SW
    //     HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2);
    // #endif

    DrvScheCreat();
    DrvScheInit();

    // adc
    HAL_ADCEx_Calibration_Start(&hadc1);
    //  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADConv[0], ARRAY_SIZE(ADConv));
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADConv[0], ARRAY_SIZE(ADConv));

    //	HAL_TIM_Base_Start(&htim1); // TRGO for ADC
    HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);  // TRGO for ADC

    //  AS5600_Init(&as5600);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    axis_e eAxisNo = AXIS_0;

    sHallEnc.u16EncRes = 6 * P(eAxisNo).u16MotPolePairs;

#if 1  // 2804
    P(eAxisNo).u16MotPolePairs = 7;
    P(eAxisNo).u32EncRes       = (1 << 14);
#else  // hall
    P(eAxisNo).u16MotPolePairs = 2;
    P(eAxisNo).u32EncRes       = 6 * P(eAxisNo).u16MotPolePairs;
#endif

    P(eAxisNo).u32EncPosOffset = 748;

#if 1

    P(eAxisNo).u16AppSel = 2;

//	P(eAxisNo).u32CommCmd = 1;
#endif

    while (1)
    {
        // led
        PeriodicTask(UNIT_S, HAL_GPIO_TogglePin(LED2_PIN));
        PeriodicTask(2 * UNIT_S, D._Resv114 = D._Resv113 / 2; D._Resv113 = 0;);  // 中断次数 Hz

        // key
        PeriodicTask(UNIT_S / CONFIG_FLEXBTN_SCAN_FREQ_HZ, FlexBtn_Cycle());

        // modbus
        eMBPoll();

        // HallEnc_Isr(&sHallEnc);

        // P(eAxisNo).u16HallState  = sHallEnc.eHallState;
        // P(eAxisNo).u32EncRes     = sHallEnc.u16EncRes;
        // P(eAxisNo).u32EncPos     = sHallEnc.u16EncPos;
        // P(eAxisNo).s32EncTurns   = sHallEnc.s32EncTurns;
        // P(eAxisNo).s64EncMultPos = sHallEnc.s64EncMultPos;
        // P(eAxisNo).s32DrvSpdFb   = sHallEnc.s16SpdFb;

        DrvScheCycle();

        P(eAxisNo).s32SpdDigRef01 = P(eAxisNo).s64EncMultPos;

        //  PeriodicTask(125 * UNIT_US, DrvScheIsr());

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
    if (flexbtn[BUTTON_NEXT].eEvent == FLEXBTN_EVENT_RELEASE)
    {
        HAL_NVIC_SystemReset();
    }

    if (flexbtn[BUTTON_OKAY].eEvent == FLEXBTN_EVENT_RELEASE)
    {
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
