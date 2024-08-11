#ifndef __GSDK_ERRNO_H__
#define __GSDK_ERRNO_H__

#include "gdefs.h"
#include "logger.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef int32_t err_t;

/**
 * @brief error code
 * @{
 */

#define ERR_NONE          0  /*!< no error */
#define ERR_FAIL          1  /*!< generic error code indicating failure */
#define ERR_NULLPTR       2  /*!< null pointer */
#define ERR_BUSY          3  /*!< operation has not fully completed */
#define ERR_TIMEOUT       4  /*!< operation timed out */
#define ERR_OVERFLOW      5  /*!< memory overflow */
#define ERR_INVALID_VALUE 6  /*!< invalid value */
#define ERR_CRC_FAIL      7  /*!< crc or checksum was invalid */
#define ERR_NO_MEM        8  /*!< out of memory */
#define ERR_NO_DEVICE     9  /*!< no such device */
#define ERR_NOT_FOUND     10 /*!< requested resource not found */
#define ERR_NOT_SUPPORTED 11 /*!< operation or feature not supported */
#define ERR_NOT_ALLOWED   12 /*!< operation is not allowed */
#define ERR_NOT_CONNECTED 13 /*!< operation is not connected */
#define ERR_NO_MESSAGE    14 /*!< no message */
#define ERR_BAD_MESSAGE   15 /*!< unknown message */
#define ERR_MISMACTH      16 /*!< content mismatch */
#define ERR_ACCESS_DENIED 17 /*!< insufficient permission */

/**
 * @}
 */

/**
 * @brief error check
 * @{
 */

// #ifdef NDEBUG

// #else

#define ERRCHK_ABORT(expr)   \
    do {                     \
        err_t eno = (expr);  \
                             \
        if (eno != ERR_NONE) \
        {                    \
            abort();         \
        }                    \
                             \
    } while (0)

#define ERRCHK_EXIT(expr)    \
    do {                     \
        err_t eno = (expr);  \
                             \
        if (eno != ERR_NONE) \
        {                    \
            return;          \
        }                    \
                             \
    } while (0)

#define ERRCHK_RETURN(expr)  \
    do {                     \
        err_t eno = (expr);  \
                             \
        if (eno != ERR_NONE) \
        {                    \
            return eno;      \
        }                    \
                             \
    } while (0)

#define ERRCHK_JUMP(expr, eno, label) \
    do {                              \
        eno = (expr);                 \
                                      \
        if (eno != ERR_NONE)          \
        {                             \
            goto label;               \
        }                             \
                                      \
    } while (0)

// #endif

/**
 * @}
 */

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
