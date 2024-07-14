#ifndef __GSDK_ENDIAN_H__
#define __GSDK_ENDIAN_H__

#include "gdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief byte order
 *
 *  LE: little endian, low byte at low address
 *  BE: big endian, high byte at low address
 *  HE: host endian
 *
 */

typedef uint16_t byteorder_t, endian_t;

#if defined(__ORDER_LITTLE_ENDIAN__)
#define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#else
#define LITTLE_ENDIAN 1234
#endif

#if defined(__ORDER_BIG_ENDIAN__)
#define BIG_ENDIAN __ORDER_BIG_ENDIAN__
#else
#define BIG_ENDIAN 4321
#endif

#if defined(__BYTE_ORDER__)
#define HOST_ENDIAN __BYTE_ORDER__
#elif defined(__ARMEL__)  // arm le
#define HOST_ENDIAN LITTLE_ENDIAN
#elif defined(__ARMEB__)  // arm be
#define HOST_ENDIAN BIG_ENDIAN
#endif

#if !defined(HOST_ENDIAN)
#warning "set `HOST_ENDIAN` as `LITTLE_ENDIAN`"
#define HOST_ENDIAN LITTLE_ENDIAN
#endif

/**
 * @brief number from byte array
 * @{
 */

#if (HOST_ENDIAN == LITTLE_ENDIAN)

#define he8(x)  le8(x)
#define he16(x) le16(x)
#define he32(x) le32(x)
#define he64(x) le64(x)

#elif (HOST_ENDIAN == BIG_ENDIAN)

#define he8(x)  be8(x)
#define he16(x) be16(x)
#define he32(x) be32(x)
#define he64(x) be64(x)

#endif

// clang-format off

static inline uint8_t le8(const uint8_t* p)
{
    return p[0];
}

static inline uint16_t le16(const uint8_t* p)
{
    return *(u16*)p;
}

static inline uint32_t le32(const uint8_t* p)
{
    // stm32的32位指针需要4字节对齐才能直接解引用
    return ((u32)le16(p + 2) << 16) | (u32)le16(p);
}

static inline uint64_t le64(const uint8_t* p)
{
    return ((u64)le32(p + 4) << 32) | (u64)le32(p);
}

static inline uint8_t be8(const uint8_t* p)
{
    return p[0];
}

static inline uint16_t be16(const uint8_t* p)
{
    return ((u16)p[1] << 0) | ((u16)p[0] << 8);
}

static inline uint32_t be32(const uint8_t* p)
{
    return ((u64)be32(p) << 16) | (u64)le32(p + 2);
}

static inline uint64_t be64(const uint8_t* p)
{
    return ((u64)be32(p) << 32) | (u64)le32(p + 4);
}

// clang-format on

/**
 * @}
 */

/**
 * @brief endian convert
 *
 * - host endian to big endian.
 * - host endian to little endian.
 * - big endian to host endian.
 * - little endian to host endian.
 *
 * @{
 */

#if (HOST_ENDIAN == LITTLE_ENDIAN)

#define htobe16(x) bswap16(x)
#define htobe32(x) bswap32(x)
#define htobe64(x) bswap64(x)
#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)

#define be16toh(x) bswap16(x)
#define be32toh(x) bswap32(x)
#define be64toh(x) bswap64(x)
#define le16toh(x) (x)
#define le32toh(x) (x)
#define le64toh(x) (x)

#elif (HOST_ENDIAN == BIG_ENDIAN)

#define htobe16(x) (x)
#define htobe32(x) (x)
#define htobe64(x) (x)
#define htole16(x) bswap16(x)
#define htole32(x) bswap32(x)
#define htole64(x) bswap64(x)

#define be16toh(x) (x)
#define be32toh(x) (x)
#define be64toh(x) (x)
#define le16toh(x) bswap16(x)
#define le32toh(x) bswap32(x)
#define le64toh(x) bswap64(x)

#endif

/**
 * @}
 */

/**
 * @brief byte swap
 * @{
 */

#if (defined(__GNUC__) && !defined(__CC_ARM))

#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)
#define bswap64(x) __builtin_bswap64(x)

#else

// clang-format off

static inline uint16_t bswap16(uint16_t x)
{
    return ((x & 0x00FF) << 8) |
           ((x & 0xFF00) >> 8);
}

static inline uint32_t bswap32(uint32_t x)
{
    return ((x & 0x000000FF) << 24) |
           ((x & 0x0000FF00) << 8) |
           ((x & 0x00FF0000) >> 8) |
           ((x & 0xFF000000) >> 24);
}

static inline uint64_t bswap64(uint64_t x)
{
    return ((x & 0x00000000000000FFULL) << 56) |
           ((x & 0x000000000000FF00ULL) << 40) |
           ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x00000000FF000000ULL) << 8) |
           ((x & 0x000000FF00000000ULL) >> 8) |
           ((x & 0x0000FF0000000000ULL) >> 24) |
           ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0xFF00000000000000ULL) >> 56);
}

// clang-format on

#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
