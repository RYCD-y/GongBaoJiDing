//
// Created by ye on 2026/10/29.
//
// 定时器时钟/分辨率换算(供各 BSP 驱动共用)
//   定时器挂在 APB1/APB2 上, 当 APB 预分频不为 1 时, 定时器计数时钟为 PCLK 的 2 倍
//

#ifndef INC_2026GCXL_BSP_TIM_H
#define INC_2026GCXL_BSP_TIM_H

#include "main.h"
#include "stdint.h"

// 获取定时器计数时钟(Hz)
uint32_t BSP_TIM_GetClockFreq(const TIM_TypeDef *tim);

// 获取 1us 对应的计数值(已计入 PSC)
float BSP_TIM_GetTicksPerUs(const TIM_HandleTypeDef *htim);

#endif //INC_2026GCXL_BSP_TIM_H
