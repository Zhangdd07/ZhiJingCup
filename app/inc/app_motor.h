/**
  ******************************************************************************
  * @file    app_motor.h
  * @brief   应用层：麦克纳姆轮小车底盘的整车运动控制
  *          ---------------------------------------------------------------
  *          轮子编号（俯视，车头朝上）：
  *
  *                         车头
  *                LF(左前)        RF(右前)
  *                LB(左后)        RB(右后)
  *
  *          运动定义：
  *            前进     四个轮子同时正转
  *            后退     四个轮子同时反转
  *            左转     原地逆时针自转：左前/左后反转，右前/右后正转
  *            右转     原地顺时针自转：左前/左后正转，右前/右后反转
  *
  *          所有函数都是“设置目标状态”，调用后立即返回、不阻塞，
  *          需要持续多久由调用者用 HAL_Delay() 或自己写的状态机控制。
  ******************************************************************************
  */

#ifndef COMPETATION_APP_MOTOR_H
#define COMPETATION_APP_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ============================ 用户配置区 ============================ */

/* 默认行驶速度（1 ~ 100，占空比百分比）
 * 第一次上电调试建议先用 30 ~ 40，确认方向正确后再提高。 */
#define CAR_SPEED_DEFAULT   100

/* =================================================================== */

/**
  * @brief  小车底盘初始化（内部会调用 Motor_Init 启动 PWM）。
  * @note   需在 MX_GPIO_Init() / MX_TIM3_Init() / MX_TIM4_Init() 之后调用。
  */
void Car_Init(void);

/**
  * @brief  设置默认行驶速度，之后前进/后退/左转/右转都用这个速度。
  * @param  percent 1 ~ 100（超过 100 按 100 处理）
  */
void Car_SetSpeed(uint8_t percent);

/**
  * @brief  读取当前默认行驶速度（1 ~ 100）。
  */
uint8_t Car_GetSpeed(void);

/**
  * @brief  前进：四个轮子同时正转。
  */
void Car_Forward(void);

/**
  * @brief  后退：四个轮子同时反转。
  */
void Car_Backward(void);

/**
  * @brief  左转（原地逆时针自转）：左侧两轮反转，右侧两轮正转。
  */
void Car_TurnLeft(void);

/**
  * @brief  右转（原地顺时针自转）：左侧两轮正转，右侧两轮反转。
  */
void Car_TurnRight(void);

/**
  * @brief  停车：四个轮子全部断电自由滑行。
  */
void Car_Stop(void);

/**
  * @brief  按轮子分别给定速度，用于后续扩展平移、PID 差速等玩法。
  * @param  lf 左前轮速度 -100 ~ +100
  * @param  rf 右前轮速度 -100 ~ +100
  * @param  lb 左后轮速度 -100 ~ +100
  * @param  rb 右后轮速度 -100 ~ +100
  */
void Car_SetWheelSpeed(int16_t lf, int16_t rf, int16_t lb, int16_t rb);

#ifdef __cplusplus
}
#endif

#endif /* COMPETATION_APP_MOTOR_H */
