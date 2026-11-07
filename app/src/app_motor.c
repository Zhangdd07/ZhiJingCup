/**
  ******************************************************************************
  * @file    app_motor.c
  * @brief   应用层实现：麦克纳姆轮小车的前进 / 后退 / 左转 / 右转 / 停车
  ******************************************************************************
  */

#include "../inc/app_motor.h"
#include "../../bsp/inc/bsp_motor.h"

/* 当前默认行驶速度（1 ~ 100）。左转/右转用同一个值，保证转弯和直行速度一致 */
static int16_t s_car_speed = CAR_SPEED_DEFAULT;

/**
  * @brief  速度限幅到 -100 ~ +100。
  */
static int16_t Car_LimitSpeed(int16_t speed)
{
    if (speed > MOTOR_SPEED_MAX)
    {
        return MOTOR_SPEED_MAX;
    }
    if (speed < MOTOR_SPEED_MIN)
    {
        return MOTOR_SPEED_MIN;
    }

    return speed;
}

void Car_Init(void)
{
    s_car_speed = CAR_SPEED_DEFAULT;
    Motor_Init();
}

void Car_SetSpeed(uint8_t percent)
{
    if (percent > (uint8_t)MOTOR_SPEED_MAX)
    {
        percent = (uint8_t)MOTOR_SPEED_MAX;
    }

    s_car_speed = (int16_t)percent;
}

uint8_t Car_GetSpeed(void)
{
    return (uint8_t)s_car_speed;
}

void Car_SetWheelSpeed(int16_t lf, int16_t rf, int16_t lb, int16_t rb)
{
    Motor_SetSpeed(MOTOR_LF, Car_LimitSpeed(lf));
    Motor_SetSpeed(MOTOR_RF, Car_LimitSpeed(rf));
    Motor_SetSpeed(MOTOR_LB, Car_LimitSpeed(lb));
    Motor_SetSpeed(MOTOR_RB, Car_LimitSpeed(rb));
}

void Car_Forward(void)
{
    int16_t speed = s_car_speed;

    /* 四轮同向正转 -> 整车沿车头方向直行 */
    Car_SetWheelSpeed(speed, speed, speed, speed);
}

void Car_Backward(void)
{
    int16_t speed = s_car_speed;

    /* 四轮同向反转 -> 整车倒退 */
    Car_SetWheelSpeed((int16_t)(-speed), (int16_t)(-speed),
                      (int16_t)(-speed), (int16_t)(-speed));
}

void Car_TurnLeft(void)
{
    int16_t speed = s_car_speed;

    /* 左侧后退 + 右侧前进 -> 绕车体中心逆时针自转（车头向左摆） */
    Car_SetWheelSpeed((int16_t)(-speed), speed,
                      (int16_t)(-speed), speed);
}

void Car_TurnRight(void)
{
    int16_t speed = s_car_speed;

    /* 左侧前进 + 右侧后退 -> 绕车体中心顺时针自转（车头向右摆） */
    Car_SetWheelSpeed(speed, (int16_t)(-speed),
                      speed, (int16_t)(-speed));
}

void Car_Stop(void)
{
    Motor_StopAll();
}
