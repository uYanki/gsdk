#include "shell_clp.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "CLP"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

#define CLP_LINEEND     "\n"
#define CLP_INDENT      "    "

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

const char* CLP_ErrStr(clp_error_e eErrCode)
{
    static const char* aErrStr[] = {
        [CLP_ERROR_POSITIONAL_ARGUMNET_TOO_FEW]  = "positional argumnet too few",
        [CLP_ERROR_POSITIONAL_ARGUMNET_TOO_MANY] = "positional argumnet too many",
        [CLP_ERROR_OPTION_MISSING_NAME]          = "missing option name",
        [CLP_ERROR_OPTION_MISSING_VALUE]         = "missing option value",
        [CLP_ERROR_OPTION_FLAG_ONLY]             = "flag only",
        [CLP_ERROR_OPTION_UNDEFINED_NAME]        = "undefined option name",
        [CLP_ERROR_OPTION_UNCORRECT_USAGE]       = "uncorrect usage",
        [CLP_ERROR_ILLEGAL_VALUE]                = "invaild value",
    };

    return aErrStr[eErrCode];
}

const char* CLP_TypeStr(arg_type_e eType)
{
    static const char* aTypeStr[] = {
        [ARGUMENT_TYPE_END]              = "",  //
        [ARGUMENT_TYPE_POSITIONAL]       = "pos",
        [ARGUMENT_TYPE_POSITIONAL_INT32] = "pos-i32",
        [ARGUMENT_TYPE_POSITIONAL_FP32]  = "pos-f32",
        [ARGUMENT_TYPE_FLAG]             = "opt-noarg",
        [ARGUMENT_TYPE_REQUIRED]         = "opt",
        [ARGUMENT_TYPE_REQUIRED_INT32]   = "opt-i32",
        [ARGUMENT_TYPE_REQUIRED_FP32]    = "opt-f32",
        [ARGUMENT_TYPE_OPTIONAL]         = "opt",
        [ARGUMENT_TYPE_OPTIONAL_INT32]   = "opt-i32",
        [ARGUMENT_TYPE_OPTIONAL_FP32]    = "opt-f32",
    };

    return aTypeStr[eType];
}

static inline bool _IsEnd(const arg_attr_t* p)
{
    return p->eType == ARGUMENT_TYPE_END;
}

static inline bool _IsOption(const arg_attr_t* p)
{
    switch (p->eType)
    {
        case ARGUMENT_TYPE_FLAG:
        case ARGUMENT_TYPE_REQUIRED:
        case ARGUMENT_TYPE_REQUIRED_INT32:
        case ARGUMENT_TYPE_REQUIRED_FP32:
        case ARGUMENT_TYPE_OPTIONAL:
        case ARGUMENT_TYPE_OPTIONAL_INT32:
        case ARGUMENT_TYPE_OPTIONAL_FP32: return true;
        default: return false;
    }
}

static inline bool _IsPosArg(const arg_attr_t* p)
{
    switch (p->eType)
    {
        case ARGUMENT_TYPE_POSITIONAL:
        case ARGUMENT_TYPE_POSITIONAL_INT32:
        case ARGUMENT_TYPE_POSITIONAL_FP32: return true;
        default: return false;
    }
}

static int32_t _MakeError(clp_ctx_t* ctx, clp_error_e eErrCode)
{
    ctx->cpszArgStr = ctx->argv[ctx->argi - 1];
    ctx->eErrCode   = eErrCode;

    LOGW("[err] code = %s, errpos = %s", CLP_ErrStr(ctx->eErrCode), ctx->cpszArgStr);

    return CLP_TAG_ERROR;
}

static inline int32_t _MatchedOption(clp_ctx_t* ctx, const char* cpszArgStr)
{
    const arg_attr_t* pArgAttr = &ctx->argtbl.pBase[ctx->u16ArgIdx];

    ctx->cpszArgStr = cpszArgStr;

    if (ctx->cpszArgStr != nullptr)
    {
        char* pEnd = nullptr;

        char        shortname = pArgAttr->shortname == 0 ? ' ' : pArgAttr->shortname;
        const char* longname  = pArgAttr->longname == nullptr ? "" : pArgAttr->longname;

        switch (pArgAttr->type)
        {
            case ARGUMENT_TYPE_POSITIONAL:
            case ARGUMENT_TYPE_REQUIRED:
            case ARGUMENT_TYPE_OPTIONAL:
            {
                LOGD("[ok] (%-8s) %c, %s = %s", CLP_TypeStr(pArgAttr->type), shortname, longname, ctx->cpszArgStr);
                break;
            }

            case ARGUMENT_TYPE_POSITIONAL_INT32:
            case ARGUMENT_TYPE_OPTIONAL_INT32:
            case ARGUMENT_TYPE_REQUIRED_INT32:
            {
                ctx->uArgVal.i32 = strtol(cpszArgStr, &pEnd, 0);
                LOGD("[ok] (%-8s) %c, %s = %s , %d", CLP_TypeStr(pArgAttr->type), shortname, longname, ctx->cpszArgStr, ctx->uArgVal.i32);
                break;
            }

            case ARGUMENT_TYPE_POSITIONAL_FP32:
            case ARGUMENT_TYPE_REQUIRED_FP32:
            case ARGUMENT_TYPE_OPTIONAL_FP32:
            {
                ctx->uArgVal.fp32 = strtof(cpszArgStr, &pEnd);
                LOGD("[ok] (%-8s) %c, %s = %s , %f", CLP_TypeStr(pArgAttr->type), shortname, longname, ctx->cpszArgStr, ctx->uArgVal.fp32);
                break;
            }

            default:
            case ARGUMENT_TYPE_FLAG:
            {
                break;
            }
        }

        if (pEnd != nullptr && *pEnd != '\0')
        {
            return _MakeError(ctx, CLP_ERROR_ILLEGAL_VALUE);
        }
    }

    return pArgAttr->shortname;
}

static inline int32_t _MatchedPosArg(clp_ctx_t* ctx, char* tokens)
{
    for (uint16_t i = 0, j = 0; i < ctx->argtbl.u16Size; ++i)
    {
        const arg_attr_t* pArgAttr = &ctx->argtbl.pBase[i];

        if (_IsPosArg(pArgAttr) == false)
        {
            continue;
        }

        if (j++ == ctx->u16MatchedPosCnt)
        {
            ctx->u16ArgIdx = i;
            ctx->u16MatchedPosCnt++;
            return _MatchedOption(ctx, tokens);
        }
    }

    return _MakeError(ctx, CLP_ERROR_POSITIONAL_ARGUMNET_TOO_MANY);
}

static inline int32_t _MatchShortOpt(clp_ctx_t* ctx, char* tokens)
{
    for (uint16_t i = 0; i < ctx->argtbl.u16Size; ++i)
    {
        const arg_attr_t* pArgAttr = &ctx->argtbl.pBase[i];

        if (_IsOption(pArgAttr) == false)
        {
            continue;
        }

        if (pArgAttr->shortname != *tokens)
        {
            continue;
        }

        ctx->u16ArgIdx = i;

        tokens += 1;  // skip shortname

        switch (pArgAttr->type)
        {
            case ARGUMENT_TYPE_FLAG:
            {
                if (*tokens == '\0')
                {
                    // usage: '-[shortname]'
                    return _MatchedOption(ctx, nullptr);
                }
                else
                {
                    // error usage: '-[shortname][value]'
                    return _MakeError(ctx, CLP_ERROR_OPTION_FLAG_ONLY);
                }

                break;
            }

            case ARGUMENT_TYPE_OPTIONAL:
            case ARGUMENT_TYPE_OPTIONAL_INT32:
            case ARGUMENT_TYPE_OPTIONAL_FP32:
            {
                if (*tokens != '\0')
                {
                    // usage: '-[shortname][value]'
                    return _MatchedOption(ctx, tokens);
                }
                else
                {
                    if (ctx->argi == ctx->argc)
                    {
                        // usage: '-[shortname]'
                        return _MatchedOption(ctx, nullptr);
                    }
                    else
                    {
                        // '-[shortname] [value]'
                        return _MatchedOption(ctx, ctx->argv[ctx->argi++]);
                    }
                }

                break;
            }

            case ARGUMENT_TYPE_REQUIRED:
            case ARGUMENT_TYPE_REQUIRED_INT32:
            case ARGUMENT_TYPE_REQUIRED_FP32:
            {
                if (*tokens != '\0')
                {
                    // usage: '-[shortname][value]'
                    return _MatchedOption(ctx, tokens);
                }
                else
                {
                    if (ctx->argi == ctx->argc)
                    {
                        // missing value: '-[shortname]'
                        return _MakeError(ctx, CLP_ERROR_OPTION_MISSING_VALUE);
                    }
                    else
                    {
                        // '-[shortname] [value]'
                        return _MatchedOption(ctx, ctx->argv[ctx->argi++]);
                    }
                }

                break;
            }
        }
    }
}

static inline int32_t _MatchLongOpt(clp_ctx_t* ctx, char* tokens)
{
    for (uint16_t i = 0; i < ctx->argtbl.u16Size; ++i)
    {
        const arg_attr_t* pArgAttr = &ctx->argtbl.pBase[i];

        if (_IsOption(pArgAttr) == false)
        {
            continue;
        }

        if (pArgAttr->longname == nullptr)
        {
            continue;
        }

        const char* s1 = (const char*)tokens;
        const char* s2 = pArgAttr->longname;

        for (; *s1 && *s2 && *s1 != '='; s1++, s2++)
        {
            if (*s1 != *s2)
            {
                break;
            }
        }

        if (*s2 != '\0')
        {
            // find next
            continue;
        }

        if (*s1 != '\0' && *s1 != '=')
        {
            // find next
            continue;
        }

        ctx->u16ArgIdx = i;

        tokens = (char*)s1;  // skip longname

        switch (pArgAttr->type)
        {
            case ARGUMENT_TYPE_FLAG:
            {
                if (*tokens == '=')
                {
                    // error usage: '--[longname]='
                    return _MakeError(ctx, CLP_ERROR_OPTION_FLAG_ONLY);
                }
                else
                {
                    // error usage: '--[longname]'
                    return _MatchedOption(ctx, nullptr);
                }

                break;
            }

            case ARGUMENT_TYPE_OPTIONAL:
            case ARGUMENT_TYPE_OPTIONAL_INT32:
            case ARGUMENT_TYPE_OPTIONAL_FP32:
            {
                if (*tokens == '=')
                {
                    tokens += 1;  // skip '='

                    if (*tokens == '\0')
                    {
                        // missing value: '--[longname]='
                        return _MakeError(ctx, CLP_ERROR_OPTION_MISSING_VALUE);
                    }
                    else
                    {
                        // '--[longname]=[value]'
                        return _MatchedOption(ctx, tokens);
                    }
                }
                else
                {
                    if (ctx->argi == ctx->argc)
                    {
                        // '--[longname]'
                        return _MatchedOption(ctx, nullptr);
                    }
                    else
                    {
                        // '--[longname] [value]'
                        return _MatchedOption(ctx, ctx->argv[ctx->argi++]);
                    }
                }

                break;
            }

            case ARGUMENT_TYPE_REQUIRED:
            case ARGUMENT_TYPE_REQUIRED_INT32:
            case ARGUMENT_TYPE_REQUIRED_FP32:
            {
                if (*tokens == '=')
                {
                    tokens += 1;  // skip '='

                    if (*tokens == '\0')
                    {
                        // missing value: '--[longname]='
                        return _MakeError(ctx, CLP_ERROR_OPTION_MISSING_VALUE);
                    }
                    else
                    {
                        // '--[longname]=[value]'
                        return _MatchedOption(ctx, tokens);
                    }
                }
                else
                {
                    if (ctx->argi == ctx->argc)
                    {
                        // missing value: '--[longname]'
                        return _MakeError(ctx, CLP_ERROR_OPTION_MISSING_VALUE);
                    }
                    else
                    {
                        // '--[longname] [value]'
                        return _MatchedOption(ctx, ctx->argv[ctx->argi++]);
                    }
                }

                break;
            }
        }
    }

    return _MakeError(ctx, CLP_ERROR_OPTION_UNDEFINED_NAME);  // no macthed
}

err_t CLP_Init(clp_ctx_t* ctx, int argc, char** argv, const arg_attr_t* attrs)
{
    if (argc < 1 || argv == nullptr)
    {
        return -ERR_INVALID_VALUE;
    }

    ctx->argc = argc - 1;
    ctx->argv = argv + 1;

    ctx->argi = 0;

    ctx->argtbl.pBase   = attrs;
    ctx->argtbl.u16Size = 0;

    ctx->u16PosCnt        = 0;
    ctx->u16MatchedPosCnt = 0;

    ctx->u16ArgIdx = 0;

    const arg_attr_t* pArgAttr = ctx->argtbl.pBase;

    while (_IsEnd(pArgAttr) == false)
    {
        if (pArgAttr->longname != nullptr)
        {
            switch (pArgAttr->longname[0])
            {
                case '-':
                case '\0':
                {
                    // illegal char
                    LOGD("err: %s", pArgAttr->longname);
                    return -ERR_INVALID_VALUE;
                }

                default:
                {
                    break;
                }
            }
        }

        if (_IsPosArg(pArgAttr))
        {
            ctx->u16PosCnt++;
        }

        pArgAttr++, ctx->argtbl.u16Size++;
    }

    return ERR_NONE;
}

int32_t CLP_Next(clp_ctx_t* ctx)
{
    if (ctx->argc <= ctx->argi)
    {
        if (ctx->u16MatchedPosCnt < ctx->u16PosCnt)
        {
            return _MakeError(ctx, CLP_ERROR_POSITIONAL_ARGUMNET_TOO_FEW);
        }

        return CLP_TAG_EXIT;
    }

    char* tokens = ctx->argv[ctx->argi++];

    if (tokens[0] == '-')
    {
        if (tokens[1] == '-')
        {
            tokens += 2;  // skip '--'

            if (*tokens == '\0')
            {
                // missing longname: '--' or '--='
                return _MakeError(ctx, CLP_ERROR_OPTION_MISSING_NAME);
            }

            return _MatchLongOpt(ctx, tokens);  // 选项参数
        }
        else
        {
            tokens += 1;  // skip '-'

            if (tokens[0] == '\0')
            {
                // missing shortname: '-'
                return _MakeError(ctx, CLP_ERROR_OPTION_MISSING_NAME);
            }

            return _MatchShortOpt(ctx, tokens);  // 选项参数
        }
    }

    return _MatchedPosArg(ctx, tokens);  // 位置参数
}

void CLP_MakeHelpText(clp_ctx_t* ctx)
{
    printf(CLP_LINEEND);

    const arg_attr_t* pArgAttr = ctx->argtbl.pBase;

    while (_IsEnd(pArgAttr) == false)
    {
        printf(CLP_INDENT);

        if (pArgAttr->shortname != 0)
        {
            printf("-%c", pArgAttr->shortname);

            if (pArgAttr->longname != nullptr)
            {
                printf(", ");
            }
        }

        if (pArgAttr->longname != nullptr)
        {
            printf("--%s", pArgAttr->longname);
        }

        if (pArgAttr->value_desc != nullptr)
        {
            switch (pArgAttr->type)
            {
                case ARGUMENT_TYPE_REQUIRED:
                {
                    printf(" <%s>", pArgAttr->value_desc);
                    break;
                }

                case ARGUMENT_TYPE_OPTIONAL:
                {
                    printf(" (%s)", pArgAttr->value_desc);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        printf(CLP_LINEEND);

        if (pArgAttr->option_desc != nullptr)
        {
            printf("\n");

            printf(CLP_INDENT);
            printf(CLP_INDENT);

            // 长度限制? 换行处理+再次缩进

            const char* s = pArgAttr->option_desc;

            uint8_t i = 0;

            while (*s)
            {
                putchar(*s++);

                if (++i == 80)
                {
                    i = 0;
                    printf("\n");

                    printf(CLP_INDENT);
                    printf(CLP_INDENT);
                }
            }

            printf(CLP_LINEEND);
        }

        printf(CLP_LINEEND);

        pArgAttr++;
    }
}
