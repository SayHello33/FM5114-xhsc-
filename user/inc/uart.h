#ifndef _UART_H
#define _UART_H
#include "hc32l021.h"                   // Device header


void GPIO_ResetBits(GPIO_TypeDef *GPIOx,uint16_t pin);
void GPIO_SetBits(GPIO_TypeDef * GPIOx,uint16_t pin);
uint8_t GPIO_Read(GPIO_TypeDef *GPIOx,uint16_t pin);
void uart_init(void);


#endif