
// clang-format on

// 后续该生成工具时再去掉
#define aDeviceAttr sDeviceAttr
#define aAxisAttr   sAxisAttr

extern const para_attr_t aDeviceAttr[];
extern const para_attr_t aAxisAttr[];

extern device_para_t sDevicePara;
extern axis_para_t   aAxisPara[];

#define D         sDevicePara
#define P(AxisNo) aAxisPara[AxisNo]

#endif
