#ifndef _RC522_H_
#define _RC522_H_

#include <stm32f10x.h>
#include "SPI.h"
#include "delay.h"
#include <string.h>
#include "usart.h"

void RC522_test(void);//RC522����
void PcdReset(void);  //��λ
void M500PcdConfigISOType(u8 type); //���ù�����ʽ
char PcdRequest(u8 req_code, u8 * pTagType); //Ѱ��
char PcdAnticoll(u8 * pSnr);   //������

#endif


