#ifndef __PARA_TABLE_H__
#define __PARA_TABLE_H__

#include "paraattr.h"

typedef enum {
    AXIS_0,
    AXIS_1,
    AXIS_2,
    // ...
} axis_e;

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
    u32 u32SysRunTime;       // P0036 运行时间
    u16 _Resv38;             // P0038 
    u16 u16MbSlvId;          // P0039 Modbus-从站节点ID
    u32 u32MbBaudrate;       // P0040 Modbus-从站波特率
    u16 u16MbParity;         // P0042 Modbus-从站检验位
    u16 u16MbMstEndian;      // P0043 Modbus-主站大小端
    u16 u16MbDisconnTime;    // P0044 Modbus-通讯断开检测时间
    u16 u16MbResponseDelay;  // P0045 Modbus-命令响应延时 
    u16 _Resv46;             // P0046 
    u16 u16CopNodeId;        // P0047 CANopen-节点ID
    u16 u16CopBitrate;       // P0048 CANopen-比特率
    u16 u16PdoInhTime;       // P0049 CANopen-PDO禁止时间
    u16 u16CopState;         // P0050 CANopen-通信状态
    u16 _Resv51;             // P0051 
    u16 u16EcatScanPrd;      // P0052 EtherCAT-通信状态
    u16 u16EcatState;        // P0053 EtherCAT-Port0错误统计
    u16 u16EcatPort0ErrCnt;  // P0054 EtherCAT-Port1错误统计
    u16 u16EcatPort1ErrCnt;  // P0055 EtherCAT-接收错误统计
    u16 u16EcatRxErrCnt;     // P0056 EtherCAT-处理单元错误统计
    u16 u16EcatPdiErrCnt;    // P0057 
    u16 _Resv58;             // P0058 
    u16 _Resv59;             // P0059 
    u16 _Resv60;             // P0060 
    u16 u16ScopeSampPrd;     // P0061 数据记录采样周期设定
    u16 u16ScopeSampPts;     // P0062 数据记录采样点数设定
    u16 u16ScopeChCnt;       // P0063 数据记录通道数量设定
    u16 u16ScopeChSrc00;     // P0064 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc01;     // P0065 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc02;     // P0066 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc03;     // P0067 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc04;     // P0068 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc05;     // P0069 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc06;     // P0070 数据记录通道来源设定 (数据源)
    u16 u16ScopeChSrc07;     // P0071 数据记录通道来源设定 (数据源)
    u16 u16ScopeChAddr00;    // P0072 数据记录通道地址设定
    u16 u16ScopeChAddr01;    // P0073 数据记录通道地址设定
    u16 u16ScopeChAddr02;    // P0074 数据记录通道地址设定
    u16 u16ScopeChAddr03;    // P0075 数据记录通道地址设定
    u16 u16ScopeChAddr04;    // P0076 数据记录通道地址设定
    u16 u16ScopeChAddr05;    // P0077 数据记录通道地址设定
    u16 u16ScopeChAddr06;    // P0078 数据记录通道地址设定
    u16 u16ScopeChAddr07;    // P0079 数据记录通道地址设定
    u16 u16ScopeSampIdx;     // P0080 当前数据记录索引
    u16 u16ScopeSampSts;     // P0081 数据记录状态
    u16 _Resv82;             // P0082 
    u16 _Resv83;             // P0083 
    u16 _Resv84;             // P0084 
    u16 _Resv85;             // P0085 
    u16 _Resv86;             // P0086 
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
    u16 u16UmdcHwCoeff;      // P0147 主回路电压采样系数
    u16 u16UcdcHwCoeff;      // P0148 控制电电压采样系数
    u16 u16Uai0HwCoeff;      // P0149 
    u16 u16Uai1HwCoeff;      // P0150 
    u16 u16Uai2HwCoeff;      // P0151 
    u16 u16UTempHwCoeff;     // P0152 
    u16 _Resv153;            // P0153 
    u16 _Resv154;            // P0154 
    u16 u16DiState;          // P0155 DI输入端子电平状态
    u16 u16DoState;          // P0156 DO输出端子电平状态
    u16 _Resv157;            // P0157 
    s16 s16UaiPu0;           // P0158 AI0输入电压数字量
    s16 s16UaiPu1;           // P0159 AI1输入电压数字量
    s16 s16UaiPu2;           // P0160 AI2输入电压数字量
    u16 _Resv161;            // P0161 
    s16 s16UaiSi0;           // P0162 AI0输入电压
    s16 s16UaiSi1;           // P0163 AI1输入电压
    s16 s16UaiSi2;           // P0164 AI2输入电压
    u16 u16UaoSi0;           // P0165 AO0输出电压
    u16 u16UaoSi1;           // P0166 AO1输出电压
    u16 u16UaoSi2;           // P0167 AO2输出电压
    u16 _Resv168;            // P0168 
    u16 _Resv169;            // P0169 
    u16 u16UmdcSi;           // P0170 主回路母线电压物理值
    u16 u16UcdcSi;           // P0171 控制母线电压物理值
    s16 s16TempIpmSi;        // P0172 逆变桥温度物理值
    s16 s16TempRectSi;       // P0173 整流桥温度物理值
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
    u16 _Resv184;            // P0184 
    u16 _Resv185;            // P0185 
    u16 _Resv186;            // P0186 
    u16 _Resv187;            // P0187 
    u16 _Resv188;            // P0188 
    u16 _Resv189;            // P0189 
    u16 _Resv190;            // P0190 
    u16 _Resv191;            // P0191 
    u16 _Resv192;            // P0192 
    u16 _Resv193;            // P0193 
    u16 _Resv194;            // P0194 
    u16 _Resv195;            // P0195 
    u16 _Resv196;            // P0196 
    u16 _Resv197;            // P0197 
    u16 _Resv198;            // P0198 
    u16 _Resv199;            // P0199 
    u16 _Resv200;            // P0200 
    u16 _Resv201;            // P0201 
    u16 _Resv202;            // P0202 
    u16 _Resv203;            // P0203 
    u16 _Resv204;            // P0204 
    u16 _Resv205;            // P0205 
    u16 u16AlmCode01;        // P0206 历史报警代码01
    u16 u16AlmCode02;        // P0207 历史报警代码02
    u16 u16AlmCode03;        // P0208 历史报警代码03
    u16 u16AlmCode04;        // P0209 历史报警代码04
    u16 u16AlmCode05;        // P0210 历史报警代码05
    u16 u16AlmCode06;        // P0211 历史报警代码06
    u16 u16AlmCode07;        // P0212 历史报警代码07
    u16 u16AlmCode08;        // P0213 历史报警代码08
    u16 u16AlmCode09;        // P0214 历史报警代码09
    u16 u16AlmCode10;        // P0215 历史报警代码10
    u16 u16AlmCode11;        // P0216 历史报警代码11
    u16 u16AlmCode12;        // P0217 历史报警代码12
    u16 u16AlmCode13;        // P0218 历史报警代码13
    u16 u16AlmCode14;        // P0219 历史报警代码14
    u16 u16AlmCode15;        // P0220 历史报警代码15
    u32 u32AlmTime00;        // P0221 历史报警时间戳00
    u32 u32AlmTime01;        // P0223 历史报警时间戳01
    u32 u32AlmTime02;        // P0225 历史报警时间戳02
    u32 u32AlmTime03;        // P0227 历史报警时间戳03
    u32 u32AlmTime04;        // P0229 历史报警时间戳04
    u32 u32AlmTime05;        // P0231 历史报警时间戳05
    u32 u32AlmTime06;        // P0233 历史报警时间戳06
    u32 u32AlmTime07;        // P0235 历史报警时间戳07
    u32 u32AlmTime08;        // P0237 历史报警时间戳08
    u32 u32AlmTime09;        // P0239 历史报警时间戳09
    u32 u32AlmTime10;        // P0241 历史报警时间戳10
    u32 u32AlmTime11;        // P0243 历史报警时间戳11
    u32 u32AlmTime12;        // P0245 历史报警时间戳12
    u32 u32AlmTime13;        // P0247 历史报警时间戳13
    u32 u32AlmTime14;        // P0249 历史报警时间戳14
    u32 u32AlmTime15;        // P0251 历史报警时间戳15
    u16 _Resv253;            // P0253 
    u16 _Resv254;            // P0254 
    u16 _Resv255;            // P0255 
    u16 _Resv256;            // P0256 
    u16 _Resv257;            // P0257 
    u16 _Resv258;            // P0258 
    u16 _Resv259;            // P0259 
    u16 u16ProbeStatus;      // P0260 探针状态
    s64 s64ProbePosEdgePo00; // P0261 探针1上升沿锁存位置
    s64 s64ProbeNegEdgePo00; // P0265 探针1下降沿锁存位置
    s64 s64ProbePosEdgePo01; // P0269 探针2上升沿锁存位置
    s64 s64ProbeNegEdgePo01; // P0273 探针2下降沿锁存位置
    s64 s64ProbePosEdgePo02; // P0277 探针3上升沿锁存位置
    s64 s64ProbeNegEdgePo02; // P0281 探针3下降沿锁存位置
    s64 s64ProbePosEdgePo03; // P0285 探针4上升沿锁存位置
    s64 s64ProbeNegEdgePo03; // P0289 探针4下降沿锁存位置
    u16 _Resv293;            // P0293 
    u16 _Resv294;            // P0294 
    u16 u16PosLoopIsrTime;   // P0295 
    u16 u16SpdLoopIsrTime;   // P0296 
    u16 u16CurLoopIsrTime;   // P0297 
    u16 u16LoopTick;         // P0298 
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
    u16 _Resv319;            // P0319 _
    u16 _Resv320;            // P0320 _
    u16 _Resv321;            // P0321 _
    u16 u16PosLoopKp;        // P0322 位置环增益系数
    u16 u16PosLoopKi;        // P0323 位置环积分系数
    u16 u16PosLoopKd;        // P0324 位置环微分系数
    u16 u16SpdLoopKp;        // P0325 速度环增益系数
    u16 u16SpdLoopKi;        // P0326 速度环积分系数
    u16 u16SpdLoopKd;        // P0327 速度环微分系数
    u16 u16CurLoopIqKp;      // P0328 电流环转矩增益系数
    u16 u16CurLoopIqKi;      // P0329 电流环转矩积分系数
    u16 u16CurLoopIqKd;      // P0330 电流环转矩增益系数
    u16 u16CurLoopIdKp;      // P0331 电流环磁链增益系数
    u16 u16CurLoopIdKi;      // P0332 电流环磁链积分系数
    u16 u16CurLoopIdKd;      // P0333 电流环磁链增益系数
    u16 _Resv334;            // P0334 _
    u16 u16CarryFreq;        // P0335 载波频率
    u16 u16CurSampType;      // P0336 电流采样类型
    u16 _Resv337;            // P0337 
    u16 _Resv338;            // P0338 
    u16 _Resv339;            // P0339 
    u16 _Resv340;            // P0340 
    u16 u16AppSel;           // P0341 应用选择
    u16 u16CtrlMethod;       // P0342 控制方法
    u16 u16CtrlMode;         // P0343 控制模式
    u16 u16CtrlCmdSrc;       // P0344 控制命令来源
    u16 u16PwrOnAutoRun;     // P0345 上电自使能
    u16 u16ServoOn;          // P0346 伺服使能
    u16 _Resv347;            // P0347 
    u16 u16DiSrc;            // P0348 外部或虚拟DI选择
    u16 u16DoSrc;            // P0349 外部或虚拟DO选择
    u16 _Resv350;            // P0350 
    u16 _Resv351;            // P0351 
    u16 u16PosLimSrc;        // P0352 位置指令来源
    s64 s64PosLimFwd;        // P0353 数字位置指令
    s64 s64PosLimRev;        // P0357 位置规划模式
    u16 u16PosLimSts;        // P0361 位置规划状态
    u16 _Resv362;            // P0362 
    u32 u32ElecGearNum;      // P0363 电子齿轮比分子
    u32 u32ElecGearDeno;     // P0365 电子齿轮比分母
    u16 _Resv367;            // P0367 
    u16 u16EncFreqDivDir;    // P0368 编码器分频输出脉冲方向
    u16 u16EncFreqDivNum;    // P0369 编码器分频输出分子
    u16 u16EncFreqDivDeno;   // P0370 编码器分频输出分母
    u16 _Resv371;            // P0371 
    s64 s64PosDigRef00;      // P0372 多段数字位置指令00
    s64 s64PosDigRef01;      // P0376 多段数字位置指令01
    s64 s64PosDigRef02;      // P0380 多段数字位置指令02
    s64 s64PosDigRef03;      // P0384 多段数字位置指令03
    s64 s64PosDigRef04;      // P0388 多段数字位置指令04
    s64 s64PosDigRef05;      // P0392 多段数字位置指令05
    s64 s64PosDigRef06;      // P0396 多段数字位置指令06
    s64 s64PosDigRef07;      // P0400 多段数字位置指令07
    s64 s64PosDigRef08;      // P0404 多段数字位置指令08
    s64 s64PosDigRef09;      // P0408 多段数字位置指令09
    s64 s64PosDigRef10;      // P0412 多段数字位置指令10
    s64 s64PosDigRef11;      // P0416 多段数字位置指令11
    s64 s64PosDigRef12;      // P0420 多段数字位置指令12
    s64 s64PosDigRef13;      // P0424 多段数字位置指令13
    s64 s64PosDigRef14;      // P0428 多段数字位置指令14
    s64 s64PosDigRef15;      // P0432 多段数字位置指令15
    u16 _Resv436;            // P0436 
    u16 u16ProbeStatus;      // P0437 探针状态
    s64 s64ProbePosEdgePo00; // P0438 探针1上升沿锁存位置
    s64 s64ProbeNegEdgePo00; // P0442 探针1下降沿锁存位置
    s64 s64ProbePosEdgePo01; // P0446 探针2上升沿锁存位置
    s64 s64ProbeNegEdgePo01; // P0450 探针2下降沿锁存位置
    s64 s64ProbePosEdgePo02; // P0454 探针3上升沿锁存位置
    s64 s64ProbeNegEdgePo02; // P0458 探针3下降沿锁存位置
    s64 s64ProbePosEdgePo03; // P0462 探针4上升沿锁存位置
    s64 s64ProbeNegEdgePo03; // P0466 探针4下降沿锁存位置
    u16 _Resv470;            // P0470 
    u16 u16SpdRefSrc;        // P0471 速度指令来源
    s32 s32SpdDigRef;        // P0472 数字速度指令
    u16 _Resv474;            // P0474 
    u16 u16AccTime;          // P0475 加速时间
    u16 u16DecTime;          // P0476 减速时间
    u16 _Resv477;            // P0477 
    u16 u16JogDigRef;        // P0478 点动速度指令
    u16 u16JogAccDecTime;    // P0479 点动加减速时间
    u16 _Resv480;            // P0480 
    u16 u16SpdLimSrc;        // P0481 速度限制来源
    u16 u16SpdLimFwd;        // P0482 正向速度限制
    u16 u16SpdLimRev;        // P0483 反向速度限制
    u16 u16SpdLimSts;        // P0484 速度限制状态
    u16 _Resv485;            // P0485 
    u16 u16AiSpdCoeff00;     // P0486 
    u16 u16AiSpdCoeff01;     // P0487 
    u16 u16AiSpdCoeff02;     // P0488 
    u16 _Resv489;            // P0489 
    u16 _Resv490;            // P0490 
    u16 _Resv491;            // P0491 
    u16 _Resv492;            // P0492 
    u16 _Resv493;            // P0493 
    u16 _Resv494;            // P0494 
    u16 _Resv495;            // P0495 
    u16 _Resv496;            // P0496 
    s32 s32SpdDigRef00;      // P0497 多段数字速度指令00
    s32 s32SpdDigRef01;      // P0499 多段数字速度指令01
    s32 s32SpdDigRef02;      // P0501 多段数字速度指令02
    s32 s32SpdDigRef03;      // P0503 多段数字速度指令03
    s32 s32SpdDigRef04;      // P0505 多段数字速度指令04
    s32 s32SpdDigRef05;      // P0507 多段数字速度指令05
    s32 s32SpdDigRef06;      // P0509 多段数字速度指令06
    s32 s32SpdDigRef07;      // P0511 多段数字速度指令07
    s32 s32SpdDigRef08;      // P0513 多段数字速度指令08
    s32 s32SpdDigRef09;      // P0515 多段数字速度指令09
    s32 s32SpdDigRef10;      // P0517 多段数字速度指令10
    s32 s32SpdDigRef11;      // P0519 多段数字速度指令11
    s32 s32SpdDigRef12;      // P0521 多段数字速度指令12
    s32 s32SpdDigRef13;      // P0523 多段数字速度指令13
    s32 s32SpdDigRef14;      // P0525 多段数字速度指令14
    s32 s32SpdDigRef15;      // P0527 多段数字速度指令15
    u16 u16AccTime00;        // P0529 多段加速时间00
    u16 u16AccTime01;        // P0530 多段加速时间01
    u16 u16AccTime02;        // P0531 多段加速时间02
    u16 u16AccTime03;        // P0532 多段加速时间03
    u16 u16AccTime04;        // P0533 多段加速时间04
    u16 u16AccTime05;        // P0534 多段加速时间05
    u16 u16AccTime06;        // P0535 多段加速时间06
    u16 u16AccTime07;        // P0536 多段加速时间07
    u16 u16AccTime08;        // P0537 多段加速时间08
    u16 u16AccTime09;        // P0538 多段加速时间09
    u16 u16AccTime10;        // P0539 多段加速时间10
    u16 u16AccTime11;        // P0540 多段加速时间11
    u16 u16AccTime12;        // P0541 多段加速时间12
    u16 u16AccTime13;        // P0542 多段加速时间13
    u16 u16AccTime14;        // P0543 多段加速时间14
    u16 u16AccTime15;        // P0544 多段加速时间15
    u16 u16DecTime00;        // P0545 多段减速时间00
    u16 u16DecTime01;        // P0546 多段减速时间01
    u16 u16DecTime02;        // P0547 多段减速时间02
    u16 u16DecTime03;        // P0548 多段减速时间03
    u16 u16DecTime04;        // P0549 多段减速时间04
    u16 u16DecTime05;        // P0550 多段减速时间05
    u16 u16DecTime06;        // P0551 多段减速时间06
    u16 u16DecTime07;        // P0552 多段减速时间07
    u16 u16DecTime08;        // P0553 多段减速时间08
    u16 u16DecTime09;        // P0554 多段减速时间09
    u16 u16DecTime10;        // P0555 多段减速时间10
    u16 u16DecTime11;        // P0556 多段减速时间11
    u16 u16DecTime12;        // P0557 多段减速时间12
    u16 u16DecTime13;        // P0558 多段减速时间13
    u16 u16DecTime14;        // P0559 多段减速时间14
    u16 u16DecTime15;        // P0560 多段减速时间15
    u16 _Resv561;            // P0561 
    u16 _Resv562;            // P0562 
    u16 _Resv563;            // P0563 
    u16 _Resv564;            // P0564 
    u16 _Resv565;            // P0565 
    u16 _Resv566;            // P0566 
    u16 _Resv567;            // P0567 
    u16 _Resv568;            // P0568 
    u16 _Resv569;            // P0569 
    u16 _Resv570;            // P0570 
    u16 _Resv571;            // P0571 
    u16 _Resv572;            // P0572 
    u16 _Resv573;            // P0573 
    u16 _Resv574;            // P0574 
    u16 _Resv575;            // P0575 
    u16 _Resv576;            // P0576 
    u16 _Resv577;            // P0577 
    u16 _Resv578;            // P0578 
    u16 _Resv579;            // P0579 
    u16 _Resv580;            // P0580 
    u16 _Resv581;            // P0581 
    u16 _Resv582;            // P0582 
    u16 _Resv583;            // P0583 
    u16 _Resv584;            // P0584 
    u16 _Resv585;            // P0585 
    s16 s16TrqDigRef00;      // P0586 多段数字转矩指令00
    s16 s16TrqDigRef01;      // P0587 多段数字转矩指令01
    s16 s16TrqDigRef02;      // P0588 多段数字转矩指令02
    s16 s16TrqDigRef03;      // P0589 多段数字转矩指令03
    s16 s16TrqDigRef04;      // P0590 多段数字转矩指令04
    s16 s16TrqDigRef05;      // P0591 多段数字转矩指令05
    s16 s16TrqDigRef06;      // P0592 多段数字转矩指令06
    s16 s16TrqDigRef07;      // P0593 多段数字转矩指令07
    s16 s16TrqDigRef08;      // P0594 多段数字转矩指令08
    s16 s16TrqDigRef09;      // P0595 多段数字转矩指令09
    s16 s16TrqDigRef10;      // P0596 多段数字转矩指令10
    s16 s16TrqDigRef11;      // P0597 多段数字转矩指令11
    s16 s16TrqDigRef12;      // P0598 多段数字转矩指令12
    s16 s16TrqDigRef13;      // P0599 多段数字转矩指令13
    s16 s16TrqDigRef14;      // P0600 多段数字转矩指令14
    s16 s16TrqDigRef15;      // P0601 多段数字转矩指令15
    u16 _Resv602;            // P0602 
    u16 _Resv603;            // P0603 
    u16 _Resv604;            // P0604 
    u16 _Resv605;            // P0605 
    u16 _Resv606;            // P0606 
    u16 _Resv607;            // P0607 
    u16 _Resv608;            // P0608 
    u16 _Resv609;            // P0609 
    u16 _Resv610;            // P0610 
    u16 _Resv611;            // P0611 
    u16 _Resv612;            // P0612 
    s32 s32DrvSpdRef;        // P0613 驱动层速度指令
    s32 s32DrvSpdFb;         // P0615 驱动层速度反馈
    u16 _Resv617;            // P0617 
    u16 _Resv618;            // P0618 
    u16 _Resv619;            // P0619 
    u16 u16ElecAngle;        // P0620 驱动层电角度
    u16 _Resv621;            // P0621 
    u16 _Resv622;            // P0622 
    u16 _Resv623;            // P0623 
    u16 _Resv624;            // P0624 
    s16 s16Ud;               // P0625 驱动层D轴电压输出
    s16 s16Uq;               // P0626 驱动层Q轴电压输出
    s16 s16Ualpha;           // P0627 驱动层Alpha轴电压输出
    s16 s16Ubeta;            // P0628 驱动层Beta轴电压输出
    u16 u16Sector;           // P0629 驱动层SVPWM矢量扇区
    u16 u16PwmaComp;         // P0630 驱动层A相PWM比较值
    u16 u16PwmbComp;         // P0631 驱动层B相PWM比较值
    u16 u16PwmcComp;         // P0632 驱动层C相PWM比较值
    u16 _Resv633;            // P0633 
    u16 _Resv634;            // P0634 
    u16 _Resv635;            // P0635 
    u16 _Resv636;            // P0636 
    u16 _Resv637;            // P0637 
    u16 _Resv638;            // P0638 
    u16 _Resv639;            // P0639 
    s16 s16IaFbSi;           // P0640 A相反馈电流物理值
    s16 s16IbFbSi;           // P0641 B相反馈电流物理值
    s16 s16IcFbSi;           // P0642 C相反馈电流物理值
    u16 _Resv643;            // P0643 
    s64 s64UserPosRef;       // P0644 用户位置指令
    s64 s64UserPosFb;        // P0648 用户位置反馈
    s32 s32UserSpdRef;       // P0652 用户速度指令
    s32 s32UserSpdFb;        // P0654 用户速度反馈
    s16 s16UserTrqRef;       // P0656 用户转矩指令
    s16 s16UserTrqFb;        // P0657 用户转矩反馈
    u16 _Resv658;            // P0658 
    u16 _Resv659;            // P0659 
    u16 _Resv660;            // P0660 
    u16 _Resv661;            // P0661 
    u16 _Resv662;            // P0662 
    u16 _Resv663;            // P0663 
    u16 _Resv664;            // P0664 
} axis_para_t;

// clang-format on

// 后续该生成工具时再去掉
#define aDeviceAttr sDeviceAttr
#define aAxisAttr   sAxisAttr

extern const para_attr_t aDeviceAttr[];
extern const para_attr_t aAxisAttr[];

extern device_para_t sDevicePara;
extern axis_para_t   aAxisPara[];

#define D       sDevicePara
#define P(axis) aAxisPara[axis]

#endif
