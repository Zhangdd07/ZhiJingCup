/**
  ******************************************************************************
  * @file    bsp_motor.h
  * @brief   电机驱动层（STM32F103C8T6 + L298N + TIM3/TIM4 硬件 PWM）
  *          ---------------------------------------------------------------
  *          引脚分配（CubeMX 已配置为 PWM 复用推挽输出）：
  *            LF 左前轮：PA6 = TIM3_CH1 (LF1)   PA7 = TIM3_CH2 (LF2)
  *            RF 右前轮：PB0 = TIM3_CH3 (RF1)   PB1 = TIM3_CH4 (RF2)
  *            LB 左后轮：PB6 = TIM4_CH1 (LB1)   PB7 = TIM4_CH2 (LB2)
  *            RB 右后轮：PB8 = TIM4_CH3 (RB1)   PB9 = TIM4_CH4 (RB2)
  *          TIM3 / TIM4 均为：PSC = 71，ARR = 99
  *          => PWM 频率 = 72MHz / 72 / 100 = 10kHz，占空比分辨率 1%
  *
  *          驱动方式（每个轮子只用 2 根线）：
  *            speed > 0 ：IN1 输出 PWM，IN2 输出 0    -> 正转
  *            speed < 0 ：IN1 输出 0，  IN2 输出 PWM  -> 反转
  *            speed = 0 ：IN1 / IN2 都为 0            -> 自由停止（滑行）
  *          所以 L298N 的 ENA / ENB 跳线帽必须插上（使能端固定高电平），
  *          速度由 IN1 / IN2 上的 PWM 占空比决定。
  ******************************************************************************
  */

#ifndef COMPETATION_BSP_MOTOR_H
#define COMPETATION_BSP_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ============================ 用户配置区 ============================ */

/* 上电调试：若某个轮子的实际转向与整车“前进”相反，
 * 把下面 4 个宏中对应的那个改成 1 即可（纯软件修正，不用重新接线）。 */
#define MOTOR_LF_INVERT   1     /* 左前轮 */
#define MOTOR_RF_INVERT   0     /* 右前轮 */
#define MOTOR_LB_INVERT   0     /* 左后轮 */
#define MOTOR_RB_INVERT   0     /* 右后轮 */

/* 速度量纲：百分比占空比，正数正转、负数反转，范围 -100 ~ +100 */
#define MOTOR_SPEED_MAX   100
#define MOTOR_SPEED_MIN   (-MOTOR_SPEED_MAX)

extern int32_t Pulse;
/* =================================================================== */

/** 电机编号（与硬件一一对应，供上层按轮子单独调速） */
typedef enum
{
    MOTOR_LF = 0,   /**< 左前轮 PA6 / PA7 */
    MOTOR_RF,       /**< 右前轮 PB0 / PB1 */
    MOTOR_LB,       /**< 左后轮 PB6 / PB7 */
    MOTOR_RB,       /**< 右后轮 PB8 / PB9 */
    MOTOR_NUM       /**< 电机数量，仅作边界用 */
} Motor_ID;

/**
  * @brief  电机初始化：启动 TIM3 / TIM4 共 8 路 PWM 输出，并让所有轮子停转。
  * @note   需在 MX_TIM3_Init() / MX_TIM4_Init() 之后调用。
  */
void Motor_Init(void);

/**
  * @brief  设置单个轮子的转速与方向。
  * @param  id    电机编号（MOTOR_LF / MOTOR_RF / MOTOR_LB / MOTOR_RB）
  * @param  speed 速度，-100 ~ +100；正数正转、负数反转、0 停止
  */
void Motor_SetSpeed(Motor_ID id, int16_t speed);

/**
  * @brief  单个轮子自由停止（IN1 = IN2 = 0，电机断电滑行）。
  */
void Motor_Stop(Motor_ID id);

/**
  * @brief  单个轮子短接制动（IN1 = IN2 = 1，电机两端短接，刹车更快）。
  * @note   长时间保持会持续耗电，一般只在需要急停时短暂使用。
  */
void Motor_Brake(Motor_ID id);

/**
  * @brief  所有轮子自由停止。
  */
void Motor_StopAll(void);

#ifdef __cplusplus
}
#endif

#endif /* COMPETATION_BSP_MOTOR_H */
