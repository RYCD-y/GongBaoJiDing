//
// Created by ye on 2026/10/29.
//
// 按键驱动(BSP 层)实现: 见 bsp_key.h
//

#include "bsp_key.h"

#include "FreeRTOS.h"
#include "task.h"

typedef struct
{
        GPIO_TypeDef *port;
        uint16_t      pin;
        GPIO_PinState active_level;

        uint8_t  short_press_num;                   // 短按次数
        uint8_t  long_press_num;                    // 长按次数
        uint8_t  press_order[KEY_PRESS_ORDER_MAX];  // 按下顺序(1 短按 / 2 长按)
        uint8_t  press_order_select;                // 顺序记录写指针

        uint16_t press_time;  // 按下持续时间(扫描次数)
        uint16_t leave_time;  // 松手持续时间(扫描次数)
        uint8_t  last;        // 上次状态: 1 按下, 0 松开
        uint8_t  inited;

        Key_EventHandler handler;
} Key_Handle_t;

static Key_Handle_t s_key = {0};

// 清零手势记录(空闲触发后调用, 不影响空闲计时)
static void Key_ResetRecord(void)
{
        s_key.short_press_num    = 0;
        s_key.long_press_num     = 0;
        s_key.press_order_select = 0;
        s_key.press_time         = 0;

        for (uint8_t i = 0; i < KEY_PRESS_ORDER_MAX; i++)
        {
                s_key.press_order[i] = 0;
        }
}

void Key_Init(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState active_level)
{
        s_key.port         = port;
        s_key.pin          = pin;
        s_key.active_level = active_level;

        s_key.last       = 0;  // 上电默认松手
        s_key.leave_time = 0;
        s_key.handler    = NULL;
        s_key.inited     = 1;

        Key_ResetRecord();
}

void Key_SetEventHandler(Key_EventHandler handler)
{
        s_key.handler = handler;
}

void Key_Clear(void)
{
        Key_ResetRecord();
        s_key.leave_time = 0;
}

void Key_Scan(void)
{
        uint8_t pressed;

        if (!s_key.inited)
        {
                return;
        }

        pressed = (HAL_GPIO_ReadPin(s_key.port, s_key.pin) == s_key.active_level) ? 1u : 0u;

        if (pressed && !s_key.last)
        {
                /*===| 按下瞬间 |===*/
                s_key.press_time = 0;
                s_key.leave_time = 0;
        }
        else if (pressed && s_key.last)
        {
                /*===| 持续按下 |===*/
                if (s_key.press_time < UINT16_MAX)
                {
                        s_key.press_time++;
                }
        }
        else if (!pressed && s_key.last)
        {
                /*===| 松手瞬间: 判定短按/长按 |===*/
                uint8_t press_type;

                if (s_key.press_time < KEY_LONG_PRESS_CNT)
                {
                        press_type = KEY_PRESS_SHORT;
                        if (s_key.short_press_num < UINT8_MAX)
                        {
                                s_key.short_press_num++;
                        }
                }
                else
                {
                        press_type = KEY_PRESS_LONG;
                        if (s_key.long_press_num < UINT8_MAX)
                        {
                                s_key.long_press_num++;
                        }
                }

                if (s_key.press_order_select < KEY_PRESS_ORDER_MAX)
                {
                        s_key.press_order[s_key.press_order_select++] = press_type;
                }

                s_key.press_time = 0;
                s_key.leave_time = 0;
        }

        if (!pressed && !s_key.last)
        {
                /*===| 持续松手: 空闲判定 |===*/
                if (s_key.leave_time < KEY_IDLE_CNT)
                {
                        s_key.leave_time++;
                }

                if (s_key.leave_time == KEY_IDLE_CNT)
                {
                        if (s_key.handler != NULL)
                        {
                                s_key.handler();
                        }
                        Key_ResetRecord();
                }
        }

        s_key.last = pressed;
}

uint8_t Key_GetShortPressNum(void)
{
        return s_key.short_press_num;
}

uint8_t Key_GetLongPressNum(void)
{
        return s_key.long_press_num;
}

uint8_t Key_GetPressOrder(uint8_t index)
{
        if (index >= s_key.press_order_select)
        {
                return 0;
        }

        return s_key.press_order[index];
}

void Key_Task(void *argument)
{
        TickType_t last_wake_time;

        (void) argument;

        last_wake_time = xTaskGetTickCount();

        for (;;)
        {
                Key_Scan();

                vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(KEY_SCAN_PERIOD_MS));
        }
}
