#include "timestamp.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "timestamp"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#define DayToSecond     86400 /* 一天等于86400秒 */
#define HourToSecond    3600  /* 一小时等于3600秒 */
#define MinuteToSecond  60    /* 一分钟等于60秒 */
#define LeapYear        366   /* 闰年有366天 */
#define CommonYear      365   /* 平年有365天 */
#define LeapFeb         29    /* 闰年的2月 */
#define CommonFeb       28    /* 平年的2月 */

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static const u8 m_cau8MonthDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* 平年的月份日期表 */

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool IsLeapYear(u32 year)
{
    if (((0 == year % 4) && (year % 100 != 0)) || (0 == year % 400))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 时间戳转UTC时间
 */
struct tm TS2UTC(time_t time) {
    // 1970-01-01: 星期四

    struct tm sTime = {0};

    u32 u32Sec = time;
    u32 u32Day = u32Sec / DayToSecond; /* 总天数 */

    sTime.tm_year = 1970;                 /* 年份 */
    sTime.tm_wday = (3 + u32Day) % 7 + 1; /* 星期 */

    while (u32Day >= CommonYear)
    {
        if (IsLeapYear(sTime.tm_year))
        {
            /* 闰年 */
            if (u32Day >= LeapYear)
            {
                u32Day -= LeapYear;
            }
            else
            {
                break;
            }
        }
        else
        {
            /* 平年 */
            u32Day -= CommonYear;
        }

        sTime.tm_year++; /* 年份 */
    }

    while (u32Day >= CommonFeb)
    {
        if ((sTime.tm_mon == 1) && IsLeapYear(sTime.tm_year))
        {
            /* 闰年2月 */
            if (u32Day >= LeapFeb)
            {
                sTime.tm_yday += LeapFeb;
                u32Day -= LeapFeb;
            }
            else
            {
                break;
            }
        }
        else
        {
            /* 平年 */
            if (u32Day >= m_cau8MonthDay[sTime.tm_mon])
            {
                sTime.tm_yday += m_cau8MonthDay[sTime.tm_mon];
                u32Day -= m_cau8MonthDay[sTime.tm_mon];
            }
            else
            {
                break;
            }
        }

        sTime.tm_mon++;
    }

    sTime.tm_mon += 1;              /* 月份 */
    sTime.tm_mday = u32Day + 1;     /* 日期(月) */
    sTime.tm_yday += sTime.tm_mday; /* 日期(年) */

    u32Day        = u32Sec % DayToSecond;                     /* 秒 */
    sTime.tm_hour = u32Day / HourToSecond;                    /* 小时 */
    sTime.tm_min  = (u32Day % HourToSecond) / MinuteToSecond; /* 分钟 */
    sTime.tm_sec  = (u32Day % HourToSecond) % MinuteToSecond; /* 秒钟 */

    return sTime;
}

/**
 * @brief 时间戳转北京时间
 */
struct tm TS2BJ(time_t time) 
{
    return TS2UTC(time + 8 * HourToSecond);
}

/**
 * @brief 时间戳转换成秒
 */
time_t UTC2TS(struct tm* pTime)
{
    u32 second_cnt;
    u32 u32Day = 0;
    u32 u32Hour;
    u32 u32Minute;
    u32 u32Second;
    u32 i;

    for (i = 1970; i < pTime->tm_year; i++)
    {
        if (IsLeapYear(i))
        {
            u32Day += 366;
        }
        else
        {
            u32Day += 365;
        }
    }

    for (i = 0; i < pTime->tm_mon - 1; i++)
    {
        u32Day += m_cau8MonthDay[i];
    }

    if (IsLeapYear(pTime->tm_year))
    {
        u32Day += 1;  // 闰年
    }

    u32Day += pTime->tm_mday - 1;
    u32Hour   = u32Day * 24 + pTime->tm_hour;
    u32Minute = u32Hour * 60 + pTime->tm_min;
    u32Second = u32Minute * 60 + pTime->tm_sec;

    return u32Second;
}

/**
 * @brief 北京时间转时间戳
 */
time_t BJ2TS(struct tm* pTime)
{
    return UTC2TS(pTime) - 8 * HourToSecond;
}

void ShowTime(struct tm* pTime)
{
    printf("time %02d-%02d-%02d %02d:%02d:%02d (yday %d, wday %d)\n",
           pTime->tm_year,
           pTime->tm_mon,
           pTime->tm_mday,
           pTime->tm_hour,
           pTime->tm_min,
           pTime->tm_sec,
           pTime->tm_yday,
           pTime->tm_wday);
}

#if CONFIG_DEMOS_SW

void Timestamp_Test(void)
{
    // https://www.timetool.cn/timestamp/

    struct tm Time = {
        .tm_year = 2024,
        .tm_mon  = 9,
        .tm_mday = 1,
        .tm_hour = 2,
        .tm_min  = 6,
        .tm_sec  = 19,
    };

    u32 ts = BJ2TS(&Time);
    LOGI("ts %d (%d)", ts, ts - 1725127579);

    Time = TS2BJ(-11236); // 1970-01-01 04:52:44
    LOGI("time %02d-%02d-%02d %02d:%02d:%02d (yday %d, wday %d)\n",
         Time.tm_year,
         Time.tm_mon,
         Time.tm_mday,
         Time.tm_hour,
         Time.tm_min,
         Time.tm_sec,
         Time.tm_yday,
         Time.tm_wday);
}

#endif
