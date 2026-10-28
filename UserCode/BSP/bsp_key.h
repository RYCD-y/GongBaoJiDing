//
// Created by ye on 2026/10/29.
//
// 按键驱动(BSP 层), 移植自 C_Boart_IMU/Program/BSP/Key.c
//   20ms 周期扫描, 统计短按/长按次数并记录按下顺序, 可用于单击/双击/长按组合
//   端口/有效电平由调用方作为入参传入, GPIO 需由 CubeMX 先行配置为上拉输入
//
// 用法:
//   Key_Init(Key_GPIO_Port, Key_Pin, GPIO_PIN_RESET);    // 按下为低电平
//   Key_SetEventHandler(OnKeyIdle);                     // 松手空闲 1s 后回调
//   xTaskCreate(Key_Task, "KEY", 128, NULL, 3, NULL);   // 启动扫描任务
//
// 回调触发时按键记录仍然有效, 可在回调内用 Key_GetShortPressNum()/
// Key_GetLongPressNum()/Key_GetPressOrder() 判断手势, 回调返回后记录自动清零
//

#ifndef INC_2026GCXL_BSP_KEY_H
#define INC_2026GCXL_BSP_KEY_H

#include "main.h"
#include "stdint.h"

#define KEY_SCAN_PERIOD_MS  20  // 扫描周期(ms), 需与 Key_Task 一致
#define KEY_LONG_PRESS_CNT  10  // 连续按下超过该计数视为长按: 10 * 20ms = 200ms
#define KEY_IDLE_CNT        50  // 松手空闲超过该计数触发事件: 50 * 20ms = 1s
#define KEY_PRESS_ORDER_MAX 16  // 按下顺序记录深度

#define KEY_PRESS_SHORT 1  // 短按
#define KEY_PRESS_LONG  2  // 长按

typedef void (*Key_EventHandler)(void);

// 初始化按键(端口/有效电平作为入参), 不配置 GPIO
void Key_Init(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState active_level);

// 设置松手空闲后的回调, 传 NULL 取消
void Key_SetEventHandler(Key_EventHandler handler);

// 扫描一次按键(需每 KEY_SCAN_PERIOD_MS 调用一次)
void Key_Scan(void);

// 清零短按/长按/按下顺序记录
void Key_Clear(void);

// 读取本次手势记录
uint8_t Key_GetShortPressNum(void);
uint8_t Key_GetLongPressNum(void);
uint8_t Key_GetPressOrder(uint8_t index);  // 0: 越界, 1: 短按, 2: 长按

// FreeRTOS 扫描任务, 内部按 KEY_SCAN_PERIOD_MS 周期调用 Key_Scan
void Key_Task(void *argument);

#endif //INC_2026GCXL_BSP_KEY_H
