//
// Created by 15594 on 2026/11/9.
//

#include "../inc/Bluetooth.h"
#include "../../../app/inc/app_motor.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    HAL_UART_Transmit_IT(&huart2, &receive_Data, 1);
    if (receive_Data == 'F') {
        Car_Forward();
    }else if (receive_Data == 'B') {
        Car_Backward();
    }else if (receive_Data == 'R') {
        Car_TurnRight();
    }else if (receive_Data == 'L') {
        Car_TurnLeft();
    }else if (receive_Data == 'S') {
        Car_Stop();
    }
}
int _write(int file, char *ptr, int len)
{
    (void)file;
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 100);
    return len;
}