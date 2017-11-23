#ifndef _NVIC_H_
#define _NVIC_NVIC
#include <stm32f10x.h>
typedef enum {
	GPIO_A,
	GPIO_B,
	GPIO_C,
	GPIO_D,
	GPIO_E,
	GPIO_F,
	GPIO_G,
	
}GPIO_x;
//NVIC初始化，
void NVICGroupInit(u8 group);
//NVIC优先级设置
void NVICPriorityConfig(u8 preempttionPriority,u8 subPriority,u8 channel);
//GPIO外部中断引脚配置
void ExNVICInit(u8 GPIOx,u8 Bitx,u8 TRIM);


#endif
