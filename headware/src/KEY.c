//°´¼ü
#include "key.h"

void keyInit(void)
{
	RCC->APB2ENR |= 1<<6;
	GPIOE->CRL &= 0xFFF00FFF;
	GPIOE->CRL |= 0x00088000;
	GPIOE->ODR |= 0<<3 | 0<<4;
	
	//ÖÐ¶ÏÓ³Éä
	ExNVICInit(GPIO_E,3,0x01);
	NVICPriorityConfig(3,1,EXTI3_IRQn);
	
	ExNVICInit(GPIO_E,4,0x01);
	NVICPriorityConfig(2,1,EXTI4_IRQn);
	
}

u8 getKeyValue(void)
{
	u8 key = 0;
	if(!GPIOE->IDR & 1<<4)
		key |= 1<<4;
	if(!GPIOE->IDR & 1<<3)
		key |= 1<<3;
	return key;
}

void EXTI3_IRQHandler()
{
	printf("key1\n");
}
void EXTI4_IRQHandler()
{
	printf("key0\n");
}
