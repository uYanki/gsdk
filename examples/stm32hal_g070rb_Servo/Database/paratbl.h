#ifndef __PARA_TABLE_H__
#define __PARA_TABLE_H__

#include "paraattr.h"
#include "axis_defs.h"
#include "arm_math.h"

// clang-format off

typedef struct __packed {
    u32 u32DrvScheme;        // P0000 驱动器方案
    u32 u32McuSwBuildDate;   // P0002 软件构建日期
    u32 u32McuSwBuildTime;   // P0004 软件构建时间
    u32 u32McuSwVer;         // P0006 固件版本号
    u16 _Resv8;              // P0008 
    u16 u16AxisNum;          // P0009 轴数量
    u16 u16LedNum;           // P0010 指示灯个数
    u16 u16KeyNum;           // P0011 按键个数
    u16 u16ExtDiNum;         // P0012 外部数字量输入个数
    u16 u16ExtDoNum;         // P0013 外部数字量输出个数
    u16 u16ExtHDiNum;        // P0014 外部高速数字量输入个数
    u16 u16ExtHDoNum;        // P0015 外部高速数字量输出个数
    u16 u16ExtAiNum;         // P0016 外部模拟量输入个数
    u16 u16ExtAoNum;         // P0017 外部模拟量输出个数
    u16 u16TsNum;            // P0018 温度采样个数
    u16 _Resv19;             // P0019 
    u32 u32ChipDevID;        // P0020 芯片设备ID
    u32 u32ChipRevID;        // P0022 芯片版本ID
    u32 u32ChipUIDw0;        // P0024 芯片96位UID
    u32 u32ChipUIDw1;        // P0026 芯片96位UID
    u32 u32ChipUIDw2;        // P0028 芯片96位UID
    u32 u32HalVersion;       // P0030 HAL库版本号
    u16 _Resv32;             // P0032 
    u16 u16SysPwd;           // P0033 系统密码
    u16 u16SysAccess;        // P0034 当前系统权限
    u16 u16SysRst;           // P0035 系统软复位
    u16 _Resv36;             // P0036 
    u16 u16UmdcHwCoeff;      // P0037 主回路电压采样系数
    u16 u16UcdcHwCoeff;      // P0038 控制电电压采样系数
    u16 u16Uai0HwCoeff;      // P0039 
    u16 u16Uai1HwCoeff;      // P0040 
    u16 u16Uai2HwCoeff;      // P0041 
    u16 u16UTempHwCoeff;     // P0042 
    u16 _Resv43;             // P0043 
    u16 u16MbSlvId;          // P0044 Modbus-从站节点ID
    u32 u32MbBaudrate;       // P0045 Modbus-从站波特率
    u16 u16MbParity;         // P0047 Modbus-从站检验位
    u16 u16MbMstEndian;      // P0048 Modbus-主站大小端
    u16 u16MbDisconnTime;    // P0049 Modbus-通讯断开检测时间
    u16 u16MbResponseDelay;  // P0050 Modbus-命令响应延时 
    u16 _Resv51;             // P0051 
    u16 u16CopNodeId;        // P0052 CANopen-节点ID
    u16 u16CopBitrate;       // P0053 CANopen-比特率
    u16 u16PdoInhTime;       // P0054 CANopen-PDO禁止时间
    u16 u16CopState;         // P0055 CANopen-通信状态
    u16 _Resv56;             // P0056 
    u16 u16EcatScanPrd;      // P0057 EtherCAT-通信状态
    u16 u16EcatState;        // P0058 EtherCAT-Port0错误统计
    u16 u16EcatPort0ErrCnt;  // P0059 EtherCAT-Port1错误统计
    u16 u16EcatPort1ErrCnt;  // P0060 EtherCAT-接收错误统计
    u16 u16EcatRxErrCnt;     // P0061 EtherCAT-处理单元错误统计
    u16 u16EcatPdiErrCnt;    // P0062 
    u16 _Resv63;             // P0063 
    u16 _Resv64;             // P0064 
    u16 _Resv65;             // P0065 
    u16 u16ScopeSampPrd;     // P0066 数据记录采样周期设定
    u16 u16ScopeSampPts;     // P0067 数据记录采样点数设定
    u16 u16ScopeChCnt;       // P0068 数据记录通道数量设定
    u16 u16ScopeChSrc00;     // P0069 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc01;     // P0070 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc02;     // P0071 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc03;     // P0072 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc04;     // P0073 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc05;     // P0074 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc06;     // P0075 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc07;     // P0076 数据记录通道来源设定 (数据源)
    u16 u16ScopeChAddr00;    // P0077 数据记录通道地址设定
    u16 u16ScopeChAddr01;    // P0078 数据记录通道地址设定
    u16 u16ScopeChAddr02;    // P0079 数据记录通道地址设定
    u16 u16ScopeChAddr03;    // P0080 数据记录通道地址设定
    u16 u16ScopeChAddr04;    // P0081 数据记录通道地址设定
    u16 u16ScopeChAddr05;    // P0082 数据记录通道地址设定
    u16 u16ScopeChAddr06;    // P0083 数据记录通道地址设定
    u16 u16ScopeChAddr07;    // P0084 数据记录通道地址设定
    u16 u16ScopeSampIdx;     // P0085 当前数据记录索引
    u16 u16ScopeSampSts;     // P0086 数据记录状态
    u16 _Resv87;             // P0087 
    u16 _Resv88;             // P0088 
    u16 _Resv89;             // P0089 
    u16 _Resv90;             // P0090 
    u16 _Resv91;             // P0091 
    u16 _Resv92;             // P0092 
    u16 _Resv93;             // P0093 
    u16 _Resv94;             // P0094 
    u16 _Resv95;             // P0095 
    u16 _Resv96;             // P0096 
    u16 _Resv97;             // P0097 
    u16 _Resv98;             // P0098 
    u16 _Resv99;             // P0099 
    u16 _Resv100;            // P0100 
    u16 _Resv101;            // P0101 
    u16 _Resv102;            // P0102 
    u16 _Resv103;            // P0103 
    u16 _Resv104;            // P0104 
    u16 _Resv105;            // P0105 
    u16 _Resv106;            // P0106 
    u16 _Resv107;            // P0107 
    u16 _Resv108;            // P0108 
    u16 _Resv109;            // P0109 
    u16 _Resv110;            // P0110 
    u16 _Resv111;            // P0111 
    u16 _Resv112;            // P0112 
    u16 _Resv113;            // P0113 
    u16 _Resv114;            // P0114 
    u16 _Resv115;            // P0115 
    u16 _Resv116;            // P0116 
    u16 _Resv117;            // P0117 
    u16 _Resv118;            // P0118 
    u16 _Resv119;            // P0119 
    u16 _Resv120;            // P0120 
    u16 _Resv121;            // P0121 
    u16 _Resv122;            // P0122 
    u16 _Resv123;            // P0123 
    u16 _Resv124;            // P0124 
    u16 _Resv125;            // P0125 
    u16 _Resv126;            // P0126 
    u16 _Resv127;            // P0127 
    u16 _Resv128;            // P0128 
    u16 _Resv129;            // P0129 
    u16 _Resv130;            // P0130 
    u16 _Resv131;            // P0131 
    u16 _Resv132;            // P0132 
    u16 _Resv133;            // P0133 
    u16 _Resv134;            // P0134 
    u16 _Resv135;            // P0135 
    u16 _Resv136;            // P0136 
    u16 _Resv137;            // P0137 
    u16 _Resv138;            // P0138 
    u16 _Resv139;            // P0139 
    u16 _Resv140;            // P0140 
    u16 _Resv141;            // P0141 
    u16 _Resv142;            // P0142 
    u16 _Resv143;            // P0143 
    u16 _Resv144;            // P0144 
    u16 _Resv145;            // P0145 
    u16 _Resv146;            // P0146 
    u16 _Resv147;            // P0147 
    u16 _Resv148;            // P0148 
    u16 _Resv149;            // P0149 
    u16 _Resv150;            // P0150 
    u16 _Resv151;            // P0151 
    u16 _Resv152;            // P0152 
    u16 _Resv153;            // P0153 
    u16 _Resv154;            // P0154 
    u16 _Resv155;            // P0155 
    u16 _Resv156;            // P0156 
    u16 _Resv157;            // P0157 
    s16 _Resv158;            // P0158 
    s16 _Resv159;            // P0159 
    s16 _Resv160;            // P0160 
    u16 _Resv161;            // P0161 
    s16 _Resv162;            // P0162 
    s16 _Resv163;            // P0163 
    s16 _Resv164;            // P0164 
    u16 _Resv165;            // P0165 
    u16 _Resv166;            // P0166 
    u16 _Resv167;            // P0167 
    u16 _Resv168;            // P0168 
    u16 _Resv169;            // P0169 
    u16 _Resv170;            // P0170 
    u16 _Resv171;            // P0171 
    s16 _Resv172;            // P0172 
    s16 _Resv173;            // P0173 
    u16 _Resv174;            // P0174 
    u16 _Resv175;            // P0175 
    u16 _Resv176;            // P0176 
    s16 _Resv177;            // P0177 
    u16 _Resv178;            // P0178 
    u16 _Resv179;            // P0179 
    u16 u16TunerSoftVer;     // P0180 识别码1
    u16 u16TunerDrvType;     // P0181 识别码2
    u16 _Resv182;            // P0182 
    u16 _Resv183;            // P0183 
    u16 u16AlmCode01;        // P0184 历史报警代码01
    u16 u16AlmCode02;        // P0185 历史报警代码02
    u16 u16AlmCode03;        // P0186 历史报警代码03
    u16 u16AlmCode04;        // P0187 历史报警代码04
    u16 u16AlmCode05;        // P0188 历史报警代码05
    u16 u16AlmCode06;        // P0189 历史报警代码06
    u16 u16AlmCode07;        // P0190 历史报警代码07
    u16 u16AlmCode08;        // P0191 历史报警代码08
    u16 u16AlmCode09;        // P0192 历史报警代码09
    u16 u16AlmCode10;        // P0193 历史报警代码10
    u16 u16AlmCode11;        // P0194 历史报警代码11
    u16 u16AlmCode12;        // P0195 历史报警代码12
    u16 u16AlmCode13;        // P0196 历史报警代码13
    u16 u16AlmCode14;        // P0197 历史报警代码14
    u16 u16AlmCode15;        // P0198 历史报警代码15
    u32 u32AlmTime00;        // P0199 历史报警时间戳00
    u32 u32AlmTime01;        // P0201 历史报警时间戳01
    u32 u32AlmTime02;        // P0203 历史报警时间戳02
    u32 u32AlmTime03;        // P0205 历史报警时间戳03
    u32 u32AlmTime04;        // P0207 历史报警时间戳04
    u32 u32AlmTime05;        // P0209 历史报警时间戳05
    u32 u32AlmTime06;        // P0211 历史报警时间戳06
    u32 u32AlmTime07;        // P0213 历史报警时间戳07
    u32 u32AlmTime08;        // P0215 历史报警时间戳08
    u32 u32AlmTime09;        // P0217 历史报警时间戳09
    u32 u32AlmTime10;        // P0219 历史报警时间戳10
    u32 u32AlmTime11;        // P0221 历史报警时间戳11
    u32 u32AlmTime12;        // P0223 历史报警时间戳12
    u32 u32AlmTime13;        // P0225 历史报警时间戳13
    u32 u32AlmTime14;        // P0227 历史报警时间戳14
    u32 u32AlmTime15;        // P0229 历史报警时间戳15
    u16 _Resv231;            // P0231 
    u16 _Resv232;            // P0232 
    u32 u32SysRunTime;       // P0233 运行时间
    u16 _Resv235;            // P0235 
    u16 _Resv236;            // P0236 
    u16 u16DiState;          // P0237 DI输入端子电平状态
    u16 u16DoState;          // P0238 DO输出端子电平状态
    u16 _Resv239;            // P0239 
    u16 s16UaiPu0;           // P0240 AI0输入电压数字量
    u16 s16UaiPu1;           // P0241 AI1输入电压数字量
    u16 s16UaiPu2;           // P0242 AI2输入电压数字量
    u16 _Resv243;            // P0243 
    u16 s16UaiSi0;           // P0244 AI0输入电压
    u16 s16UaiSi1;           // P0245 AI1输入电压
    u16 s16UaiSi2;           // P0246 AI2输入电压
    u16 u16UaoSi0;           // P0247 AO0输出电压
    u16 u16UaoSi1;           // P0248 AO1输出电压
    u16 u16UaoSi2;           // P0249 AO2输出电压
    u16 _Resv250;            // P0250 
    u16 _Resv251;            // P0251 
    u16 u16UmdcSi;           // P0252 主回路母线电压物理值
    u16 u16UcdcSi;           // P0253 控制母线电压物理值
    u16 s16TempIpmSi;        // P0254 逆变桥温度物理值
    u16 s16TempRectSi;       // P0255 整流桥温度物理值
    u16 _Resv256;            // P0256 
    u16 _Resv257;            // P0257 
    u16 _Resv258;            // P0258 
    u16 _Resv259;            // P0259 
    u16 _Resv260;            // P0260 
    u16 u16ProbeStatus;      // P0261 探针状态
    s64 s64ProbePosEdgePo00; // P0262 探针1上升沿锁存位置
    s64 s64ProbeNegEdgePo00; // P0266 探针1下降沿锁存位置
    s64 s64ProbePosEdgePo01; // P0270 探针2上升沿锁存位置
    s64 s64ProbeNegEdgePo01; // P0274 探针2下降沿锁存位置
    s64 s64ProbePosEdgePo02; // P0278 探针3上升沿锁存位置
    s64 s64ProbeNegEdgePo02; // P0282 探针3下降沿锁存位置
    s64 s64ProbePosEdgePo03; // P0286 探针4上升沿锁存位置
    s64 s64ProbeNegEdgePo03; // P0290 探针4下降沿锁存位置
    u16 _Resv294;            // P0294 
    u16 _Resv295;            // P0295 
    u16 u16PosLoopIsrTime;   // P0296 
    u16 u16SpdLoopIsrTime;   // P0297 
    u16 u16CurLoopIsrTime;   // P0298 
    u16 u16LoopTick;         // P0299 
} device_para_t;

typedef struct __packed {
    s16 _Resv300;            // P0300 
    u16 u16MotorType;        // P0301 电机类型
    u16 u16EncoderType;      // P0302 编码器类型
    u16 u16MotPolePairs;     // P0303 电机极对数
    u16 u16MotAccMax;        // P0304 电机最大加速度
    u32 u32MotInertia;       // P0305 电机转动惯量
    u16 u16MotStatorRes;     // P0307 电机定子电阻
    u16 u16MotStatorLd;      // P0308 电机定子D轴电感
    u16 u16MotStatorLq;      // P0309 电机定子Q轴电感
    u16 u16MotCurRate;       // P0310 电机额定电流
    u16 u16MotCurMax;        // P0311 电机最大电流
    u16 u16MotTrqRate;       // P0312 电机额定转矩
    u16 u16MotTrqMax;        // P0313 电机最大转矩
    u16 u16MotSpdRate;       // P0314 电机额定转速
    u16 u16MotSpdMax;        // P0315 电机最大转速
    u16 u16MotEmfCoeff;      // P0316 电机反电动势常数
    u16 u16MotTrqCoeff;      // P0317 电机转矩系数
    u16 u16MotTm;            // P0318 电机机械时间常数
    u16 u16EncType;          // P0319 编码器类型
    u16 u16AbsEncWorkMode;   // P0320 绝对值编码器工作模式
    u32 u32EncRes;           // P0321 编码器分辨率
    u32 u32EncPosInit;       // P0323 编码器初始位置
    u32 u32EncPos;           // P0325 编码器单圈位置
    s64 s64EncMultPos;       // P0327 编码器多圈位置
    s32 s32EncTurns;         // P0331 编码器圈数
    u16 _Resv333;            // P0333 
    u16 u16HallState;        // P0334 霍尔真值
    u16 _Resv335;            // P0335 
    u16 u16CurSampType;      // P0336 电流采样类型
    u16 u16PwmDutyMax;       // P0337 
    u16 _Resv338;            // P0338 
    u16 u16CarryFreq;        // P0339 载波频率
    u16 u16PosLoopFreq;      // P0340 位置环频率
    u16 u16SpdLoopFreq;      // P0341 速度环频率
    u16 u16CurLoopFreq;      // P0342 电流环频率
    u16 _Resv343;            // P0343 
    u16 _Resv344;            // P0344 
    u16 _Resv345;            // P0345 
    u16 u16PosLoopKp;        // P0346 位置环增益系数
    u16 u16PosLoopKi;        // P0347 位置环积分系数
    u16 u16PosLoopKd;        // P0348 位置环微分系数
    u16 u16SpdLoopKp;        // P0349 速度环增益系数
    u16 u16SpdLoopKi;        // P0350 速度环积分系数
    u16 u16SpdLoopKd;        // P0351 速度环微分系数
    u16 u16CurLoopIqKp;      // P0352 电流环转矩增益系数
    u16 u16CurLoopIqKi;      // P0353 电流环转矩积分系数
    u16 u16CurLoopIqKd;      // P0354 电流环转矩增益系数
    u16 u16CurLoopIdKp;      // P0355 电流环磁链增益系数
    u16 u16CurLoopIdKi;      // P0356 电流环磁链积分系数
    u16 u16CurLoopIdKd;      // P0357 电流环磁链增益系数
    u16 _Resv358;            // P0358 
    u16 _Resv359;            // P0359 
    u16 _Resv360;            // P0360 
    u16 _Resv361;            // P0361 
    u16 _Resv362;            // P0362 
    u16 _Resv363;            // P0363 
    u16 _Resv364;            // P0364 
    u16 u16AppSel;           // P0365 应用选择
    u16 u16CtrlMethod;       // P0366 控制方法
    u16 u16CtrlMode;         // P0367 控制模式
    u16 u16CtrlCmdSrc;       // P0368 控制命令来源
    u16 u16PwrOnAutoRun;     // P0369 上电自使能
    u32 u32CommCmd;          // P0370 通信命令字
    u16 _Resv372;            // P0372 
    u16 u16DiSrc;            // P0373 外部或虚拟DI选择
    u16 u16DoSrc;            // P0374 外部或虚拟DO选择
    u16 _Resv375;            // P0375 
    u16 _Resv376;            // P0376 
    u16 u16PosLimSrc;        // P0377 位置指令来源
    s64 s64PosLimFwd;        // P0378 数字位置指令
    s64 s64PosLimRev;        // P0382 位置规划模式
    u16 u16PosLimSts;        // P0386 位置规划状态
    u16 _Resv387;            // P0387 
    u32 u32ElecGearNum;      // P0388 电子齿轮比分子
    u32 u32ElecGearDeno;     // P0390 电子齿轮比分母
    u16 _Resv392;            // P0392 
    u16 u16EncFreqDivDir;    // P0393 编码器分频输出脉冲方向
    u16 u16EncFreqDivNum;    // P0394 编码器分频输出分子
    u16 u16EncFreqDivDeno;   // P0395 编码器分频输出分母
    u16 _Resv396;            // P0396 
    s64 s64PosDigRef00;      // P0397 多段数字位置指令00
    s64 s64PosDigRef01;      // P0401 多段数字位置指令01
    s64 s64PosDigRef02;      // P0405 多段数字位置指令02
    s64 s64PosDigRef03;      // P0409 多段数字位置指令03
    s64 s64PosDigRef04;      // P0413 多段数字位置指令04
    s64 s64PosDigRef05;      // P0417 多段数字位置指令05
    s64 s64PosDigRef06;      // P0421 多段数字位置指令06
    s64 s64PosDigRef07;      // P0425 多段数字位置指令07
    s64 s64PosDigRef08;      // P0429 多段数字位置指令08
    s64 s64PosDigRef09;      // P0433 多段数字位置指令09
    s64 s64PosDigRef10;      // P0437 多段数字位置指令10
    s64 s64PosDigRef11;      // P0441 多段数字位置指令11
    s64 s64PosDigRef12;      // P0445 多段数字位置指令12
    s64 s64PosDigRef13;      // P0449 多段数字位置指令13
    s64 s64PosDigRef14;      // P0453 多段数字位置指令14
    s64 s64PosDigRef15;      // P0457 多段数字位置指令15
    u16 _Resv461;            // P0461 
    u16 u16ProbeStatus;      // P0462 探针状态
    s64 s64ProbePosEdgePo00; // P0463 探针1上升沿锁存位置
    s64 s64ProbeNegEdgePo00; // P0467 探针1下降沿锁存位置
    s64 s64ProbePosEdgePo01; // P0471 探针2上升沿锁存位置
    s64 s64ProbeNegEdgePo01; // P0475 探针2下降沿锁存位置
    s64 s64ProbePosEdgePo02; // P0479 探针3上升沿锁存位置
    s64 s64ProbeNegEdgePo02; // P0483 探针3下降沿锁存位置
    s64 s64ProbePosEdgePo03; // P0487 探针4上升沿锁存位置
    s64 s64ProbeNegEdgePo03; // P0491 探针4下降沿锁存位置
    u16 _Resv495;            // P0495 
    u16 u16SpdRefSrc;        // P0496 速度指令来源
    s32 s32SpdDigRef;        // P0497 数字速度指令
    u16 _Resv499;            // P0499 
    u16 u16AccTime;          // P0500 加速时间
    u16 u16DecTime;          // P0501 减速时间
    u16 _Resv502;            // P0502 
    u16 u16JogDigRef;        // P0503 点动速度指令
    u16 u16JogAccDecTime;    // P0504 点动加减速时间
    u16 _Resv505;            // P0505 
    u16 u16SpdLimSrc;        // P0506 速度限制来源
    u16 u16SpdLimFwd;        // P0507 正向速度限制
    u16 u16SpdLimRev;        // P0508 反向速度限制
    u16 u16SpdLimSts;        // P0509 速度限制状态
    u16 _Resv510;            // P0510 
    u16 u16AiSpdCoeff00;     // P0511 
    u16 u16AiSpdCoeff01;     // P0512 
    u16 u16AiSpdCoeff02;     // P0513 
    u16 _Resv514;            // P0514 
    u16 _Resv515;            // P0515 
    u16 _Resv516;            // P0516 
    u16 _Resv517;            // P0517 
    u16 _Resv518;            // P0518 
    u16 _Resv519;            // P0519 
    u16 _Resv520;            // P0520 
    u16 _Resv521;            // P0521 
    s32 s32SpdDigRef00;      // P0522 多段数字速度指令00
    s32 s32SpdDigRef01;      // P0524 多段数字速度指令01
    s32 s32SpdDigRef02;      // P0526 多段数字速度指令02
    s32 s32SpdDigRef03;      // P0528 多段数字速度指令03
    s32 s32SpdDigRef04;      // P0530 多段数字速度指令04
    s32 s32SpdDigRef05;      // P0532 多段数字速度指令05
    s32 s32SpdDigRef06;      // P0534 多段数字速度指令06
    s32 s32SpdDigRef07;      // P0536 多段数字速度指令07
    s32 s32SpdDigRef08;      // P0538 多段数字速度指令08
    s32 s32SpdDigRef09;      // P0540 多段数字速度指令09
    s32 s32SpdDigRef10;      // P0542 多段数字速度指令10
    s32 s32SpdDigRef11;      // P0544 多段数字速度指令11
    s32 s32SpdDigRef12;      // P0546 多段数字速度指令12
    s32 s32SpdDigRef13;      // P0548 多段数字速度指令13
    s32 s32SpdDigRef14;      // P0550 多段数字速度指令14
    s32 s32SpdDigRef15;      // P0552 多段数字速度指令15
    u16 u16AccTime00;        // P0554 多段加速时间00
    u16 u16AccTime01;        // P0555 多段加速时间01
    u16 u16AccTime02;        // P0556 多段加速时间02
    u16 u16AccTime03;        // P0557 多段加速时间03
    u16 u16AccTime04;        // P0558 多段加速时间04
    u16 u16AccTime05;        // P0559 多段加速时间05
    u16 u16AccTime06;        // P0560 多段加速时间06
    u16 u16AccTime07;        // P0561 多段加速时间07
    u16 u16AccTime08;        // P0562 多段加速时间08
    u16 u16AccTime09;        // P0563 多段加速时间09
    u16 u16AccTime10;        // P0564 多段加速时间10
    u16 u16AccTime11;        // P0565 多段加速时间11
    u16 u16AccTime12;        // P0566 多段加速时间12
    u16 u16AccTime13;        // P0567 多段加速时间13
    u16 u16AccTime14;        // P0568 多段加速时间14
    u16 u16AccTime15;        // P0569 多段加速时间15
    u16 u16DecTime00;        // P0570 多段减速时间00
    u16 u16DecTime01;        // P0571 多段减速时间01
    u16 u16DecTime02;        // P0572 多段减速时间02
    u16 u16DecTime03;        // P0573 多段减速时间03
    u16 u16DecTime04;        // P0574 多段减速时间04
    u16 u16DecTime05;        // P0575 多段减速时间05
    u16 u16DecTime06;        // P0576 多段减速时间06
    u16 u16DecTime07;        // P0577 多段减速时间07
    u16 u16DecTime08;        // P0578 多段减速时间08
    u16 u16DecTime09;        // P0579 多段减速时间09
    u16 u16DecTime10;        // P0580 多段减速时间10
    u16 u16DecTime11;        // P0581 多段减速时间11
    u16 u16DecTime12;        // P0582 多段减速时间12
    u16 u16DecTime13;        // P0583 多段减速时间13
    u16 u16DecTime14;        // P0584 多段减速时间14
    u16 u16DecTime15;        // P0585 多段减速时间15
    u16 _Resv586;            // P0586 
    u16 _Resv587;            // P0587 
    u16 _Resv588;            // P0588 
    u16 _Resv589;            // P0589 
    u16 _Resv590;            // P0590 
    u16 _Resv591;            // P0591 
    u16 s16TrqDigRef00;      // P0592 多段数字转矩指令00
    u16 s16TrqDigRef01;      // P0593 多段数字转矩指令01
    u16 s16TrqDigRef02;      // P0594 多段数字转矩指令02
    u16 s16TrqDigRef03;      // P0595 多段数字转矩指令03
    u16 s16TrqDigRef04;      // P0596 多段数字转矩指令04
    u16 s16TrqDigRef05;      // P0597 多段数字转矩指令05
    u16 s16TrqDigRef06;      // P0598 多段数字转矩指令06
    u16 s16TrqDigRef07;      // P0599 多段数字转矩指令07
    u16 s16TrqDigRef08;      // P0600 多段数字转矩指令08
    u16 s16TrqDigRef09;      // P0601 多段数字转矩指令09
    u16 s16TrqDigRef10;      // P0602 多段数字转矩指令10
    u16 s16TrqDigRef11;      // P0603 多段数字转矩指令11
    u16 s16TrqDigRef12;      // P0604 多段数字转矩指令12
    u16 s16TrqDigRef13;      // P0605 多段数字转矩指令13
    u16 s16TrqDigRef14;      // P0606 多段数字转矩指令14
    u16 s16TrqDigRef15;      // P0607 多段数字转矩指令15
    u16 _Resv608;            // P0608 
    u16 _Resv609;            // P0609 
    u16 _Resv610;            // P0610 
    u16 u16OpenPeriod;       // P0611 开环运行周期
    s16 s16OpenElecAngInit;  // P0612 开环电角度初值
    s16 s16OpenElecAngInc;   // P0613 开环电角度自增量
    s16 s16OpenUqRef;        // P0614 开环D轴指令
    s16 s16OpenUdRef;        // P0615 开环Q轴指令
    u16 _Resv616;            // P0616 
    u16 _Resv617;            // P0617 
    u16 _Resv618;            // P0618 
    u16 _Resv619;            // P0619 
    u16 u16EncCmd;           // P0620 
    u16 u16EncErrCode;       // P0621 编码器错误代码
    u16 u16EncComErrSum;     // P0622 编码器通信错误次数
    u16 _Resv623;            // P0623 
    u16 _Resv624;            // P0624 
    u16 u16AxisFSM;          // P0625 轴状态机
    u16 _Resv626;            // P0626 
    u16 _Resv627;            // P0627 
    u16 _Resv628;            // P0628 
    u16 _Resv629;            // P0629 
    s32 s32DrvSpdRef;        // P0630 驱动层速度指令
    s32 s32DrvSpdFb;         // P0632 驱动层速度反馈
    u16 _Resv634;            // P0634 
    u16 _Resv635;            // P0635 
    u16 _Resv636;            // P0636 
    u16 u16ElecAngle;        // P0637 驱动层电角度
    u16 _Resv638;            // P0638 
    u16 _Resv639;            // P0639 
    u16 _Resv640;            // P0640 
    u16 _Resv641;            // P0641 
    s16 s16Ud;               // P0642 驱动层D轴电压输出
    s16 s16Uq;               // P0643 驱动层Q轴电压输出
    s16 s16Ualpha;           // P0644 驱动层Alpha轴电压输出
    s16 s16Ubeta;            // P0645 驱动层Beta轴电压输出
    u16 u16Sector;           // P0646 驱动层SVPWM矢量扇区
    u16 u16PwmaComp;         // P0647 驱动层A相PWM比较值
    u16 u16PwmbComp;         // P0648 驱动层B相PWM比较值
    u16 u16PwmcComp;         // P0649 驱动层C相PWM比较值
    u16 _Resv650;            // P0650 
    u16 u16StopCmd;          // P0651 停机指令
    u16 _Resv652;            // P0652 
    s32 s32LogicSpdRef;      // P0653 逻辑层速度指令
    u16 u16LogicSpdAccTime;  // P0655 逻辑层加速度时间
    u16 u16LogicSpdDecTime;  // P0656 逻辑层减速度时间
    u16 u16LogicSpdPlanMode; // P0657 逻辑层速度规划模式
    u16 u16JogCmd;           // P0658 点动指令使能信号
    u16 u16JogSpdRef;        // P0659 点动速度指令
    u16 _Resv660;            // P0660 
    u16 u16SpdAccTime;       // P0661 
    u16 u16SpdDecTime;       // P0662 
    u16 _Resv663;            // P0663 
    u16 u16MultMotionEn;     // P0664 多段模式启动
    u16 _Resv665;            // P0665 
    u16 u16SpdPlanMode;      // P0666 速度规划模式
    u16 u16StopPlanMode;     // P0667 停机模式
    u16 u16SpdMulRefSel;     // P0668 多段速度选择
    u16 _Resv669;            // P0669 
    s32 s32LogicSpdLimFwd;   // P0670 逻辑层正向速度限制
    s32 s32LogicSpdLimRev;   // P0672 逻辑层反向速度限制
    u16 _Resv674;            // P0674 
    s32 s32PlanSpdRef;       // P0675 规划层速度指令
    u16 _Resv677;            // P0677 
    u16 _Resv678;            // P0678 
    u16 _Resv679;            // P0679 
    u16 _Resv680;            // P0680 
    s16 s16IaFbSi;           // P0681 A相反馈电流物理值
    s16 s16IbFbSi;           // P0682 B相反馈电流物理值
    s16 s16IcFbSi;           // P0683 C相反馈电流物理值
    u16 _Resv684;            // P0684 
    s64 s64UserPosRef;       // P0685 用户位置指令
    s64 s64UserPosFb;        // P0689 用户位置反馈
    s32 s32UserSpdRef;       // P0693 用户速度指令
    s32 s32UserSpdFb;        // P0695 用户速度反馈
    s16 s16UserTrqRef;       // P0697 用户转矩指令
    s16 s16UserTrqFb;        // P0698 用户转矩反馈
    u16 _Resv699;            // P0699 
    u16 _Resv700;            // P0700 
    u16 _Resv701;            // P0701 
} axis_para_t;

// clang-format on

// 后续该生成工具时再去掉
#define aDeviceAttr sDeviceAttr
#define aAxisAttr   sAxisAttr

extern const para_attr_t aDeviceAttr[];
extern const para_attr_t aAxisAttr[];

extern device_para_t sDevicePara;
extern axis_para_t   aAxisPara[];

#define D          sDevicePara
#define P(eAxisNo) aAxisPara[eAxisNo]

#endif
