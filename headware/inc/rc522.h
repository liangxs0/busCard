#ifndef _RC522_H_
#define _RC522_H_

#include <stm32f10x.h>
#include "SPI.h"
#include "delay.h"
#include <string.h>
#include "usart.h"

void RC522_test(void);//RC522测试
void PcdReset(void);  //复位
void M500PcdConfigISOType(u8 type); //设置工作方式
char PcdRequest(u8 req_code, u8 * pTagType); //寻卡
char PcdAnticoll(u8 * pSnr);   //读卡号

#endif


