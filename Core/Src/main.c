/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../app/inc/app_motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* 底盘调试模式：
 *   0 = 关闭，只跑你自己的正式逻辑；
 *   1 = 单轮点动标定：依次单独驱动 左前 -> 右前 -> 左后 -> 右后，各 1 秒。
 *       用来确认“哪个通道驱动哪个轮子”以及“正转是不是车头方向”；
 *   2 = 四动作自检：前进 -> 后退 -> 左转 -> 右转 -> 停车，各 1 秒。
 * 接线还没确认过时请用 1，标定完再改成 2 检查整车动作。 */
#define CAR_DEMO_MODE   0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* 底盘初始化：启动 TIM3 / TIM4 共 8 路 PWM，默认速度 CAR_SPEED_DEFAULT */
  Car_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Car_Forward();
    HAL_Delay(10000);
    Car_TurnRight();
    HAL_Delay(2000);
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#if (CAR_DEMO_MODE == 1)
    /* ---------- 单轮点动：同一时刻只有一个轮子转 ----------
     * 参数顺序是 (左前, 右前, 左后, 右后)，速度单位是百分比。
     * 每一段停下来时，记下：转的是哪个位置的轮子？转向是不是车头方向？ */
    Car_SetWheelSpeed( 40,  0,  0,  0);   /* 左前 LF：PA6 / PA7 */
    HAL_Delay(1000);
    Car_Stop();
    HAL_Delay(800);

    Car_SetWheelSpeed(  0, 40,  0,  0);   /* 右前 RF：PB0 / PB1 */
    HAL_Delay(1000);
    Car_Stop();
    HAL_Delay(800);

    Car_SetWheelSpeed(  0,  0, 40,  0);   /* 左后 LB：PB6 / PB7 */
    HAL_Delay(1000);
    Car_Stop();
    HAL_Delay(800);

    Car_SetWheelSpeed(  0,  0,  0, 40);   /* 右后 RB：PB8 / PB9 */
    HAL_Delay(1000);
    Car_Stop();
    HAL_Delay(1500);
#elif (CAR_DEMO_MODE == 2)
    /* ---------- 四动作自检：前进 -> 后退 -> 左转 -> 右转 -> 停车 ---------- */
    Car_Forward();
    HAL_Delay(1000);
    Car_Backward();
    HAL_Delay(1000);
    Car_TurnLeft();
    HAL_Delay(1000);
    Car_TurnRight();
    HAL_Delay(1000);
    Car_Stop();
    HAL_Delay(1000);
#else
    /* ---------- 正式逻辑写在这里，例如 ----------
     * Car_SetSpeed(60);
     * Car_Forward();
     * HAL_Delay(2000);
     * Car_TurnLeft();
     * HAL_Delay(500);
     * Car_Stop();
     */
#endif
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
