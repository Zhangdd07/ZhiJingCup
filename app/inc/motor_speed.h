//
// Created by 15594 on 2026/11/11.
//

#ifndef COMPETATION_MOTOR_SPEED_H
#define COMPETATION_MOTOR_SPEED_H

#include "../../bsp/inc/bsp_motor.h"

#define GEAR_RATIO 35
#define FEEDBACK 11
#define DELTA_TIME 100 //每次测速的Δt（ms）
#define PI 3.1415926
#define R 0.03

extern float total_distance ,distance ,velocity;

void MotorSpeed_Calculation(void);
#endif //COMPETATION_MOTOR_SPEED_H
