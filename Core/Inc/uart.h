/*
 * uart.h
 *
 *  Created on: Apr 3, 2023
 *      Author: nguye_tihap4s
 */

#include "main.h"

#ifndef INC_UART_H_
#define INC_UART_H_

#define DMX_TX_Pin GPIO_PIN_9
#define DMX_TX_GPIO_Port GPIOA
#define DMX_RX_Pin GPIO_PIN_10
#define DMX_RX_GPIO_Port GPIOA
#define DMX_UART_INIT_SEND_ADD huart1.Init.StopBits = UART_STOPBITS_1
#define DMX_UART_INIT_SEND_DATA huart1.Init.StopBits = UART_STOPBITS_2

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern void MX_USART1_UART_Init(void);
//extern void MX_USART1_UART_Init_Set(void);
void MX_USART2_UART_Init(void);

#endif /* INC_UART_H_ */
