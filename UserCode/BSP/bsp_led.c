//
// Created by ye on 2026/10/29.
//
// RGB LED 驱动(BSP 层)实现: 见 bsp_led.h
//

#include "bsp_led.h"

#include "FreeRTOS.h"
#include "task.h"

typedef struct
{
        TIM_HandleTypeDef *htim;
        uint32_t           channel[3];  // R / G / B 对应通道
        uint16_t           max_light;   // 定时器 ARR, 即最大占空比计数值
        uint16_t           hue;         // 彩虹流水当前色相
        uint8_t            inited;
} LED_Handle_t;

static LED_Handle_t s_led = {0};

// 写单通道占空比(0 ~ max_light)
static void LED_Write(LED_Color color, uint16_t light)
{
        if (!s_led.inited || (uint8_t) color > (uint8_t) LED_B)
        {
                return;
        }

        if (light > s_led.max_light)
        {
                light = s_led.max_light;
        }

        __HAL_TIM_SET_COMPARE(s_led.htim, s_led.channel[color], light);
}

void LED_Init(TIM_HandleTypeDef *htim, uint32_t ch_r, uint32_t ch_g, uint32_t ch_b)
{
        s_led.htim                   = htim;
        s_led.channel[LED_R]         = ch_r;
        s_led.channel[LED_G]         = ch_g;
        s_led.channel[LED_B]         = ch_b;
        s_led.max_light              = (uint16_t) __HAL_TIM_GET_AUTORELOAD(htim);
        s_led.hue                    = 0;
        s_led.inited                 = 1;

        HAL_TIM_PWM_Start(htim, ch_r);
        HAL_TIM_PWM_Start(htim, ch_g);
        HAL_TIM_PWM_Start(htim, ch_b);

        LED_OFF(LED_R);
        LED_OFF(LED_G);
        LED_OFF(LED_B);
}

void LED_SetRGB(uint8_t r, uint8_t g, uint8_t b)
{
        LED_Write(LED_R, (uint16_t) ((uint32_t) r * s_led.max_light / 255u));
        LED_Write(LED_G, (uint16_t) ((uint32_t) g * s_led.max_light / 255u));
        LED_Write(LED_B, (uint16_t) ((uint32_t) b * s_led.max_light / 255u));
}

void LED_SetHSV(uint16_t h, uint8_t s, uint8_t v)
{
        float rgb_max, rgb_min, rgb_adj;
        float r, g, b;
        uint8_t i;
        uint16_t difs;

        if (h > 360)
        {
                h = 360;
        }
        if (s > 100)
        {
                s = 100;
        }
        if (v > 100)
        {
                v = 100;
        }

        i    = (uint8_t) (h / 60);  // 0 ~ 6
        difs = (uint16_t) (h % 60);

        rgb_max = (float) v * 2.55f;
        rgb_min = rgb_max * (100.0f - (float) s) / 100.0f;
        rgb_adj = (rgb_max - rgb_min) * (float) difs / 60.0f;

        switch (i)
        {
        case 0:
                r = rgb_max;
                g = rgb_min + rgb_adj;
                b = rgb_min;
                break;
        case 1:
                r = rgb_max - rgb_adj;
                g = rgb_max;
                b = rgb_min;
                break;
        case 2:
                r = rgb_min;
                g = rgb_max;
                b = rgb_min + rgb_adj;
                break;
        case 3:
                r = rgb_min;
                g = rgb_max - rgb_adj;
                b = rgb_max;
                break;
        case 4:
                r = rgb_min + rgb_adj;
                g = rgb_min;
                b = rgb_max;
                break;
        default:
                r = rgb_max;
                g = rgb_min;
                b = rgb_max - rgb_adj;
                break;
        }

        LED_SetRGB((uint8_t) r, (uint8_t) g, (uint8_t) b);
}

void LED_ON(LED_Color color, uint16_t light)
{
        if (light > 255)
        {
                light = 255;
        }

        LED_Write(color, (uint16_t) ((uint32_t) light * s_led.max_light / 255u));
}

void LED_OFF(LED_Color color)
{
        LED_Write(color, 0);
}

void LED_Rainbow(uint16_t step)
{
        LED_SetHSV(s_led.hue, 100, 100);

        s_led.hue = (uint16_t) ((s_led.hue + step) % 360);
}

void LED_Task(void *argument)
{
        TickType_t last_wake_time;

        (void) argument;

        last_wake_time = xTaskGetTickCount();

        for (;;)
        {
                LED_Rainbow(4);

                vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(LED_TASK_PERIOD_MS));
        }
}
