#ifndef __USART2_H
#define	__USART2_H

#include "stm32f10x.h"
#include <stdio.h>

//²¨ÌØÂÊ
#define             BLT_USART_BAUD_RATE                       38400

//´®¿Ú2
#define             BLT_USARTx                                USART2
#define             BLT_USART_APBxClock_FUN                 RCC_APB1PeriphClockCmd
#define             BLT_USART_CLK                             RCC_APB1Periph_USART2
#define             BLT_USART_GPIO_APBxClock_FUN            RCC_APB2PeriphClockCmd
#define             BLT_USART_GPIO_CLK                       (RCC_APB2Periph_GPIOA )     
#define             BLT_USART_TX_PORT                         GPIOA   
#define             BLT_USART_TX_PIN                          GPIO_Pin_2
#define             BLT_USART_RX_PORT                         GPIOA 
#define             BLT_USART_RX_PIN                          GPIO_Pin_3
#define             BLT_USART_IRQ                             USART2_IRQn
#define             BLT_USART_IRQHandler                         USART2_IRQHandler

void BLT_USART_Config(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

void bsp_USART_Process(void);
char *get_rebuff(uint16_t *len);
void clean_rebuff(void);

#endif /* __USART2_H */
