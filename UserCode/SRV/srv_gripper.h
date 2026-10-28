//
// Created by ye on 2026/10/28.
//

#ifndef GONGBAOJIDING_SRV_GRIPPER_H
#define GONGBAOJIDING_SRV_GRIPPER_H
#include "bsp_servo.h"
extern Servo_t servo;

void Gripper_Init();

void Gripper_Grip();

void Gripper_Release();

#endif //GONGBAOJIDING_SRV_GRIPPER_H
