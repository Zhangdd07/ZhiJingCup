//
// Created by 15594 on 2026/11/11.
//

#include "../inc/motor_speed.h"

#include "stm32f1xx_hal.h"

uint32_t feedback_per_lap = GEAR_RATIO * FEEDBACK;
uint32_t time = 0;
float total_distance ,distance ,velocity;

void MotorSpeed_Calculation(void) {
    if (HAL_GetTick() - time > DELTA_TIME) {
        distance = Pulse * 2 * PI * R / feedback_per_lap;
        velocity = distance / DELTA_TIME * 1000;
        total_distance += distance ;
        time = HAL_GetTick();
        Pulse = 0;
    }

}