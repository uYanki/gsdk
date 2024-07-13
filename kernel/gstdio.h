#ifndef __GSDK_STDIO_H__
#define __GSDK_STDIO_H__

#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief printf
 */

#ifndef LIENEND
#define LIENEND "\n"
#endif /*!< LIENEND */

#ifndef GSDK_PRINTF
#define GSDK_PRINTF(...) printf(__VA_ARGS__)
#endif /*!< GSDK_PRINTF */

#define GSDK_PRINTLN(format, ...) GSDK_PRINTF(format LIENEND, ##__VA_ARGS__)

/**
 * @brief scanf
 */

#ifndef GSDK_SCANF
#define GSDK_SCANF(...) scanf(__VA_ARGS__)
#endif /*!< GSDK_SCANF */

/**
 * @brief assert
 */

#ifndef GSDK_ASSERT
#ifndef NDEBUG

#if 0

#define GSDK_ASSERT(expr, msg) assert((msg, expr));

#else

#define GSDK_ASSERT(expr, msg)                                                    \
    do                                                                            \
    {                                                                             \
        if (!(expr))                                                              \
        {                                                                         \
            GSDK_PRINTLN("\"" #expr "\" assert failed at file: %s, line: %d. %s", \
                         __FILE__, __LINE__, #msg);                               \
            while (1);                                                            \
        }                                                                         \
    } while (0)

#endif

#else /*!< NDEBUG */
#define GSDK_ASSERT(expr, msg)
#endif /*!< NDEBUG */
#endif /*!< GSDK_ASSERT */

/**
 * @brief memory alloc
 */
#ifndef GSDK_MALLOC

#endif /*!< GSDK_MALLOC */

/**
 * @brief memory free
 */
#ifndef GSDK_MFREE

#endif /*!< GSDK_MFREE */

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
