//中断
#include "NVIC.h"
static volatile u8 NVIC_group;

void NVICGroupInit(u8 group)
{
	//SCB->AIRCR
	//System Control Block 系统控制块
	u32 temp;
	NVIC_group = group;
	//通过STM32组号获取CM3内核中的分组号
	group = ~group & 0x07;

	temp = SCB->AIRCR;
	temp &= 0x0000F8FF;
	temp |= group << 8;
	temp |= 0x5FA0000;
	SCB->AIRCR = temp;
}
//NVIC 优先级设置
void NVICPriorityConfig (u8 preempttionPriority,u8 subPriority,u8 channel)
{
	//NVIC->IP寄存器
	//根据中断分组指定4位有效的抢占优先级位数和。。。
	u8 temp;
	//指定有效位的临时变量
	u8 temppter ,tempsub;
	temppter = 0x04 - NVIC_group;
	tempsub = 0x0F >> NVIC_group;

	temp = preempttionPriority << temppter;
	temp |= subPriority & tempsub;
	temp <<= 4;
	NVIC->IP[channel] = temp;
	NVIC -> ISER[channel>>0x05] |= (channel & 0x1F);
}
//GPIO 外部中断引脚配置
void ExNVICInit(u8 GPIO_x,u8 Bitx,u8 TRIM)
{
	//计算AFIO->EXITCR寄存器组的位置
	u8 exAddr,exOffset;
	exAddr = Bitx/4;
	exOffset = (Bitx%4)*4;
	//开启AFIO时钟
	RCC->APB2ENR |= 0x01;
	//将传入的IO引脚映射到对应的EXIT线上
	AFIO->EXTICR[exAddr] |= GPIO_x << exOffset;
	//开放IO引脚对应的外部中断线上的中断屏蔽
	EXTI->IMR |= 1<<Bitx;
	EXTI->EMR |= 1<<Bitx;

	if(TRIM & 0x01)
		EXTI->RTSR |= 1 << Bitx;
	if(TRIM & 0x02)
		EXTI->FTSR |= 1 << Bitx;
}



