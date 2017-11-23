//LED
#include "led.h"
#include "delay.h"
#include "usart.h"
void LedInit()
{
	RCC->APB2ENR |= 1<<3;
	RCC->APB2ENR |= 1<<6;
	GPIOB->CRL &= 0xFFFFFFFF;
	GPIOB->CRL |= 0x00300000;
	GPIOE->CRL &= 0xFFFFFFFF;
	GPIOE->CRL |= 0x00300000;
	GPIOB->ODR |= 0<<5;
	GPIOE->ODR |= 0<<5;
	
}
void LedStart()
{
	
	GPIOB->ODR |= 1<<5;
	GPIOE->ODR |= 1<<5;
	Delay_ms(3000);
	GPIOB->ODR |= 0<<5;
	GPIOE->ODR |= 0<<5;
	printf("aa\n");
	Delay_ms(3000);
}

