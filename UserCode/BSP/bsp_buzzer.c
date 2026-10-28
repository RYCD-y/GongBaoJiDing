//
// Created by ye on 2026/10/29.
//
// 蜂鸣器驱动(BSP 层)实现: 见 bsp_buzzer.h
//

#include "bsp_buzzer.h"

#include "bsp_tim.h"

#include "FreeRTOS.h"
#include "task.h"

// 音调频率表(Hz), 下标与 Buzzer_Tone_t 对应
static const float s_tone_freq[] = {
        [P] = 0.0f,
        [L1] = 262.0f,  [L2] = 294.0f,  [L3] = 330.0f,  [L4] = 349.0f,
        [L5] = 392.0f,  [L6] = 440.0f,  [L7] = 494.0f,
        [M1] = 523.0f,  [M2] = 587.0f,  [M3] = 659.0f,  [M4] = 698.0f,
        [M5] = 784.0f,  [M6] = 880.0f,  [M7] = 988.0f,
        [H1] = 1046.0f, [H2] = 1175.0f, [H3] = 1318.0f, [H4] = 1397.0f,
        [H5] = 1568.0f, [H6] = 1760.0f, [H7] = 1976.0f,
};

typedef struct
{
        TIM_HandleTypeDef *htim;
        uint32_t           channel;
        volatile Buzzer_SoundEffect_t sound_effect;
        uint8_t            inited;
} Buzzer_Handle_t;

static Buzzer_Handle_t s_buzzer = {0};

void Buzzer_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{
        s_buzzer.htim         = htim;
        s_buzzer.channel      = channel;
        s_buzzer.sound_effect = Buzzer_SoundEffect_OFF;
        s_buzzer.inited       = 1;

        Buzzer_Start();
}

void Buzzer_Start(void)
{
        if (!s_buzzer.inited)
        {
                return;
        }

        HAL_TIM_PWM_Start(s_buzzer.htim, s_buzzer.channel);
}

void Buzzer_Stop(void)
{
        if (!s_buzzer.inited)
        {
                return;
        }

        HAL_TIM_PWM_Stop(s_buzzer.htim, s_buzzer.channel);
}

void Buzzer_SetFrequency(float freq_hz)
{
        uint32_t tim_clk;
        uint32_t arr;

        if (!s_buzzer.inited)
        {
                return;
        }

        // 低于人耳下限的频率视为休止: 占空比清零
        if (freq_hz < 20.0f)
        {
                __HAL_TIM_SET_COMPARE(s_buzzer.htim, s_buzzer.channel, 0);
                return;
        }

        tim_clk = BSP_TIM_GetClockFreq(s_buzzer.htim->Instance);
        arr     = (uint32_t) ((float) tim_clk / ((float) BUZZER_PSC * freq_hz) + 0.5f);

        if (arr < 2u)
        {
                arr = 2u;
        }
        if (arr > 65536u)
        {
                arr = 65536u;
        }

        // 频率 = tim_clk / (PSC * ARR), 占空比固定 50%
        __HAL_TIM_SET_PRESCALER(s_buzzer.htim, BUZZER_PSC - 1u);
        __HAL_TIM_SET_AUTORELOAD(s_buzzer.htim, arr - 1u);
        __HAL_TIM_SET_COMPARE(s_buzzer.htim, s_buzzer.channel, arr / 2u);
}

void Buzzer_SetTone(Buzzer_Tone_t tone)
{
        if ((uint8_t) tone > (uint8_t) H7)
        {
                return;
        }

        Buzzer_SetFrequency(s_tone_freq[tone]);
}

void Buzzer_SetSoundEffect(Buzzer_SoundEffect_t effect)
{
        s_buzzer.sound_effect = effect;
}

void Buzzer_Task(void *argument)
{
        (void) argument;

        for (;;)
        {
                switch ((uint8_t) s_buzzer.sound_effect)
                {
                case Buzzer_SoundEffect_OFF:
                {
                        Buzzer_SetTone(P);
                        s_buzzer.sound_effect = Buzzer_SoundEffect_OFF;
                        Buzzer_Stop();
                        break;
                }
                case Buzzer_SoundEffect_SystemStart:
                {
                        Buzzer_Start();
                        Buzzer_SetTone(M1); vTaskDelay(pdMS_TO_TICKS(200)); Buzzer_SetTone(P); vTaskDelay(pdMS_TO_TICKS(50));
                        Buzzer_SetTone(M2); vTaskDelay(pdMS_TO_TICKS(200)); Buzzer_SetTone(P); vTaskDelay(pdMS_TO_TICKS(50));
                        Buzzer_SetTone(M3); vTaskDelay(pdMS_TO_TICKS(200)); Buzzer_SetTone(P); vTaskDelay(pdMS_TO_TICKS(50));
                        Buzzer_SetTone(M2); vTaskDelay(pdMS_TO_TICKS(200)); Buzzer_SetTone(P); vTaskDelay(pdMS_TO_TICKS(50));
                        Buzzer_SetTone(M5); vTaskDelay(pdMS_TO_TICKS(200)); Buzzer_SetTone(P); vTaskDelay(pdMS_TO_TICKS(50));
                        Buzzer_SetTone(M7); vTaskDelay(pdMS_TO_TICKS(200)); Buzzer_SetTone(P); vTaskDelay(pdMS_TO_TICKS(50));
                        Buzzer_SetTone(M6); vTaskDelay(pdMS_TO_TICKS(500));
                        Buzzer_SetTone(M5); vTaskDelay(pdMS_TO_TICKS(250));
                        Buzzer_SetTone(M6); vTaskDelay(pdMS_TO_TICKS(750));
                        Buzzer_SetTone(P);
                        s_buzzer.sound_effect = Buzzer_SoundEffect_OFF;
                        break;
                }
                case Buzzer_SoundEffect_SuperCap_ON:
                {
                        Buzzer_Start();
                        Buzzer_SetTone(M1); vTaskDelay(pdMS_TO_TICKS(200));
                        Buzzer_SetTone(M3); vTaskDelay(pdMS_TO_TICKS(200));
                        Buzzer_SetTone(M5); vTaskDelay(pdMS_TO_TICKS(200));
                        Buzzer_SetTone(P);
                        s_buzzer.sound_effect = Buzzer_SoundEffect_OFF;
                        break;
                }
                case Buzzer_SoundEffect_SuperCap_OFF:
                {
                        Buzzer_Start();
                        Buzzer_SetTone(M5); vTaskDelay(pdMS_TO_TICKS(200));
                        Buzzer_SetTone(M3); vTaskDelay(pdMS_TO_TICKS(200));
                        Buzzer_SetTone(M1); vTaskDelay(pdMS_TO_TICKS(200));
                        Buzzer_SetTone(P);
                        s_buzzer.sound_effect = Buzzer_SoundEffect_OFF;
                        break;
                }
                default:
                {
                        s_buzzer.sound_effect = Buzzer_SoundEffect_OFF;
                        break;
                }
                }

                vTaskDelay(pdMS_TO_TICKS(BUZZER_TASK_PERIOD_MS));
        }
}
