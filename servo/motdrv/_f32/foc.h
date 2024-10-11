#ifndef __FOC_H__ // 条件编译指令，防止头文件重复包含
#define __FOC_H__

#include <math.h>   // 包含数学函数库，例如 sin() 和 cos()
#include <stdint.h> // 包含整数类型定义，例如 uint8_t

typedef struct
{              // 定义 FOC 结构体类型
  float u_d;   // 控制变量 u_d
  float u_q;   // 控制变量 u_q
  float theta; // 角度 theta

  float u_alpha; // 变换后的控制变量 u_alpha
  float u_beta;  // 变换后的控制变量 u_beta

  float t_a; // 电压矢量 t_a
  float t_b; // 电压矢量 t_b
  float t_c; // 电压矢量 t_c

  float i_a; // 电流 i_a
  float i_b; // 电流 i_b
  float i_c; // 电流 i_c

  float i_alpha; // 变换后的电流 i_alpha
  float i_beta;  // 变换后的电流 i_beta

  float i_d; // d 轴电流 i_d
  float i_q; // q 轴电流 i_q
} FOC;       // FOC 结构体类型声明

void ipark(FOC *foc);  // Inverse Park 反 Park 变换函数声明
void ipark2(FOC *foc); // Inverse Park 反 Park 变换函数声明（另一种实现方式）
void clarke(FOC *foc); // Clarke 变换函数声明
void park(FOC *foc);   // Park 变换函数声明
void svpwm(FOC *foc);  // Space Vector PWM (SVPWM) 函数声明

#endif // FOC_H                  // 结束条件编译指令
