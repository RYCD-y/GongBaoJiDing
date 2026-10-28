//
// Created by ye on 2026/10/29.
//
// RGB LED 驱动(BSP 层), 移植自 C_Boart_IMU/Program/BSP/LED.c
//   三路 PWM(TIM5 CH1/2/3) 调光, 支持直接设 RGB/HSV 与彩虹流水任务
//   定时器/通道由调用方作为入参传入
//
// 用法:
//   // PH12=R, PH11=G, PH10=B (与 C_Boart_IMU 接线一致)
//   LED_Init(&htim5, TIM_CHANNEL_3, TIM_CHANNEL_2, TIM_CHANNEL_1);
//   LED_SetRGB(255, 0, 0);
//   xTaskCreate(LED_Task, "LED", 128, NULL, 2, NULL);   // 彩虹流水
//

#ifndef INC_2026GCXL_BSP_LED_H
#define INC_2026GCXL_BSP_LED_H

#include "main.h"
#include "stdint.h"

#define LED_TASK_PERIOD_MS 20  // 流水灯步进周期(ms)

typedef enum
{
        LED_R = 0,
        LED_G = 1,
        LED_B = 2,
} LED_Color;

// 初始化三路 PWM 并熄灭
void LED_Init(TIM_HandleTypeDef *htim, uint32_t ch_r, uint32_t ch_g, uint32_t ch_b);

// 直接设置 RGB(0~255), 按定时器 ARR 自动缩放占空比
void LED_SetRGB(uint8_t r, uint8_t g, uint8_t b);

// 设置 HSV: h 0~360(色环角度), s/v 0~100
void LED_SetHSV(uint16_t h, uint8_t s, uint8_t v);

// 单色亮度控制, light 0~255
void LED_ON(LED_Color color, uint16_t light);
void LED_OFF(LED_Color color);

// 彩虹流水: 色环前进 step 度
void LED_Rainbow(uint16_t step);

// FreeRTOS 彩虹流水任务
void LED_Task(void *argument);

#endif //INC_2026GCXL_BSP_LED_H
