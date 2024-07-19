#include "motdrv.h"

/**
 *            _____________________________________________
 *            |         __|             __|             __|
 *            |       |/  |           |/  |            |/  |
 *            |    0__|  /_\       1__|  /_\        2__|  /_\
 *            |       |\__|           |\__|            |\__|
 *          * | *         |               |               |
 *       *    |    *      +---------------|---------------|-------u
 *      *     |     *     |               +---------------|-------v
 *      *     |     *     |               |               +-------w
 *       *    |    *      |               |               |
 *          * | *       __|             __|             __|
 *            |       |/  |           |/  |           |/  |
 *            |    3__|  /_\       4__|  /_\       5__|  /_\
 *            |       |\__|           |\__|           |\__|
 *            |___________|_______________|_______________|
 *
 *
 *
 *           ┌─────────┐        ┌───────┐     Va
 *    Vq     │         │ Valpha │       ├──────►
 *    ──────►│ Inverse ├───────►│       │
 *           │         │        │ SVPWM │     Vb
 *    Vd     │         │ Vbeta  │       ├──────►
 *    ──────►│  Park   ├───────►│       │     Vc
 *           │         │        │       ├──────►
 *           └────▲────┘        └───────┘
 *                │
 *                │theta
 */

#if 1

#define Q15Mul(q1, q2) ((q15)((((q31)(q1)) * ((q31)(q2))) >> 15))

#define QMul(q1, q2)   Q15Mul(q1, q2)
#define QMul2(q1)      (q15)((q31)(q1) << 1)
#define QDiv2(q1)      (q15)((q31)(q1) >> 1)

#define IQ15_MAX       32767
#define IQ(f)          (q15)((f32)(f) * (f32)IQ15_MAX)
#define Q2F(q)         (f32)((f32)(q) / (f32)IQ15_MAX)

#else

q15 Q15Mul(q15 q1, q15 q2)
{
    q31 q = (q31)q1 * (q31)q2;

    if (q < 0)
    {
        q = (-q) >> 15;

        return -q;
    }
    else
    {
        q = q >> 15;
    }

    if (q != _Q15Mul(q1, q2))
    {
        return 32767;
    }

    return q;
}

#endif

/**
 * @note d = alpha * cos(theta) + beta  * sin(theta)
 *       q = beta  * cos(theta) - alpha * sin(theta)
 */
void MC_Park(motdrv_t* p)
{
    p->Id = Q15Mul(p->Ialpha, p->Cos) + Q15Mul(p->Ibeta, p->Sin);
    p->Iq = Q15Mul(p->Ibeta, p->Cos) - Q15Mul(p->Ialpha, p->Sin);
}

/**
 * @note alpha = d * cos(theta) - q * sin(theta)
 *       beta  = d * sin(theta) + q * cos(theta)
 */
void MC_InvPark(motdrv_t* p)
{
    p->Ualpha = Q15Mul(p->Ud, p->Cos) - Q15Mul(p->Uq, p->Sin);
    p->Ubeta  = Q15Mul(p->Ud, p->Sin) + Q15Mul(p->Uq, p->Cos);
}

/**
 * @note alpha = a
 *       beta  = b * 1 / sqrt(3)  - c * 1 / sqrt(3)
 * @note alpha = a
 *       beta  = a / sqrt(3)  + b * 2 / sqrt(3)
 */
void MC_Clark(motdrv_t* p)
{
#if 0  // 3 currents
    p->Ialpha = p->Ia;
    p->Ibeta  = Q15Mul(p->Ib - p->Ic, Q15(M_INVSQRT3));
#else  // 2 currents
    p->Ialpha = p->Ia;
    p->Ibeta  = Q15Mul(p->Ia + (p->Ib << 1), Q15(M_INVSQRT3));
#endif
}

/**
 * @note a = alpha
 *       b = -0.5 * alpha + sqrt(3) / 2 * beta
 *       c = -0.5 * alpha - sqrt(3) / 2 * beta
 */
void MC_InvClark(motdrv_t* p)
{
    p->Ua = p->Ualpha;
    p->Ub = -(p->Ualpha >> 1) + Q15Mul(p->Ubeta, Q15(M_SQRT3_2));
    p->Uc = -(p->Ualpha >> 1) - Q15Mul(p->Ubeta, Q15(M_SQRT3_2));
}

/**
 * @brief rebuild three-phase current
 * @note  电流方向：流入绕组为正，流出绕组为负(未适配)
 */
void MC_RebuildCurrent(motdrv_t* p, q15 Ix[])
{
    /**
     * @note
     *
     *  Ia = Im * cos(θ)
     *  Ib = Im * cos(θ-2π/3)
     *  Ic = Im * cos(θ-4π/3)
     *     = Im * cos(θ+2π/3)
     *
     *  Ia + Ib + Ic = 0 (基尔霍夫定律)
     *
     */

    switch (p->eCurSamp)
    {
        case CUR_SAMP_UV_LINE:  // Iu,Iv
        {
            p->Ia = Ix[0] - p->IaZero;
            p->Ib = Ix[1] - p->IbZero;
            p->Ic = -(p->Ia + p->Ib);
            break;
        }
        case CUR_SAMP_VW_LINE:  // Iv,Iw
        {
            p->Ib = Ix[0] - p->IbZero;
            p->Ic = Ix[1] - p->IcZero;
            p->Ia = -(p->Ib + p->Ic);
            break;
        }
        case CUR_SAMP_UVW_LINE:  // Iu,Iv,Iw
        {
            p->Ia = Ix[0] - p->IaZero;
            p->Ib = Ix[1] - p->IbZero;
            p->Ic = Ix[2] - p->IcZero;
            break;
        }

        case CUR_SAMP_S_DOWN_BRG:  // I1,I2
        {
            switch (p->eSector)
            {
                case SECTOR_1:
                {
                    p->Ia = +(Ix[0] - p->IxZero) << 1;
                    p->Ic = -(Ix[1] - p->IxZero) << 1;
                    p->Ib = -(p->Ia + p->Ic);
                    break;
                }
                case SECTOR_2:
                {
                    p->Ib = +(Ix[0] - p->IxZero) << 1;
                    p->Ic = -(Ix[1] - p->IxZero) << 1;
                    p->Ia = -(p->Ib + p->Ic);
                    break;
                }
                case SECTOR_3:
                {
                    p->Ib = +(Ix[0] - p->IxZero) << 1;
                    p->Ia = -(Ix[1] - p->IxZero) << 1;
                    p->Ic = -(p->Ia + p->Ib);
                    break;
                }
                case SECTOR_4:
                {
                    p->Ic = +(Ix[0] - p->IxZero) << 1;
                    p->Ia = -(Ix[1] - p->IxZero) << 1;
                    p->Ib = -(p->Ia + p->Ic);
                    break;
                }
                case SECTOR_5:
                {
                    p->Ic = +(Ix[0] - p->IxZero) << 1;
                    p->Ib = -(Ix[1] - p->IxZero) << 1;
                    p->Ia = -(p->Ib + p->Ic);
                    break;
                }
                case SECTOR_6:
                {
                    p->Ia = +(Ix[0] - p->IxZero) << 1;
                    p->Ib = -(Ix[1] - p->IxZero) << 1;
                    p->Ic = -(p->Ia + p->Ib);
                    break;
                }

                default: break;
            }

            break;
        }

        case CUR_SAMP_UV_DOWN_BRG:  // Iu,Iv
        {
            p->Ia = (p->IaZero - Ix[0]) << 1;
            p->Ib = (p->IbZero - Ix[1]) << 1;
            p->Ic = -p->Ia - p->Ib;
            break;
        }
        case CUR_SAMP_VW_DOWN_BRG:  // Iv,Iw
        {
            p->Ib = (p->IbZero - Ix[0]) << 1;
            p->Ic = (p->IcZero - Ix[1]) << 1;
            p->Ia = -p->Ib - p->Ic;
            break;
        }
        case CUR_SAMP_UW_DOWN_BRG:  // Iu,Iw
        {
            p->Ia = (p->IaZero - Ix[0]) << 1;
            p->Ic = (p->IcZero - Ix[1]) << 1;
            p->Ib = -p->Ia - p->Ic;
            break;
        }

        case CUR_SAMP_UVW_DOWN_BRG:  // Iu,Iv,Iw
        {
            p->Ia = Ix[0] - p->IaZero;
            p->Ib = Ix[1] - p->IbZero;
            p->Ic = Ix[2] - p->IcZero;
            break;
        }

        default: break;
    }
}

/**
 * @brief
 */
void MC_SinCos(motdrv_t* p)
{
    // clang-format off
    static const u16 u16SinTbl[] = {
        0x0000, 0x00C9, 0x0192, 0x025B, 0x0324, 0x03ED, 0x04B6, 0x057F, 0x0648, 0x0711, 0x07D9, 0x08A2, 0x096A, 0x0A33, 0x0AFB, 0x0BC4, 0x0C8C, 0x0D54, 0x0E1C, 0x0EE3, 0x0FAB, 0x1072, 0x113A, 0x1201,
        0x12C8, 0x138F, 0x1455, 0x151C, 0x15E2, 0x16A8, 0x176E, 0x1833, 0x18F9, 0x19BE, 0x1A82, 0x1B47, 0x1C0B, 0x1CCF, 0x1D93, 0x1E57, 0x1F1A, 0x1FDD, 0x209F, 0x2161, 0x2223, 0x22E5, 0x23A6, 0x2467,
        0x2528, 0x25E8, 0x26A8, 0x2767, 0x2826, 0x28E5, 0x29A3, 0x2A61, 0x2B1F, 0x2BDC, 0x2C99, 0x2D55, 0x2E11, 0x2ECC, 0x2F87, 0x3041, 0x30FB, 0x31B5, 0x326E, 0x3326, 0x33DF, 0x3496, 0x354D, 0x3604,
        0x36BA, 0x376F, 0x3824, 0x38D9, 0x398C, 0x3A40, 0x3AF2, 0x3BA5, 0x3C56, 0x3D07, 0x3DB8, 0x3E68, 0x3F17, 0x3FC5, 0x4073, 0x4121, 0x41CE, 0x427A, 0x4325, 0x43D0, 0x447A, 0x4524, 0x45CD, 0x4675,
        0x471C, 0x47C3, 0x4869, 0x490F, 0x49B4, 0x4A58, 0x4AFB, 0x4B9D, 0x4C3F, 0x4CE0, 0x4D81, 0x4E20, 0x4EBF, 0x4F5D, 0x4FFB, 0x5097, 0x5133, 0x51CE, 0x5268, 0x5302, 0x539B, 0x5432, 0x54C9, 0x5560,
        0x55F5, 0x568A, 0x571D, 0x57B0, 0x5842, 0x58D3, 0x5964, 0x59F3, 0x5A82, 0x5B0F, 0x5B9C, 0x5C28, 0x5CB3, 0x5D3E, 0x5DC7, 0x5E4F, 0x5ED7, 0x5F5D, 0x5FE3, 0x6068, 0x60EB, 0x616E, 0x61F0, 0x6271,
        0x62F1, 0x6370, 0x63EE, 0x646C, 0x64E8, 0x6563, 0x65DD, 0x6656, 0x66CF, 0x6746, 0x67BC, 0x6832, 0x68A6, 0x6919, 0x698B, 0x69FD, 0x6A6D, 0x6ADC, 0x6B4A, 0x6BB7, 0x6C23, 0x6C8E, 0x6CF8, 0x6D61,
        0x6DC9, 0x6E30, 0x6E96, 0x6EFB, 0x6F5E, 0x6FC1, 0x7022, 0x7083, 0x70E2, 0x7140, 0x719D, 0x71F9, 0x7254, 0x72AE, 0x7307, 0x735E, 0x73B5, 0x740A, 0x745F, 0x74B2, 0x7504, 0x7555, 0x75A5, 0x75F3,
        0x7641, 0x768D, 0x76D8, 0x7722, 0x776B, 0x77B3, 0x77FA, 0x783F, 0x7884, 0x78C7, 0x7909, 0x794A, 0x7989, 0x79C8, 0x7A05, 0x7A41, 0x7A7C, 0x7AB6, 0x7AEE, 0x7B26, 0x7B5C, 0x7B91, 0x7BC5, 0x7BF8,
        0x7C29, 0x7C59, 0x7C88, 0x7CB6, 0x7CE3, 0x7D0E, 0x7D39, 0x7D62, 0x7D89, 0x7DB0, 0x7DD5, 0x7DFA, 0x7E1D, 0x7E3E, 0x7E5F, 0x7E7E, 0x7E9C, 0x7EB9, 0x7ED5, 0x7EEF, 0x7F09, 0x7F21, 0x7F37, 0x7F4D,
        0x7F61, 0x7F74, 0x7F86, 0x7F97, 0x7FA6, 0x7FB4, 0x7FC1, 0x7FCD, 0x7FD8, 0x7FE1, 0x7FE9, 0x7FF0, 0x7FF5, 0x7FF9, 0x7FFD, 0x7FFE,
    };
    // clang-format on

    /* 10 bit index computation  */
    /*   u16Index = (u16)(s16Angle + 32768); */
    /*   u16Index /= 64;     */

    u16 u16Index = (u16)(p->u16ElecAngle + 32768) >> 6;

    switch (u16Index & 0x0300)
    {
        case 0x0200:  // 0~90
        {
            p->Sin = u16SinTbl[(u8)(u16Index)];
            p->Cos = u16SinTbl[(u8)(0xFF - (u8)(u16Index))];
            break;
        }
        case 0x0300:  // 90~180
        {
            p->Sin = u16SinTbl[(u8)(0xFF - (u8)(u16Index))];
            p->Cos = -u16SinTbl[(u8)(u16Index)];
            break;
        }
        case 0x0000:  // 180~270
        {
            p->Sin = -u16SinTbl[(u8)(u16Index)];
            p->Cos = -u16SinTbl[(u8)(0xFF - (u8)(u16Index))];
            break;
        }
        case 0x0100:  // 270~360
        {
            p->Sin = -u16SinTbl[(u8)(0xFF - (u8)(u16Index))];
            p->Cos = u16SinTbl[(u8)(u16Index)];
            break;
        }
    }
}

void MC_MinMaxU(motdrv_t* p)
{
    if (p->Ua >= p->Ub)
    {
        if (p->Ua >= p->Uc)
        {
            if (p->Ub >= p->Uc)
            {
                p->eSector = SECTOR_1;
                p->Umax    = p->Ua;
                p->Umid    = p->Ub;
                p->Umin    = p->Uc;
            }
            else
            {
                p->eSector = SECTOR_6;
                p->Umax    = p->Ua;
                p->Umid    = p->Uc;
                p->Umin    = p->Ub;
            }
        }
        else
        {
            p->eSector = SECTOR_5;
            p->Umax    = p->Uc;
            p->Umid    = p->Ua;
            p->Umin    = p->Ub;
        }
    }
    else
    {
        if (p->Ua < p->Uc)
        {
            if (p->Ub < p->Uc)
            {
                p->eSector = SECTOR_4;
                p->Umax    = p->Uc;
                p->Umid    = p->Ub;
                p->Umin    = p->Ua;
            }
            else
            {
                p->eSector = SECTOR_3;
                p->Umax    = p->Ub;
                p->Umid    = p->Uc;
                p->Umin    = p->Ua;
            }
        }
        else
        {
            p->eSector = SECTOR_2;
            p->Umax    = p->Ub;
            p->Umid    = p->Ua;
            p->Umin    = p->Uc;
        }
    }
}

void MC_OverMod(motdrv_t* p)
{}

/**
 * @brief circle limitation
 */
void MC_CirLim(motdrv_t* p)
{
    u32 UdSq  = (u32)((q31)p->Ud * (q31)p->Ud);
    u32 UqSq  = (u32)((q31)p->Uq * (q31)p->Uq);
    u32 Q15Sq = (u32)((q31)Q15_MAX * (q31)Q15_MAX);

    if (Q15Sq < (UdSq + UqSq))
    {
        p->Uq = (s16)sqrt(Q15Sq - UdSq);
    }
}

void SVPWMx(motdrv_t* p)
{
    u16 hPWM_PERIOD = p->DutyMax;
    s16 nAlph       = p->Ualpha;
    s16 nBeta       = -p->Ubeta;

    s16 u, v, w;
    s32 uvw_average;
    s16 uvw_min;
    s16 uvw_max;

    u = nAlph;
    v = (s16)(-(((s32)nBeta * 28378 + (s32)nAlph * 16384)) >> 15);
    w = -u - v;

    uvw_min     = u < v ? u < w ? u : w : v < w ? v
                                                : w;
    uvw_max     = u > v ? u > w ? u : w : v > w ? v
                                                : w;
    uvw_average = (uvw_max + uvw_min) >> 1;

    u = (u - uvw_average) >> 1;
    v = (v - uvw_average) >> 1;
    w = (w - uvw_average) >> 1;

    u += 16384;
    v += 16384;
    w += 16384;

    p->Ta = Q15Mul(u, hPWM_PERIOD);
    p->Tb = Q15Mul(v, hPWM_PERIOD);
    p->Tc = Q15Mul(w, hPWM_PERIOD);
}

/**
 * @brief zero-sequence injection 零序分量注入
 *
 * @note 三相对称电力系统中
 *
 *  正序：三相量大小相等，互差120°。（ABCABCABC...）
 *  负序：三相量大小相等，互差120°。 (ACBACBACB..., 正序的任意两相交换就是负序)
 *  零序：三相量大小相等，相位一致。
 *
 *      单相接地故障           ——> 系统有正序负序和零序分量
 *      两相短路故障           ——> 系统有正序和负序分量
 *      两相短路接地故障       ——> 系统有正序负序和零序分量
 *      三相短路故障和正常运行  ——> 系统里面是正序分量
 *
 * 正序分量 positive sequence component 对于三相系统三个大小相等的分量，且在相位上依次有前一个分量超前后一个分量120度的分量组。
 * 负序分量（negative sequence component），是对于三相系统三个大小相等的分量，且在相位上依次有前一个分量滞后一个分量120°的分量组。
 *
 */

void MC_ZeroSeqInj(motdrv_t* p)
{
    q15 U0 = -QDiv2(p->Umax + p->Umin);  // V0=-0.5*(Vmax+Vmin)

    float32_t m = 2 / M_SQRT3;

    u16 u16DutyHalf = p->DutyMax / 2;

    p->Ta = m * Q15Mul(p->Ua + U0, u16DutyHalf) + u16DutyHalf;
    p->Tb = m * Q15Mul(p->Ub + U0, u16DutyHalf) + u16DutyHalf;
    p->Tc = m * Q15Mul(p->Uc + U0, u16DutyHalf) + u16DutyHalf;
}

void MC_SVPWM5(motdrv_t* p)
{}

#if 0

#define MAX_MODULATION_91_PER_CENT  0
#define MAX_MODULATION_100_PER_CENT 1
#define MAX_MODULATION              MAX_MODULATION_100_PER_CENT

#if (MAX_MODULATION == MAX_MODULATION_100_PER_CENT)
#define MAX_MODULE  32767  // root(Vd^2+Vq^2) <= MAX_MODULE = 32767*100%
#define START_INDEX 63

static const u16 circle_limit_table[65] =
    {
        32767, 32390, 32146, 31907, 31673, 31444, 31220, 31001, 30787, 30577, 30371,
        30169, 29971, 29777, 29587, 29400, 29217, 29037, 28861, 28687, 28517,
        28350, 28185, 28024, 27865, 27709, 27555, 27404, 27256, 27110, 26966,
        26824, 26685, 26548, 26413, 26280, 26149, 26019, 25892, 25767, 25643,
        25521, 25401, 25283, 25166, 25051, 24937, 24825, 24715, 24606, 24498,
        24392, 24287, 24183, 24081, 23980, 23880, 23782, 23684, 23588, 23493,
        23400, 23307, 23215, 23125};

#elif (MAX_MODULATION == MAX_MODULATION_91_PER_CENT)
#define MAX_MODULE  29818  // root(Vd^2+Vq^2) <= MAX_MODULE = 32767*91%
#define START_INDEX 52

static const u16 circle_limit_table[76] =
    {
        32588, 32411, 32066, 31732, 31569, 31250, 30940, 30789, 30492, 30205,
        29925, 29788, 29519, 29258, 29130, 28879, 28634, 28395, 28278, 28048,
        27823, 27713, 27497, 27285, 27181, 26977, 26777, 26581, 26485, 26296,
        26110, 26019, 25840, 25664, 25492, 25407, 25239, 25076, 24995, 24835,
        24679, 24602, 24450, 24301, 24155, 24082, 23940, 23800, 23731, 23594,
        23460, 23328, 23263, 23135, 23008, 22946, 22822, 22701, 22641, 22522,
        22406, 22291, 22234, 22122, 22011, 21956, 21848, 21741, 21636, 21584,
        21482, 21380, 21330, 21231, 21133, 21037};
#endif

void RevPark_Circle_Limitation(void)
{
    s32 temp;

    temp = MotorFoc.VdRef * MotorFoc.VdRef + MotorFoc.VqRef * MotorFoc.VqRef;  // min value 0, max value 2*32767*32767

    if (temp > (u32)((MAX_MODULE * MAX_MODULE)))  // (Vd^2+Vq^2) > MAX_MODULE^2 ?
    {
        u16 index;

        temp = temp >> 24;    // min value START_INDEX, max value 127
        temp -= START_INDEX;  // min value 0, max value 127 - START_INDEX
        index = circle_limit_table[(u8)temp];

        temp           = (s16)MotorFoc.VqRef * (u16)(index);
        MotorFoc.VqRef = (s16)(temp >> 15);

        temp           = (s16)MotorFoc.VdRef * (u16)(index);
        MotorFoc.VdRef = (s16)(temp >> 15);
    }
}

#endif

/***************************************************************************************************
//	Svpwm_xy：普通七段法SVPWM在静止直角坐标系下的实现

***************************************************************************************************/

u8  bSector2;
u16 hTimePhA = 0, hTimePhB = 0, hTimePhC = 0;

#define SQRT_3     1.732051
#define PWM_PERIOD pfoc->DutyMax
#define T          (PWM_PERIOD * 4)  // 扩大4倍
#define T_SQRT3    (u16)(T * SQRT_3)

/***************SVM 5段 7段**************/
#define SECTION_5 0
#define SECTION_7 1

void svm(motdrv_t* pfoc)
{
    s32 wX, wY, wZ, wUAlpha, wUBeta;

    // calc_ref_vec(pfoc);		//测试用，监测VR1,VR2,VR3

    wUAlpha = pfoc->Ualpha * T_SQRT3;
    wUBeta  = -(pfoc->Ubeta * T);

    wX = wUBeta;
    wY = (wUBeta + wUAlpha) >> 1;
    wZ = (wUBeta - wUAlpha) >> 1;

    // Sector calculation from wX, wY, wZ
    if (wY < 0)
    {
        if (wZ < 0)
        {
            pfoc->eSector = SECTOR_5;
        }
        else if (wX <= 0)
        {
            pfoc->eSector = SECTOR_4;
        }
        else  // wX > 0
        {
            pfoc->eSector = SECTOR_3;
        }
    }
    else  // wY > 0
    {
        if (wZ >= 0)
        {
            pfoc->eSector = SECTOR_2;
        }
        else if (wX <= 0)
        {
            pfoc->eSector = SECTOR_6;
        }
        else  // wX > 0
        {
            pfoc->eSector = SECTOR_1;
        }
    }

    uint8_t x_segment = SECTION_7;

    /* Duty cycles computation */
    if (x_segment == SECTION_7)
    {
        switch (pfoc->eSector)
        {
            case SECTOR_1:  // 时间区间是0-4-6-7-7-6-4-0
            case SECTOR_4:  // 时间区间是0-1-3-7-7-3-1-0
                hTimePhA = (T >> 3) + ((((T + wX) - wZ) >> 1) >> 17);
                hTimePhB = hTimePhA + (wZ >> 17);
                hTimePhC = hTimePhB - (wX >> 17);

                break;
            case SECTOR_2:
            case SECTOR_5:
                hTimePhA = (T >> 3) + ((((T + wY) - wZ) >> 1) >> 17);
                hTimePhB = hTimePhA + (wZ >> 17);
                hTimePhC = hTimePhA - (wY >> 17);
                break;

            case SECTOR_3:
            case SECTOR_6:
                hTimePhA = (T >> 3) + ((((T - wX) + wY) >> 1) >> 17);
                hTimePhC = hTimePhA - (wY >> 17);
                hTimePhB = hTimePhC + (wX >> 17);
                break;
            default:
                break;
        }
    }
    else if (x_segment == SECTION_5)
    {
        switch (pfoc->eSector)
        {
            case SECTOR_1:  // 时间区间是4-6-7-7-6-4
                hTimePhA = PWM_PERIOD;
                hTimePhB = hTimePhC + (wX >> 17);
                hTimePhC = PWM_PERIOD - ((wX - wZ) >> 17);  // 2^17=(2^15)*(2^2)
                break;
            case SECTOR_2:  // 时间区间2-6-7-7-6-2
                hTimePhA = hTimePhC + (wY >> 17);
                hTimePhB = PWM_PERIOD;
                hTimePhC = PWM_PERIOD - ((wY + wZ) >> 17);
                break;
            case SECTOR_3:  // 时间区间2-3-7-7-3-2
                hTimePhA = PWM_PERIOD - ((wX - wY) >> 17);
                hTimePhB = PWM_PERIOD;
                hTimePhC = hTimePhA - (wY >> 17);
                break;
            case SECTOR_4:  // 时间区间是1-3-7-7-3-1
                hTimePhA = PWM_PERIOD - ((wZ - wX) >> 17);
                hTimePhB = hTimePhA + (wZ >> 17);
                hTimePhC = PWM_PERIOD;
                break;
            case SECTOR_5:  // 时间区间1-5-7-7-5-1
                hTimePhA = hTimePhB - (wZ >> 17);
                hTimePhB = PWM_PERIOD + ((wY + wZ) >> 17);
                hTimePhC = PWM_PERIOD;
                break;
            case SECTOR_6:  // 时间区间4-5-7-7-5-4
                hTimePhA = PWM_PERIOD;
                hTimePhB = PWM_PERIOD - ((wY - wX) >> 17);
                hTimePhC = hTimePhB - (wX >> 17);
                break;
            default:
                break;
        }
    }

    pfoc->Ta = (PWM_PERIOD - hTimePhA);  // U
    pfoc->Tb = (PWM_PERIOD - hTimePhC);  // V
    pfoc->Tc = (PWM_PERIOD - hTimePhB);  // W
}

///

typedef enum {
    SVM_COM_C = 0,  //!< SVPWM common SVM mode
    SVM_MIN_C = 1,  //!< DPWM minimum SVM mode
    SVM_MAX_C = 2,  //!< DPWM maximum SVM
    SVM_DQ_S  = 3   //!< SVPWM standard DQ mode
} svm_mode_e;

/**
 * @brief Space Vector Generator (SVGEN) calculation.
 *        Implements a SVM that saturates at the level of Max Modulation.
 *
 * @note matlab: https://ww2.mathworks.cn/help/mcb/ref/pwmreferencegenerator.html
 *       ti: C2000Ware_MotorControl_SDK_5_01_00_00\libraries\transforms\svgen\include\svgen.h
 *
 * @param[in] handle    The space vector generator (SVGEN) handle
 * @param[in] pVab_V    The pointer to the alpha/beta voltages, V
 * @param[in] pVabc_pu  The pointer to the three phase voltages, pu
 *
 */
void SVGEN_run(motdrv_t* p, uint8_t eMode)
{
    float32_t oneOverDcBus_invV__ = 1;  //!< The inverse DC bus voltage value, 1/V

    float32_t pVab_V_value[2] = {p->Ualpha, p->Ubeta};

    float32_t Vmax_pu = 0, Vmin_pu = 0, Vcom_pu;
    float32_t oneOverDcBus_invV = oneOverDcBus_invV__;

    float32_t Valpha_pu = pVab_V_value[0];  // * oneOverDcBus_invV;
    float32_t Vbeta_pu  = pVab_V_value[1];  // * oneOverDcBus_invV;

    float32_t Va_tmp = (float32_t)(0.5f) * Valpha_pu;
    float32_t Vb_tmp = M_SQRT3_2 * Vbeta_pu;

    float32_t Va_pu = Valpha_pu;

    // -0.5*Valpha + sqrt(3)/2 * Vbeta
    float32_t Vb_pu = -Va_tmp + Vb_tmp;

    // -0.5*Valpha - sqrt(3)/2 * Vbeta
    float32_t Vc_pu = -Va_tmp - Vb_tmp;

    // Find Vmax and Vmin
    Vmax_pu = MAX3(Va_pu, Vb_pu, Vc_pu);
    Vmin_pu = MIN3(Va_pu, Vb_pu, Vc_pu);

    float32_t value[3];

    if (eMode == SVM_COM_C)  // ok
    {
        // Compute Vcom = 0.5*(Vmax+Vmin)
        Vcom_pu = 0.5f * (Vmax_pu + Vmin_pu);

        // Subtract common-mode term to achieve SV modulation
        value[0] = (Va_pu - Vcom_pu);
        value[1] = (Vb_pu - Vcom_pu);
        value[2] = (Vc_pu - Vcom_pu);
    }
    else if (eMode == SVM_MIN_C)  // 噪音
    {
        value[0] = (Va_pu - Vmin_pu) - IQ(0.5f);
        value[1] = (Vb_pu - Vmin_pu) - IQ(0.5f);
        value[2] = (Vc_pu - Vmin_pu) - IQ(0.5f);
    }
    else if (eMode == SVM_MAX_C)  // 噪音
    {
        value[0] = (Va_pu - Vmax_pu) + IQ(0.5f);
        value[1] = (Vb_pu - Vmax_pu) + IQ(0.5f);
        value[2] = (Vc_pu - Vmax_pu) + IQ(0.5f);
    }

    float32_t m = 2 / SQRT_3;

    p->Ta = m * QMul(-value[0], p->DutyMax / 2) + p->DutyMax / 2;
    p->Tb = m * QMul(-value[1], p->DutyMax / 2) + p->DutyMax / 2;
    p->Tc = m * QMul(-value[2], p->DutyMax / 2) + p->DutyMax / 2;
}

void MC_SVGEN5() {}

#if 0

/**
 * @brief svm Space vector modulation. Magnitude must not be larger than sqrt(3)/2, or 0.866 to avoid overmodulation.
 *        See https://github.com/vedderb/bldc/pull/372#issuecomment-962499623 for a full description.
 * @param alpha voltage
 * @param beta Park transformed and normalized voltage
 * @param PWMFullDutyCycle is the peak value of the PWM counter.
 * @param tAout PWM duty cycle phase A (0 = off all of the time, PWMFullDutyCycle = on all of the time)
 * @param tBout PWM duty cycle phase B
 * @param tCout PWM duty cycle phase C
 */
void foc_svm(float alpha, float beta, uint32_t PWMFullDutyCycle,
				uint32_t* tAout, uint32_t* tBout, uint32_t* tCout, uint32_t *svm_sector) {
	uint32_t sector;

	if (beta >= 0.0f) {
		if (alpha >= 0.0f) {
			//quadrant I
			if (ONE_BY_SQRT3 * beta > alpha) {
				sector = 2;
			} else {
				sector = 1;
			}
		} else {
			//quadrant II
			if (-ONE_BY_SQRT3 * beta > alpha) {
				sector = 3;
			} else {
				sector = 2;
			}
		}
	} else {
		if (alpha >= 0.0f) {
			//quadrant IV5
			if (-ONE_BY_SQRT3 * beta > alpha) {
				sector = 5;
			} else {
				sector = 6;
			}
		} else {
			//quadrant III
			if (ONE_BY_SQRT3 * beta > alpha) {
				sector = 4;
			} else {
				sector = 5;
			}
		}
	}

	// PWM timings
	uint32_t tA, tB, tC;

	switch (sector) {

	// sector 1-2
	case 1: {
		// Vector on-times
		uint32_t t1 = (alpha - ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;
		uint32_t t2 = (TWO_BY_SQRT3 * beta) * PWMFullDutyCycle;

		// PWM timings
		tA = (PWMFullDutyCycle + t1 + t2) / 2;
		tB = tA - t1;
		tC = tB - t2;

		break;
	}

	// sector 2-3
	case 2: {
		// Vector on-times
		uint32_t t2 = (alpha + ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;
		uint32_t t3 = (-alpha + ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;

		// PWM timings
		tB = (PWMFullDutyCycle + t2 + t3) / 2;
		tA = tB - t3;
		tC = tA - t2;

		break;
	}

	// sector 3-4
	case 3: {
		// Vector on-times
		uint32_t t3 = (TWO_BY_SQRT3 * beta) * PWMFullDutyCycle;
		uint32_t t4 = (-alpha - ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;

		// PWM timings
		tB = (PWMFullDutyCycle + t3 + t4) / 2;
		tC = tB - t3;
		tA = tC - t4;

		break;
	}

	// sector 4-5
	case 4: {
		// Vector on-times
		uint32_t t4 = (-alpha + ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;
		uint32_t t5 = (-TWO_BY_SQRT3 * beta) * PWMFullDutyCycle;

		// PWM timings
		tC = (PWMFullDutyCycle + t4 + t5) / 2;
		tB = tC - t5;
		tA = tB - t4;

		break;
	}

	// sector 5-6
	case 5: {
		// Vector on-times
		uint32_t t5 = (-alpha - ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;
		uint32_t t6 = (alpha - ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;

		// PWM timings
		tC = (PWMFullDutyCycle + t5 + t6) / 2;
		tA = tC - t5;
		tB = tA - t6;

		break;
	}

	// sector 6-1
	case 6: {
		// Vector on-times
		uint32_t t6 = (-TWO_BY_SQRT3 * beta) * PWMFullDutyCycle;
		uint32_t t1 = (alpha + ONE_BY_SQRT3 * beta) * PWMFullDutyCycle;

		// PWM timings
		tA = (PWMFullDutyCycle + t6 + t1) / 2;
		tC = tA - t1;
		tB = tC - t6;

		break;
	}
	}

	*tAout = tA;
	*tBout = tB;
	*tCout = tC;
	*svm_sector = sector;
}

#endif
