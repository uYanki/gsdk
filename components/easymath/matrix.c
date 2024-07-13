#include <stdlib.h>  ///< malloc free

#include "matrix.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "matrix"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void matrix_fill(matrix_t* src, f32 val)
{
    u16  n = src->rows * src->cols;
    f32* p = src->elems;
    while (n--) { *p++ = val; }
}

bool matrix_copy(matrix_t* src, matrix_t* dst)
{
    if (src->rows != dst->rows ||
        src->cols != dst->cols)
    {
        return FALSE;
    }

    {
        u16  n = src->cols * src->rows;
        f32 *s = src->elems, *d = dst->elems;
        while (n--) { *d++ = *s++; }
    }

    return true;
}

bool matrix_add(matrix_t* src1, matrix_t* src2, matrix_t* dst)
{
    if (src1->rows != dst->rows ||
        src1->cols != dst->cols ||
        src2->rows != dst->rows ||
        src2->cols != dst->cols)
    {
        return FALSE;
    }

    {
        u16  n  = dst->rows * dst->cols;
        f32 *s1 = src1->elems, *s2 = src2->elems, *d = dst->elems;
        while (n--) { *d++ = *s1++ + *s2++; }
    }

    return true;
}

bool matrix_subtract(matrix_t* src1, matrix_t* src2, matrix_t* dst)
{
    if (src1->rows != dst->rows ||
        src1->cols != dst->cols ||
        src2->rows != dst->rows ||
        src2->cols != dst->cols)
    {
        return FALSE;
    }

    {
        u16  n  = dst->rows * dst->cols;
        f32 *s1 = src1->elems, *s2 = src2->elems, *d = dst->elems;
        while (n--) { *d++ = *s1++ - *s2++; }
    }

    return true;
}

static void _matrix_multiply(matrix_t* src1, matrix_t* src2, matrix_t* dst)
{
    u8   col1, row1, col2;
    f32* p = dst->elems;

    for (row1 = 0; row1 < src1->rows; ++row1)
    {
        for (col2 = 0; col2 < src2->cols; ++col2, ++p)
        {
            for (*p = 0, col1 = 0; col1 < src1->cols; ++col1)
            {
                *p += (*matrix_cell(src1, row1, col1) * *matrix_cell(src2, col1, col2));
            }
        }
    }
}

bool matrix_multiply(matrix_t* src1, matrix_t* src2, matrix_t* dst)
{
    if (src1->cols != src2->rows &&
        dst->rows != src1->rows &&
        dst->cols != src2->cols)
    {
        return FALSE;
    }

    if (dst != src1 && dst != src2)
    {
        _matrix_multiply(src1, src2, dst);
    }
    else
    {
        f32* buf = (f32*)malloc(sizeof(f32) * dst->rows * dst->cols);
        if (buf == nullptr)
        {
            return FALSE;
        }
        {
            matrix_t tmp = {
                .elems = buf,
                .rows  = dst->rows,
                .cols  = dst->cols,
            };
            _matrix_multiply(src1, src2, &tmp);
            matrix_copy(&tmp, dst);
        }
        free(buf);
    }

    return true;
}

bool matrix_transpose(matrix_t* src, matrix_t* dst)
{
    u8 row, col;
    u8 rows = src->rows,
       cols = src->cols;
    f32 *s  = src->elems,
        *d  = dst->elems;

    if (src == dst)
    {
        // 同源

        if (rows = cols)
        {
            // 方阵
            for (row = 0; row < rows; ++row)
            {
                for (col = row; col < cols; ++col)
                {
                    if (row == col)
                    {
                        continue;
                    }
                    SWAP_ANY(f32, d[row * cols + col], s[col * rows + row]);
                }
            }
        }
        else
        {
            // 非方阵
            f32* buf = (f32*)malloc(sizeof(f32) * rows * cols);

            if (buf == nullptr)
            {
                return FALSE;
            }

            {
                matrix_t tmp = {
                    .elems = buf,
                    .rows  = cols,
                    .cols  = row,
                };
                matrix_transpose(src, &tmp);
                matrix_copy(&tmp, dst);
            }

            free(buf);

            return true;
        }
    }
    else
    {
        // 异源

        if ((src->rows * src->cols) !=
            (dst->rows * dst->cols))
        {
            return FALSE;
        }

        dst->rows = src->cols;
        dst->cols = src->rows;

        for (row = 0; row < rows; ++row)
        {
            for (col = 0; col < cols; ++col)
            {
                d[col * rows + row] = s[row * cols + col];
            }
        }

        return true;
    }
}

bool matrix_identity(matrix_t* src, f32 val)
{
    if (src->rows != src->cols)
    {
        return FALSE;
    }

    {
        u8   col, row;
        f32* s = src->elems;
        for (col = 0; col < src->cols; ++col)
        {
            for (row = 0; row < src->cols; ++row)
            {
                *s++ = (row == col) ? val : 0.0f;
            }
        }
    }

    return true;
}

void matrix_printf(cstr tag, matrix_t* src, cstr fmt, cstr sep)
{
    u8   row, col;
    f32* dat = src->elems;
    if (tag)
    {
        printf(tag);
        println("");
    }
    for (row = 0; row < src->rows; ++row)
    {
        col = 0;
        while (col < src->cols)
        {
            printf(fmt, *dat++);
            if (++col < src->cols)
            {
                printf(sep);
            }
        }
        println("");
    }
    println("");
}

bool matrix_remain_lower(matrix_t* src)
{
    u8 rows = src->rows;
    u8 cols = src->cols;

    if (rows != cols)
    {
        return FALSE;
    }

    {
        u8   row, col;
        f32* s = src->elems;

        for (row = 0; row < rows; ++row)
        {
            for (col = row + 1; col < cols; ++col)
            {
                *s++ = 0.0f;
            }
            s += row;
            s += 1;
        }
    }

    return true;
}

bool matrix_inverse(matrix_t* src, matrix_t* dst)
{
    // http://www.yunsuan.info/cgi-bin/matrix_inverse.py
    // https://www.osgeo.cn/app/sc211
    // https://matrix.reshish.com/zh/inverse.php

    if (src->rows != src->cols ||
        dst->rows != src->rows ||
        dst->cols != src->cols)
    {
        return FALSE;
    }
    {
        RO u8 n = src->cols;

        f32 *s = src->elems, *d = dst->elems;

        f32 *st, *dt;            // temporary
        f32 *sp, *dp;            // pointer
        f32 *s_pivot, *d_pivot;  // row pivot

        f32 in;
        u8  i, j, col;

        matrix_identity(dst, 1.0f);

        for (col = 0; col < n; ++col)
        {
            st = s + (col * n);
            if (*st == 0.0f)
            {
                return FALSE;
            }

            s_pivot = sp = s + (col * n);
            d_pivot = dp = d + (col * n);

            in = *s_pivot;
            for (j = 0u; j < col; ++j)
            {
                *sp++ /= in;
            }
            for (j = col; j < n; ++j)
            {
                *sp++ /= in;
                *dp++ /= in;
            }

            sp = s, dp = d;
            for (i = 0u; i < n; ++i)
            {
                if (i == col)
                {
                    sp += n - col;
                    dp += n;
                }
                else
                {
                    in = *sp, st = s_pivot, dt = d_pivot;
                    for (j = n - col; j > 0u; --j)
                    {
                        *sp++ -= in * *st++;
                    }
                    for (j = n; j > 0u; --j)
                    {
                        *dp++ -= in * *dt++;
                    }
                }
                sp += col;
            }

            ++s;
        }
    }

    return true;
}

bool matrix_scale(matrix_t* src, matrix_t* dst, f32 scale)
{
    if ((src->rows != dst->rows) ||
        (src->cols != dst->cols))
    {
        return FALSE;
    }

    {
        u16  n = src->rows * src->cols;
        f32 *s = src->elems, *d = dst->elems;
        while (n--)
        {
            *d++ = *s++ * scale;
        }
    }

    return true;
}
bool matrix_offset(matrix_t* src, matrix_t* dst, f32 offset)
{
    if ((src->rows != dst->rows) ||
        (src->cols != dst->cols))
    {
        return FALSE;
    }

    {
        u16  n = src->rows * src->cols;
        f32 *s = src->elems, *d = dst->elems;
        while (n--)
        {
            *d++ = *s++ + offset;
        }
    }

    return true;
}

#if 0

void Matrix_Test()
{
    f32 dat_a[][6] = {
        {1, 0, 0, 1, 1, 0},
        {0, 0, 0, 1, 4, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 2, 0, 1},
        {0, 0, 0, 1, 3, 0},
        {0, 0, 0, 0, 0, 4},
    };

    f32 dat_b[][6] = {
        {1, 0, 0, 1, 1, 0},
        {0, 1, 1, 1, 4, 0},
        {3, 1, 0, 0, 0, 0},
        {2, 3, 0, 2, 0, 1},
        {1, 4, 0, 1, 3, 0},
        {0, 5, 1, 0, 0, 4},
    };

    f32 dat_c[6][6] = {0};

    matrix_t A, B, C;

    matrix_setsrc(&A, dat_a);
    matrix_setsrc(&B, dat_b);
    matrix_setsrc(&C, dat_c);

    matrix_inverse(&A, &C);
    matrix_print("[AC]", &C);
    matrix_inverse(&B, &C);
    matrix_print("[BC]", &C);
}

#endif