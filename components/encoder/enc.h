

typedef enum {
    ENC_INC_AB,        // 增量式: AB
    ENC_INC_ABZ,       // 增量式: ABZ
    ENC_INC_ABZ_UVW,   // 增量式: ABz+UVW
    ENC_ABS_BISS,      // 绝对值: BiSS协议
    ENC_ABS_TAMAGAWA,  // 绝对值: 多摩川
    ENC_ABS_ENDAT,     // 绝对值: EnDat协议
    ENC_SINCOS,        // 正余弦
    ENC_RESOLVER       // 旋变
} encoder_type_e;
