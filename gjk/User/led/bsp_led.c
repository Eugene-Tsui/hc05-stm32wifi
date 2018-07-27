//bsp : board support package
#include "stm32f10x.h"
#include "bsp_led.h"

void LED__GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LED_G_GPIO_CLK | LED_B_GPIO_CLK,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = LED_G_GPIO_PIN;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Speed =  GPIO_Speed_50MHz;
	
	GPIO_Init(LED_G_GPIO_PORT,&GPIO_InitStruct);
	
	LED_G_OFF;
}
