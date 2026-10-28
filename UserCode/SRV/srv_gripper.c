//
// Created by ye on 2026/10/28.
//

#include "srv_gripper.h"

#include "bsp_servo.h"
#include "tim.h"
Servo_t servo;

//待整定
#define GRIP_ANGLE    00.0f         //张开角度
#define RELEASE_ANGLE 50.0f         //夹住角度
#define INITIAL_ANGLE RELEASE_ANGLE //初始角度

void Gripper_Init()
{
        Servo_Init(&servo, &htim1, TIM_CHANNEL_1);
        Servo_SetAngle(&servo, INITIAL_ANGLE);
}

void Gripper_Grip()
{
        Servo_SetAngle(&servo, GRIP_ANGLE);
}

void Gripper_Release()
{
        Servo_SetAngle(&servo, RELEASE_ANGLE);
}
