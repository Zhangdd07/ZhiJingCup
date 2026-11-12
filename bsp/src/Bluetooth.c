//
// Created by 15594 on 2026/11/9.
//
#include "usart.h"
#include "../inc/Bluetooth.h"

#include "../../../app/inc/app_motor.h"
#include <stdint.h>

uint8_t receive_Data[50];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    HAL_UART_Transmit_IT(&huart2, receive_Data, 1);
    if (receive_Data[0] == 'F') {
        Car_Forward();
    }else if (receive_Data[0] == 'B') {
        Car_Backward();
    }else if (receive_Data[0] == 'R') {
        Car_TurnRight();
    }else if (receive_Data[0] == 'L') {
        Car_TurnLeft();
    }else if (receive_Data[0] == 'S') {
        Car_Stop();
    }
    HAL_UART_Receive_IT(&huart2, receive_Data, 1);
}
int _write(int file, char *ptr, int len)
{
    (void)file;
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 100);
    return len;
}