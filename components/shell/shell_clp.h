/**
 * @brief command line parser
 */

#ifndef __SHELL_CLP_H__
#define __SHELL_CLP_H__

#include "gsdk.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define CLP_TAG_ERROR -2
#define CLP_TAG_EXIT  -1

/**
 * @brief 选项赋值
 *
 * ① 赋值运算符：-v=value --verbose=value
 * ② 空格+下个参数：-v value --verbose value (包含'-')
 *
 */

// clang-format off

#define ARGUMENT_OPTION_HELP  \
    { ARGUMENT_TYPE_FLAG, 'h', "help", "show help message and exit", nullptr }, \
    { ARGUMENT_TYPE_FLAG, '?', "help", "show help message and exit", nullptr }

#define ARGUMENT_OPTION_VERSION  \
    { ARGUMENT_TYPE_FLAG, 'v', "version", "show version exit", nullptr }

#define ARGUMENT_END  \
    { ARGUMENT_TYPE_END, 0, 0, 0, 0 }

// clang-format on

typedef enum {
    ARGUMENT_TYPE_POSITIONAL,        // 位置参数
    ARGUMENT_TYPE_POSITIONAL_INT32,  // 位置参数（解析为整数）
    ARGUMENT_TYPE_POSITIONAL_FP32,   // 位置参数（解析为浮点）
    ARGUMENT_TYPE_FLAG,              // 选项（无参数）
    ARGUMENT_TYPE_REQUIRED,          // 选项 (参数必填)
    ARGUMENT_TYPE_REQUIRED_INT32,    // 选项 (参数必填，解析为整数)
    ARGUMENT_TYPE_REQUIRED_FP32,     // 选项 (参数必填，解析为整数)
    ARGUMENT_TYPE_OPTIONAL,          // 选项 (参数可填)
    ARGUMENT_TYPE_OPTIONAL_INT32,    // 选项 (参数可填，解析为浮点)
    ARGUMENT_TYPE_OPTIONAL_FP32,     // 选项 (参数可填，解析为浮点)
    ARGUMENT_TYPE_END,               // 截止标志
} arg_type_e;

typedef struct {
    arg_type_e  type;         // 参数类型
    int         shortname;    // 短选项
    const char* longname;     // 长选项
    const char* option_desc;  // 参数描述, 仅用生成帮助文本
    const char* value_desc;   // 值描述, 仅用生成帮助文本（example: "--option_desc=<value_desc>"）
} arg_attr_t;

typedef enum {
    CLP_ERROR_POSITIONAL_ARGUMNET_TOO_FEW,   // 参数过少
    CLP_ERROR_POSITIONAL_ARGUMNET_TOO_MANY,  // 参数过多
    CLP_ERROR_OPTION_MISSING_NAME,           // 参数名缺失
    CLP_ERROR_OPTION_MISSING_VALUE,          // 参数值缺失
    CLP_ERROR_OPTION_FLAG_ONLY,              // 参数值过多
    CLP_ERROR_OPTION_UNDEFINED_NAME,         // 参数名未定义
    CLP_ERROR_OPTION_UNCORRECT_USAGE,        // 不正确的用法
    CLP_ERROR_ILLEGAL_VALUE,                 // 不合法的参数值
} clp_error_e;

#define __IN
#define __OUT

/**
 * @brief command line parser
 */
typedef struct {
    __IN int    argc;
    __IN char** argv;
    int         argi;  // 下一个 argv 的索引

    struct {
        const arg_attr_t* pBase;    // 参数属性表地址
        uint16_t          u16Size;  // 参数属性表大小
    } argtbl;

    uint16_t u16PosCnt;         // 参数属性表中的位置参数个数
    uint16_t u16MatchedPosCnt;  // 已匹配的位置参数个数

    __OUT uint16_t    u16ArgIdx;   // 匹配结果在参数索引中的索引
    __OUT const char* cpszArgStr;  // 当前参数值

    __OUT union {  // 当前参数值
        int32_t   i32;
        float32_t fp32;
    } uArgVal;

    __OUT clp_error_e eErrCode;  // 错误码

} clp_ctx_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

extern err_t   CLP_Init(clp_ctx_t* ctx, int argc, char** argv, const arg_attr_t* cpAttrTbl);
extern int32_t CLP_Next(clp_ctx_t* ctx);
extern void    CLP_MakeHelpText(clp_ctx_t* ctx);

extern const char* CLP_TypeStr(arg_type_e eType);
extern const char* CLP_ErrStr(clp_error_e eErrCode);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
