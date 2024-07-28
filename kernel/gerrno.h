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

#define ERR_NONE          0 /*!< no error */
#define ERR_FAIL          1 /*!< generic error code indicating failure */

#define ERR_NO_MEM        100 /*!< out of memory */
#define ERR_NULLPTR       101 /*!< null pointer */
#define ERR_INVALID_VALUE 102 /*!< invalid value */
#define ERR_INVALID_STATE 103 /*!< invalid state */
#define ERR_INVALID_SIZE  104 /*!< invalid size */
#define ERR_INVALID_RANGE 105 /*!< invalid range */
#define ERR_INVALID_CRC   106 /*!< crc or checksum was invalid */
#define ERR_NOT_FOUND     107 /*!< requested resource not found */
#define ERR_NOT_SUPPORTED 108 /*!< operation or feature not supported */
#define ERR_NOT_FINISHED  109 /*!< operation has not fully completed */
#define ERR_NOT_ALLOWED   110 /*!< operation is not allowed */
#define ERR_NOT_CONNECTED 111 /*!< operation is not connected */
#define ERR_NOT_EXIST     112 /*!< target doesn't exist */
#define ERR_BUSY          113 /*!< busy state */
#define ERR_TIMEOUT       114 /*!< operation timed out */
#define ERR_BAD_MESSAGE   115 /*!< bad message */
#define ERR_NO_MESSAGE    116 /*!< no message */
#define ERR_OVERFLOW      117 /*!< address... overflow */
#define ERR_UNDERFLOW     118 /*!< address... underflow */
#define ERR_MISMACTH      119 /*!< content mismatch */

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
