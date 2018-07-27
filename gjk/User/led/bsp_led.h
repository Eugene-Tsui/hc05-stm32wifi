#ifndef  __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"

#define LED_B_NO GPIO_ResetBits(LED_B_GPIO_PORT,LED_B_GPIO_PIN)
#define LED_B_OFF GPIO_SetBits(LED_B_GPIO_PORT,LED_B_GPIO_PIN)

#define LED_G_NO GPIO_ResetBits(LED_G_GPIO_PORT,LED_G_GPIO_PIN)
#define LED_G_OFF GPIO_SetBits(LED_G_GPIO_PORT,LED_G_GPIO_PIN)

#define LED_G_GPIO_PIN   GPIO_Pin_0
#define LED_G_GPIO_PORT  GPIOB
#define LED_G_GPIO_CLK RCC_APB2Periph_GPIOB

#define LED_B_GPIO_PIN   GPIO_Pin_1
#define LED_B_GPIO_PORT  GPIOB
#define LED_B_GPIO_CLK RCC_APB2Periph_GPIOB

void LED__GPIO_Config(void);
#endif