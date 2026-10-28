//
// Created by ye on 2026/10/28.
//

#include "app.h"

#include "bsp_servo.h"
#include "iwdg.h"
#include "SRV/srv_gripper.h"

void App_Task(void *p)
{
        int      i, j     = 0;
        float    angle    = 10.0f;
        uint16_t iwdg_cnt = 0;

        Gripper_Init();
        TickType_t xLastWakeTime;
        // Initialise the xLastWakeTime variable with the current time.
        xLastWakeTime = xTaskGetTickCount();

        for (;;)
        {
                i++;
                if (i % 1 == 0)
                {}
                if (i % 10 == 0)
                {
                        HAL_IWDG_Refresh(&hiwdg);
                }
                if (i % 1000 == 0)
                {
                        if (j)
                        {
                                Gripper_Release();
                        }
                        else
                        {
                                Gripper_Grip();
                        }

                        j = !j;
                }


                // 嗂狗: 本任务 1ms 一次, 100ms 嗂一次(IWDG 超时约 0.5s)
                if (++iwdg_cnt >= 100)
                {
                        iwdg_cnt = 0;
                }

                vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
        }
}
