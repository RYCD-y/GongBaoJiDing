//
// Created by ye on 2026/10/29.
//
// 定时器时钟/分辨率换算实现: 见 bsp_tim.h
//

#include "bsp_tim.h"

uint32_t BSP_TIM_GetClockFreq(const TIM_TypeDef *tim)
{
        RCC_ClkInitTypeDef clk_config = {0};
        uint32_t           latency    = 0;
        uint32_t           pclk;

        HAL_RCC_GetClockConfig(&clk_config, &latency);

        // 挂载在 APB2 上的定时器
        if (tim == TIM1 || tim == TIM8 || tim == TIM9 || tim == TIM10 || tim == TIM11)
        {
                pclk = HAL_RCC_GetPCLK2Freq();
                return (clk_config.APB2CLKDivider == RCC_HCLK_DIV1) ? pclk : (pclk * 2u);
        }

        pclk = HAL_RCC_GetPCLK1Freq();
        return (clk_config.APB1CLKDivider == RCC_HCLK_DIV1) ? pclk : (pclk * 2u);
}

float BSP_TIM_GetTicksPerUs(const TIM_HandleTypeDef *htim)
{
        return (float) BSP_TIM_GetClockFreq(htim->Instance)
               / ((float) (htim->Init.Prescaler + 1u) * 1000000.0f);
}
