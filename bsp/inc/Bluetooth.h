//
// Created by 15594 on 2026/11/9.
//

#ifndef COMPETATION_BLUETOOTH_H
#define COMPETATION_BLUETOOTH_H

#include <stdint.h>

#include "stm32f1xx_hal_uart.h"
#include "usart.h"

char receive_Data;

int _write(int file, char *ptr, int len);


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif //COMPETATION_BLUETOOTH_H
