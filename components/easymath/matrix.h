#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    f32* elems;
    u8   rows;
    u8   cols;
} matrix_t;

#define matrix_setsrc(mat, arr)          \
    do                                   \
    {                                    \
        (mat)->elems = (f32*)arr;        \
        (mat)->rows  = ARRAY_SIZE(arr);  \
        (mat)->cols  = ARRAY_SIZE(*arr); \
    } while (0)

#define matrix_print(tag, src) matrix_printf(tag, src, "%.2f", ", ")

#define matrix_zero(src)       matrix_fill(src, 0.0f)

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void matrix_printf(cstr tag, matrix_t* src, cstr fmt, cstr sep);

void matrix_fill(matrix_t* src, f32 val);
bool matrix_copy(matrix_t* src, matrix_t* dst);

bool matrix_identity(matrix_t* src, f32 val);  ///< 单位矩阵

bool matrix_scale(matrix_t* src, matrix_t* dst, f32 scale);
bool matrix_offset(matrix_t* src, matrix_t* dst, f32 offset);

bool matrix_add(matrix_t* src1, matrix_t* src2, matrix_t* dst);
bool matrix_subtract(matrix_t* src, matrix_t* src2, matrix_t* dst);
bool matrix_multiply(matrix_t* src, matrix_t* src2, matrix_t* dst);

bool matrix_transpose(matrix_t* src, matrix_t* dst); /*<! 转矩 */
bool matrix_inverse(matrix_t* src, matrix_t* dst);   /*<! 逆 */

bool matrix_remain_lower(matrix_t* src); /*<! 右下三角 */

static inline f32* matrix_cell(matrix_t* src, u8 row, u8 col)
{
    return &(src->elems[row * src->rows + col]);
}

#ifdef __cplusplus
}
#endif

#endif
