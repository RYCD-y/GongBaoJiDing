//
// Created by ye on 2026/10/27.
//
// 舵机驱动(BSP 层)实现: 见 bsp_servo.h
//

#include "bsp_servo.h"
#include "utils.h"

/*===| 内部函数 |===*/

// 获取定时器计数时钟(APB1/APB2 定时器时钟, 非 1 分频时为 PCLK 的 2 倍)
static uint32_t Servo_GetTimClk(const TIM_TypeDef *tim)
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

// 校验 PWM 通道合法性
static uint8_t Servo_IsValidChannel(uint32_t channel)
{
        switch (channel)
        {
        case TIM_CHANNEL_1:
        case TIM_CHANNEL_2:
        case TIM_CHANNEL_3:
        case TIM_CHANNEL_4:
                return 1;
        default:
                return 0;
        }
}

// 把脉宽(us)写入比较寄存器
static void Servo_WritePulse(Servo_t *servo, float pulse_us)
{
        uint32_t arr   = __HAL_TIM_GET_AUTORELOAD(servo->htim);
        float    ticks = pulse_us * servo->tick_per_us;

        // 限幅到 [0, ARR] 后四舍五入写入比较寄存器
        ticks = float_limit(ticks, 0.0f, (float) arr);

        __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, (uint32_t) float_rounding(ticks));
}

// 脉宽 -> 角度(反算, 保证 SetPulse 后 angle 与实际输出一致)
static float Servo_PulseToAngle(const Servo_t *servo, float pulse_us)
{
        float ratio;

        ratio = (pulse_us - servo->cfg.min_pulse) / (servo->cfg.max_pulse - servo->cfg.min_pulse);
        if (servo->cfg.reversed)
        {
                ratio = 1.0f - ratio;
        }

        return servo->cfg.min_angle + ratio * (servo->cfg.max_angle - servo->cfg.min_angle);
}

/*===| 初始化 |===*/

uint8_t Servo_InitEx(Servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel, const Servo_Config_t *cfg)
{
        float tick_per_us;
        uint32_t arr;

        if (servo == NULL || htim == NULL || cfg == NULL)
        {
                return 1;
        }
        if (!Servo_IsValidChannel(channel))
        {
                return 1;
        }
        if (cfg->max_angle <= cfg->min_angle || cfg->max_pulse <= cfg->min_pulse || cfg->min_pulse < 0.0f)
        {
                return 1;
        }

        // 1us 对应的计数值 = 计数频率 / 1MHz
        tick_per_us = (float) Servo_GetTimClk(htim->Instance)
                      / ((float) (htim->Init.Prescaler + 1u) * 1000000.0f);

        arr = __HAL_TIM_GET_AUTORELOAD(htim);

        // PWM 周期短于最大脉宽时无法输出该脉宽(如 100kHz 的 PWM 配 500~2500us)
        if (cfg->max_pulse * tick_per_us > (float) arr)
        {
                return 1;
        }

        servo->htim        = htim;
        servo->channel     = channel;
        servo->cfg         = *cfg;
        servo->tick_per_us = tick_per_us;
        servo->angle       = cfg->min_angle;
        servo->pulse       = cfg->min_pulse;
        servo->enabled     = 0;
        servo->inited      = 1;

        return Servo_Enable(servo);
}

uint8_t Servo_Init(Servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel)
{
        const Servo_Config_t cfg = {
                .min_angle = SERVO_DEFAULT_MIN_ANGLE,
                .max_angle = SERVO_DEFAULT_MAX_ANGLE,
                .min_pulse = SERVO_DEFAULT_MIN_PULSE,
                .max_pulse = SERVO_DEFAULT_MAX_PULSE,
                .reversed  = 0,
        };

        return Servo_InitEx(servo, htim, channel, &cfg);
}

/*===| PWM 启停 |===*/

uint8_t Servo_Enable(Servo_t *servo)
{
        if (servo == NULL || !servo->inited)
        {
                return 1;
        }

        if (HAL_TIM_PWM_Start(servo->htim, servo->channel) != HAL_OK)
        {
                return 1;
        }

        servo->enabled = 1;

        // 启动后立即输出当前目标脉宽, 避免停在 0 占空比
        Servo_WritePulse(servo, servo->pulse);

        return 0;
}

void Servo_Disable(Servo_t *servo)
{
        if (servo == NULL || !servo->inited)
        {
                return;
        }

        HAL_TIM_PWM_Stop(servo->htim, servo->channel);

        servo->enabled = 0;
}

/*===| 控制 |===*/

uint8_t Servo_SetAngle(Servo_t *servo, float angle)
{
        float ratio;
        float pulse;

        if (servo == NULL || !servo->inited)
        {
                return 1;
        }

        // 角度限幅到标定范围
        angle = float_limit(angle, servo->cfg.min_angle, servo->cfg.max_angle);

        ratio = (angle - servo->cfg.min_angle) / (servo->cfg.max_angle - servo->cfg.min_angle);
        if (servo->cfg.reversed)
        {
                ratio = 1.0f - ratio;
        }

        pulse = servo->cfg.min_pulse + ratio * (servo->cfg.max_pulse - servo->cfg.min_pulse);

        servo->angle = angle;
        servo->pulse = pulse;

        Servo_WritePulse(servo, pulse);

        return 0;
}

uint8_t Servo_SetPulse(Servo_t *servo, float pulse_us)
{
        if (servo == NULL || !servo->inited)
        {
                return 1;
        }

        // 脉宽限幅到标定范围
        pulse_us = float_limit(pulse_us, servo->cfg.min_pulse, servo->cfg.max_pulse);

        servo->pulse = pulse_us;
        servo->angle = Servo_PulseToAngle(servo, pulse_us);

        Servo_WritePulse(servo, pulse_us);

        return 0;
}

float Servo_GetAngle(const Servo_t *servo)
{
        return (servo == NULL) ? 0.0f : servo->angle;
}

float Servo_GetPulse(const Servo_t *servo)
{
        return (servo == NULL) ? 0.0f : servo->pulse;
}
