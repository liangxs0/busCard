#include "rc522.h"
#include "stm32f10x.h"
#include "SPI.h"


/****************************************************************************************************
*函数功能:RC522初始化
*参数:无
*返回值:无
*****************************************************************************************************/
void RC522_Init ( void )
{
	printf(" MISO OK\n");
	SPI_Config();
	
	RC522_Reset_Disable();
	
	RC522_CS_Disable();
	
		PcdReset();
	M500PcdConfigISOType( 'A' );//设置工作方式

}

/****************************************************************************************************
*函数功能:RC522管脚初始化
*参数:无
*返回值:无
*****************************************************************************************************/
void SPI_Config( void )
{
 
  GPIO_InitTypeDef GPIO_InitStructure;
  
	//配置 CS 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" CS OK\n");
  //配置SCK 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" SCK OK\n");
  //配置MOSI 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" MOSI OK\n");
  //配置MISO 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	printf(" MISO OK\n");
	
  /*配置  RST */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" RST OK\n");
}


