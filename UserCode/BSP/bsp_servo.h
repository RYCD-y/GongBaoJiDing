//
// Created by ye on 2026/10/27.
//
// 舵机驱动(BSP 层)
//   通过定时器 PWM 输出 50Hz 信号, 高电平 500~2500us 对应 0~180°
//   定时器句柄/通道全部作为入参传入, 驱动本身不绑定任何具体硬件,
//   同一份代码可同时驱动 TIM1/TIM8 等任意 PWM 通道
//
// 用法:
//   Servo_t servo;
//   Servo_Init(&servo, &htim1, TIM_CHANNEL_1);        // 0~180°, 500~2500us
//   Servo_SetAngle(&servo, 90.0f);                    // 转到中位
//
// 注意:
//   1. 定时器需由 CubeMX 先配置为 PWM 输出(推荐 50Hz, 1MHz 计数, 如 PSC=168-1 ARR=20000-1),
//      驱动会按实际 PSC/ARR 自动换算脉宽->计数值, 20ms 周期以外的配置同样可用
//   2. 若 PWM 周期短于最大脉宽(如 100kHz), 初始化会失败返回 1
//

#ifndef INC_2026GCXL_BSP_SERVO_H
#define INC_2026GCXL_BSP_SERVO_H

#include "main.h"
#include "stdint.h"

/* 常见模拟舵机(SG90/MG996R)默认标定 */
#define SERVO_DEFAULT_MIN_ANGLE 0.0f
#define SERVO_DEFAULT_MAX_ANGLE 180.0f
#define SERVO_DEFAULT_MIN_PULSE 500.0f
#define SERVO_DEFAULT_MAX_PULSE 2500.0f

/* 舵机标定参数: 角度与脉宽的对应关系 */
typedef struct
{
        float   min_angle;  // 最小角度(°), 对应 min_pulse
        float   max_angle;  // 最大角度(°), 对应 max_pulse
        float   min_pulse;  // 最小角度对应的高电平脉宽(us)
        float   max_pulse;  // 最大角度对应的高电平脉宽(us)
        uint8_t reversed;   // 1: 角度反向(角度增大时脉宽减小)
} Servo_Config_t;

/* 舵机对象 */
typedef struct
{
        /* ---- 硬件端口(全部由调用方作为入参传入) ---- */
        TIM_HandleTypeDef *htim;      // PWM 定时器句柄
        uint32_t           channel;   // PWM 通道, 如 TIM_CHANNEL_1

        /* ---- 标定参数 ---- */
        Servo_Config_t cfg;

        /* ---- 运行状态 ---- */
        float   tick_per_us; // 1us 对应的定时器计数值
        float   angle;       // 当前目标角度(°)
        float   pulse;       // 当前目标脉宽(us)
        uint8_t inited;      // 初始化完成
        uint8_t enabled;     // PWM 已启动
} Servo_t;

/*===| 初始化 |===*/

// 使用默认标定(0~180°, 500~2500us)初始化并启动 PWM
// 返回 0 成功, 1 参数/配置错误
uint8_t Servo_Init(Servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel);

// 使用自定义标定初始化并启动 PWM
uint8_t Servo_InitEx(Servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel, const Servo_Config_t *cfg);

/*===| PWM 启停 |===*/

// 启动 PWM 输出
uint8_t Servo_Enable(Servo_t *servo);

// 停止 PWM 输出, 舵机失去保持力矩
void Servo_Disable(Servo_t *servo);

/*===| 控制 |===*/

// 设置角度(°), 超出标定范围自动限幅; 返回 0 成功
uint8_t Servo_SetAngle(Servo_t *servo, float angle);

// 直接设置脉宽(us), 超出标定范围自动限幅; 返回 0 成功
uint8_t Servo_SetPulse(Servo_t *servo, float pulse_us);

// 读取当前目标角度/脉宽
float Servo_GetAngle(const Servo_t *servo);
float Servo_GetPulse(const Servo_t *servo);

#endif //INC_2026GCXL_BSP_SERVO_H
