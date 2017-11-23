#include "rc522.h"
#include "stm32f10x.h"
#include "SPI.h"


/****************************************************************************************************
*��������:RC522��ʼ��
*����:��
*����ֵ:��
*****************************************************************************************************/
void RC522_Init ( void )
{
	printf(" MISO OK\n");
	SPI_Config();
	
	RC522_Reset_Disable();
	
	RC522_CS_Disable();
	
		PcdReset();
	M500PcdConfigISOType( 'A' );//���ù�����ʽ

}

/****************************************************************************************************
*��������:RC522�ܽų�ʼ��
*����:��
*����ֵ:��
*****************************************************************************************************/
void SPI_Config( void )
{
 
  GPIO_InitTypeDef GPIO_InitStructure;
  
	//���� CS 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" CS OK\n");
  //����SCK 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" SCK OK\n");
  //����MOSI 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" MOSI OK\n");
  //����MISO 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	printf(" MISO OK\n");
	
  /*����  RST */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf(" RST OK\n");
}


