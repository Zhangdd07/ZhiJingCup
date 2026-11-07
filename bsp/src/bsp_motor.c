/**
  ******************************************************************************
  * @file    bsp_motor.c
  * @brief   电机驱动层实现：L298N + TIM3/TIM4 硬件 PWM，单轮调速与换向
  ******************************************************************************
  */

#include "../inc/bsp_motor.h"
#include "../../Core/Inc/tim.h"

/* 每个轮子的硬件资源：一个定时器的两路 PWM 分别接到 L298N 的 IN1 / IN2 */
typedef struct
{
    TIM_HandleTypeDef *htim;    /**< 所属定时器 */
    uint32_t ch_in1;            /**< L298N IN1 对应的 PWM 通道 */
    uint32_t ch_in2;            /**< L298N IN2 对应的 PWM 通道 */
} Motor_Hw_t;

/* 下标顺序必须与 Motor_ID 一致 */
static const Motor_Hw_t s_motor_hw[MOTOR_NUM] =
{
    { &htim3, TIM_CHANNEL_1, TIM_CHANNEL_2 },   /* MOTOR_LF：PA6 / PA7 */
    { &htim3, TIM_CHANNEL_3, TIM_CHANNEL_4 },   /* MOTOR_RF：PB0 / PB1 */
    { &htim4, TIM_CHANNEL_1, TIM_CHANNEL_2 },   /* MOTOR_LB：PB6 / PB7 */
    { &htim4, TIM_CHANNEL_3, TIM_CHANNEL_4 }    /* MOTOR_RB：PB8 / PB9 */
};

/* 转向修正：1 表示该轮逻辑方向取反（对应 bsp_motor.h 里的配置宏） */
static const uint8_t s_motor_invert[MOTOR_NUM] =
{
    MOTOR_LF_INVERT,
    MOTOR_RF_INVERT,
    MOTOR_LB_INVERT,
    MOTOR_RB_INVERT
};

/**
  * @brief  取定时器一个 PWM 周期的计数值（ARR + 1），100% 占空比对应的比较值。
  */
static uint32_t Motor_GetPeriod(const Motor_Hw_t *hw)
{
    return (uint32_t)__HAL_TIM_GET_AUTORELOAD(hw->htim) + 1U;
}

/**
  * @brief  把 -100 ~ +100 的速度换算成比较寄存器值（0 ~ ARR+1）。
  */
static uint32_t Motor_SpeedToCompare(const Motor_Hw_t *hw, int16_t speed)
{
    uint32_t period = Motor_GetPeriod(hw);
    uint32_t magnitude = (speed >= 0) ? (uint32_t)speed : (uint32_t)(-speed);
    uint32_t compare = (magnitude * period) / (uint32_t)MOTOR_SPEED_MAX;

    return (compare > period) ? period : compare;
}

void Motor_Init(void)
{
    uint32_t i;

    /* 启动 8 路 PWM 输出（同一通道重复调用不会出错，这里每个通道各调用一次） */
    for (i = 0; i < (uint32_t)MOTOR_NUM; i++)
    {
        (void)HAL_TIM_PWM_Start(s_motor_hw[i].htim, s_motor_hw[i].ch_in1);
        (void)HAL_TIM_PWM_Start(s_motor_hw[i].htim, s_motor_hw[i].ch_in2);
    }

    /* 上电默认静止，避免初始化瞬间飞车 */
    Motor_StopAll();
}

void Motor_SetSpeed(Motor_ID id, int16_t speed)
{
    if ((uint32_t)id >= (uint32_t)MOTOR_NUM)
    {
        return;
    }

    const Motor_Hw_t *hw = &s_motor_hw[id];

    /* 限幅 */
    if (speed > MOTOR_SPEED_MAX)
    {
        speed = MOTOR_SPEED_MAX;
    }
    else if (speed < MOTOR_SPEED_MIN)
    {
        speed = MOTOR_SPEED_MIN;
    }

    /* 按轮子安装方向修正 */
    if (s_motor_invert[id] != 0U)
    {
        speed = (int16_t)(-speed);
    }

    uint32_t compare = Motor_SpeedToCompare(hw, speed);
    uint32_t compare_a;
    uint32_t compare_b;

    if (speed > 0)
    {
        /* 正转：IN1 出 PWM，IN2 拉低 */
        compare_a = compare;
        compare_b = 0U;
    }
    else if (speed < 0)
    {
        /* 反转：IN1 拉低，IN2 出 PWM */
        compare_a = 0U;
        compare_b = compare;
    }
    else
    {
        /* 停止：两路都为低，电机自由滑行 */
        compare_a = 0U;
        compare_b = 0U;
    }

    __HAL_TIM_SET_COMPARE(hw->htim, hw->ch_in1, compare_a);
    __HAL_TIM_SET_COMPARE(hw->htim, hw->ch_in2, compare_b);
}

void Motor_Stop(Motor_ID id)
{
    if ((uint32_t)id >= (uint32_t)MOTOR_NUM)
    {
        return;
    }

    __HAL_TIM_SET_COMPARE(s_motor_hw[id].htim, s_motor_hw[id].ch_in1, 0U);
    __HAL_TIM_SET_COMPARE(s_motor_hw[id].htim, s_motor_hw[id].ch_in2, 0U);
}

void Motor_Brake(Motor_ID id)
{
    if ((uint32_t)id >= (uint32_t)MOTOR_NUM)
    {
        return;
    }

    /* IN1 = IN2 = 1：电机两端短接，形成能耗制动 */
    uint32_t period = Motor_GetPeriod(&s_motor_hw[id]);

    __HAL_TIM_SET_COMPARE(s_motor_hw[id].htim, s_motor_hw[id].ch_in1, period);
    __HAL_TIM_SET_COMPARE(s_motor_hw[id].htim, s_motor_hw[id].ch_in2, period);
}

void Motor_StopAll(void)
{
    uint32_t i;

    for (i = 0; i < (uint32_t)MOTOR_NUM; i++)
    {
        Motor_Stop((Motor_ID)i);
    }
}
