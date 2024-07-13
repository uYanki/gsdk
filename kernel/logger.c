#include "logger.h"

#if (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_INTEGER)

#include "delay.h"

uint32_t LOG_GetTimestamp(void)
{
    return GetTickUs() / 1000;
}

#elif (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_STRING)

#include <sys/time.h>

char* LOG_GetTimestamp(void)
{
    static char tmp[64];

    struct timeval  tv;
    struct timezone tz;
    struct tm*      tm;

    time_t now;

    gettimeofday(&tv, &tz);
    now = tv.tv_sec;
    tm  = localtime(&now);

#if 0
    sprintf(tmp, "%02d-%02d-%02d %02d:%02d:%02d.%ld", 1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);
#else
    sprintf(tmp, "%02d:%02d:%02d.%03d", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);
#endif

    return tmp;
}

#endif
