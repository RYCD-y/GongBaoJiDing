//
// Created by ye on 2026/10/29.
//
// 蜂鸣器驱动(BSP 层), 移植自 C_Boart_IMU/Program/BSP/Buzzer_Task.c
//   通过改变 PWM 频率发声(TIM4 CH3), 保留音调表与预设音效
//   定时器/通道由调用方作为入参传入
//
// 用法:
//   Buzzer_Init(&htim4, TIM_CHANNEL_3);
//   Buzzer_SetSoundEffect(Buzzer_SoundEffect_SystemStart);
//   xTaskCreate(Buzzer_Task, "BUZ", 256, NULL, 3, NULL);
//

#ifndef INC_2026GCXL_BSP_BUZZER_H
#define INC_2026GCXL_BSP_BUZZER_H

#include "main.h"
#include "stdint.h"

#define BUZZER_TASK_PERIOD_MS 10  // 音效任务空闲节拍(ms), 建议不超过 30
#define BUZZER_PSC            50  // 发声用预分频, 与 168MHz 主频配合

/*===| 预设音效 |===*/
typedef enum
{
        Buzzer_SoundEffect_OFF = 0,
        Buzzer_SoundEffect_SystemStart,
        Buzzer_SoundEffect_SuperCap_ON,
        Buzzer_SoundEffect_SuperCap_OFF,
} Buzzer_SoundEffect_t;

/*===| 音调 |===*/
typedef enum
{
        P = 0,  // 休止
        L1, L2, L3, L4, L5, L6, L7,
        M1, M2, M3, M4, M5, M6, M7,
        H1, H2, H3, H4, H5, H6, H7,
} Buzzer_Tone_t;

// 初始化并启动 PWM
void Buzzer_Init(TIM_HandleTypeDef *htim, uint32_t channel);

// 启动/停止 PWM 输出
void Buzzer_Start(void);
void Buzzer_Stop(void);

// 设置音调(休止符 P 输出 0 占空比)
void Buzzer_SetTone(Buzzer_Tone_t tone);

// 直接设置发声频率(Hz), freq_hz < 20 视为休止
void Buzzer_SetFrequency(float freq_hz);

// 设置预设音效, 由 Buzzer_Task 播放
void Buzzer_SetSoundEffect(Buzzer_SoundEffect_t effect);

// FreeRTOS 音效播放任务
void Buzzer_Task(void *argument);

#endif //INC_2026GCXL_BSP_BUZZER_H
