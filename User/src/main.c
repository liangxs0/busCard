#include <stm32f10x.h>
#include "NVIC.h"
#include "usart.h"
#include "SPI.h"
#include "rc522.h"
#include "delay.h"
#include "led.h"
//#include "key.h"
int main()
{
	Delay_init(72);
	//≥ı ºªØ÷–∂œ
	NVICGroupInit(2);
	Usart1Init(57600);
	RC522_Init();
	LedInit();
	//keyInit();
	while(1){
	//printf("B");
	//Delay_ms(100);
	LedStart();
//	RC522_test();
	//usart1Putchar((u8)'c');
	//usart1PutStr((u8*)"HELLO\n");	
	}
		
	//return 0;
}

