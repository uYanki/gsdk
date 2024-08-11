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

#ifndef PRINTF
#define PRINTF(...) printf(__VA_ARGS__)
#endif /*!< PRINTF */

#define PRINTLN(format, ...) PRINTF(format LIENEND, ##__VA_ARGS__)

/**
 * @brief scanf
 */

#ifndef SCANF
#define SCANF(...) scanf(__VA_ARGS__)
#endif /*!< SCANF */

/**
 * @brief assert
 */

#ifndef ASSERT
#ifndef NDEBUG

#if 0

#define ASSERT(expr, msg) assert((msg, expr));

#else

#define ASSERT(expr, msg)                                                    \
    do                                                                            \
    {                                                                             \
        if (!(expr))                                                              \
        {                                                                         \
            PRINTLN("\"" #expr "\" assert failed at file: %s, line: %d. %s", \
                         __FILE__, __LINE__, #msg);                               \
            while (1);                                                            \
        }                                                                         \
    } while (0)

#endif

#else /*!< NDEBUG */
#define ASSERT(expr, msg)
#endif /*!< NDEBUG */
#endif /*!< ASSERT */

/**
 * @brief memory alloc
 */
#ifndef MALLOC

#endif /*!< MALLOC */

/**
 * @brief memory free
 */
#ifndef MFREE

#endif /*!< MFREE */

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
