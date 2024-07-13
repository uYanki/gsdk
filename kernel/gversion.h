#ifndef __GSDK_VERSION_H__
#define __GSDK_VERSION_H__

#include "gdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/** major version number (X.x.x) */
#define GSDK_VERSION_MAJOR 5
/** minor version number (x.X.x) */
#define GSDK_VERSION_MINOR 4
/** patch version number (x.x.X) */
#define GSDK_VERSION_PATCH 0

/**
 * @brief macro to convert sdk version number into an integer
 * @note  to be used in comparisons, such as GSDK_VERSION >= GSDK_VERSION_VAL(1, 0, 0)
 */
#define GSDK_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

/**
 * @brief current sdk version, as an integer
 * @note  to be used in comparisons, such as GSDK_VERSION >= GSDK_VERSION_VAL(1, 0, 0)
 */
#define GSDK_VERSION GSDK_VERSION_VAL(GSDK_VERSION_MAJOR, GSDK_VERSION_MINOR, GSDK_VERSION_PATCH)

/**
 * @brief firmware build time
 */
typedef struct {
    struct {
        uint16_t u16Year;
        uint8_t  u8Month;
        uint8_t  u8Day;
    } sDate;
    struct {
        uint8_t u8Hour;
        uint8_t u8Minute;
        uint8_t u8Second;
    } sTime;
} build_time_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

build_time_t GetBuildTime(void);

#ifdef __cplusplus
}
#endif

#endif
