#include "rc522.h"


 /*************************************
 *  使用SPI与RC522通信
 *
 *
 **************************************/
/********************************************************************************
*函数功能:SPI发送数据
*参数:待发送的数据
*返回值:无
*
**********************************************************************************/
void RC522_Send(u8 byte)
{
    u8 counter;

    for(counter=0;counter<8;counter++)
    {     
			if ( byte & 0x80 )
					RC522_MOSI_H();//
			else 
					RC522_MOSI_L();
			Delay_us(200);
			RC522_SCK_L();
			Delay_us(200);
			RC522_SCK_H();
			Delay_us(200);
			byte <<= 1; 
    } 
}
/*******************************************************************************************
*函数功能:SPI读函数
*参数:无
*返回值:读到的数据
*
********************************************************************************************/
u8 RC522_Read( void )
{
	u8 counter;
	u8 Data;
	for(counter=0;counter<8;counter++)
	{
			Data <<= 1;
			RC522_SCK_L();
		  Delay_us(200);
			if (RC522_MISO_int() == 1)
			Data |= 0x01;
      Delay_us(200);
			RC522_SCK_H();
       Delay_us(200);  	
	}
	return Data;
	
}
/******************************************************************************************
*函数功能:读RC522寄存器
*参数:address 寄存器的地址
*返回值:寄存器的当前值
*
******************************************************************************************/
u8 ReadRC(u8 address )
{
	u8 Addr, Return;
	
	Addr=((address<<1)&0x7E)|0x80;
	
	RC522_CS_Enable();
	
	RC522_Send(Addr);
	
	Return=RC522_Read();
	
	RC522_CS_Disable();
		
	return Return;
	
	
}


/*******************************************************************************************
*函数功能:写RC522寄存器
*参数:Addresss 寄存器的地址
     Value 写入寄存器的值
*返回值:无
 *******************************************************************************************/
void WriteRawRC(u8 Address,u8 Value)
{  
	u8 Addr;
	
	Addr =(Address<<1)&0x7E;
	
	RC522_CS_Enable();
	
	RC522_Send(Addr);
	
	RC522_Send(Value);
	
	RC522_CS_Disable();	

	
}
/*******************************************************************************************
*函数功能:对RC522寄存器置位
*参数:Reg 寄存器的纸质
          mask 置位的值
* 返回值:无
 *******************************************************************************************/
void SetBitMask(u8 Reg,u8 Mask )  
{
    u8 Temp;
	
    Temp=ReadRC(Reg);
	
    WriteRawRC(Reg,Temp|Mask);         // set bit mask
	
}

/*******************************************************************************************
*函数功能:对RC522寄存器清零
*参数:Reg RC522的地址
     Mask 清零的数据
*返回值:无
*******************************************************************************************/
void ClearBitMask(u8 Reg,u8 Mask )  
{
    u8 Temp;
	
    Temp=ReadRC(Reg);
	
    WriteRawRC(Reg,Temp&(~Mask));  // clear bit mask
	
}
/*******************************************************************************************
*函数功能:开天线
*参数:无
*返回值:无
********************************************************************************************/
void PcdAntennaOn(void)
{
    u8 temp;
	
    temp=ReadRC(TxControlReg);
	
    if (!(temp&0x03))
			SetBitMask(TxControlReg,0x03);		
}
/*******************************************************************************************
*函数功能:关闭天线
*参数:无
*返回值:
*
 *******************************************************************************************/
void PcdAntennaOff ( void )
{
  ClearBitMask(TxControlReg,0x03 );
	
}


/*******************************************************************************************
*函数功能:复位RC522
*参数:无
*返回值:无
*
*******************************************************************************************/
void PcdReset(void)
{
	RC522_Reset_Disable();
	
	Delay_us(1);
	
	RC522_Reset_Enable();
	
	Delay_us(1);
	
	RC522_Reset_Disable();
	
	Delay_us(1);
	
	WriteRawRC(CommandReg,0x0f);
	
	while(ReadRC(CommandReg)&0x10);
	
	Delay_us(1);
	
  WriteRawRC(ModeReg, 0x3D );            //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
	
  WriteRawRC(TReloadRegL,30);          //16位定时器低位    
	WriteRawRC(TReloadRegH,0);			     //16位定时器高位
	
  WriteRawRC(TModeReg,0x8D);				   //定义内部定时器的设置
	
  WriteRawRC(TPrescalerReg,0x3E);			 //设置定时器分频系数
	
	WriteRawRC(TxAutoReg,0x40);				   //调制发送信号为100%ASK	
	

}

/*******************************************************************************************
*函数功能:设置RC522的工作方式
*参数:CardType:工作方式
*返回值:无
*
*******************************************************************************************/
void M500PcdConfigISOType(u8 CardType)
{
	if (CardType== 'A')                     //ISO14443_A
  {
		ClearBitMask(Status2Reg,0x08);
		
		WriteRawRC(ModeReg,0x3D);//3F
		
		WriteRawRC(RxSelReg,0x86);//84
		
		WriteRawRC(RFCfgReg,0x7F);   //4F
		
		WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		
		WriteRawRC(TReloadRegH,0);
		
		WriteRawRC(TModeReg,0x8D);
		
		WriteRawRC(TPrescalerReg,0x3E);
		
		Delay_us(2);
		PcdAntennaOn();//开天线		
   }

}


/*******************************************************************************************
*函数功能:通过RC522和ISO14443卡通讯
*参数:    Command，RC522的命令
*         intData，RC522发送到卡片的数据
*         intLen，发送数据的字节长度
*         outData，接收到的卡片返回数据
*         outLen，返回数据的长度
*返回值:
********************************************************************************************/
char PcdComMF522(u8 Command,u8 *intData,u8 intLen,u8 * Data,u32 * outLen )		
{
    char Status =Fault;
    u8 IrqEn  = 0x00;
    u8 WaitFor = 0x00;
    u8 LastBits;
    u8 N;
    u32 L;

    switch (Command)
    {
       case PCD_AUTHENT:		//Mifare认证
          IrqEn=0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
          WaitFor=0x10;		//认证寻卡等待时候 查询空闲中断标志位
          break;
			 
       case PCD_TRANSCEIVE:		//接收发送 发送接收
          IrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
          WaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
          break;
			 
       default:
         break;
			 
    }
   
    WriteRawRC( ComIEnReg,IrqEn|0x80);		//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反 
    ClearBitMask( ComIrqReg,0x80 );			//Set1该位清零时，CommIRqReg的屏蔽位清零
    WriteRawRC( CommandReg,PCD_IDLE );		//写空闲命令
    SetBitMask( FIFOLevelReg,0x80);			//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
    
    for (L= 0; L<intLen;L++ )
		  WriteRawRC(FIFODataReg,Data [L]);    		//写数据进FIFOdata
			
    WriteRawRC(CommandReg,Command);					//写命令
   
    
    if (Command == PCD_TRANSCEIVE )
			SetBitMask(BitFramingReg,0x80);  				//StartSend置位启动数据发送 该位与收发命令使用时才有效
    
    L= 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms
		
    do 														//认证 与寻卡等待时间	
    {
         N = ReadRC ( ComIrqReg );							//查询事件中断
         L--;
    } while ((L!=0)&&(!(N&0x01))&&(!(N&WaitFor ) ) );		//退出条件i=0,定时器中断，与写空闲命令
		
    ClearBitMask ( BitFramingReg, 0x80 );					//清理允许StartSend位
		
    if (L!= 0 )
    {
			if ( ! ( ReadRC ( ErrorReg ) & 0x1B ) )			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
			{
				Status =True ;
				
				if ( N & IrqEn & 0x01 )					//是否发生定时器中断
				  Status =Irq_falg ;   
					
				if (Command ==PCD_TRANSCEIVE )
				{
					N = ReadRC ( FIFOLevelReg );			//读FIFO中保存的字节数
					
					LastBits = ReadRC(ControlReg ) & 0x07;	//最后接收到得字节的有效位数
					
					if(LastBits )
						* outLen=(N - 1 )*8+LastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
					else
						*outLen=N * 8;   					//最后接收到的字节整个字节有效
					
					if (N ==0)		
            N = 1;    
					
					if (N >max)
						N =max;   
					
					for (L=0;L<N;L ++ )
					  Data[L] =ReadRC(FIFODataReg );   
					
					}
					
      }
			
			else
				Status =Fault;   
			
    }
   
   SetBitMask ( ControlReg, 0x80 );           // stop timer now
   WriteRawRC ( CommandReg, PCD_IDLE ); 
//	if(Status == True)	 
//		//printf("Status = %u\n",Status);
   return Status;
				
}


/*******************************************************************************************
*函数功能:寻卡
*参数：Card_code，寻卡方式
*                    = 0x52，寻感应区内所有符合14443A标准的卡
*                     = 0x26，寻未进入休眠状态的卡
*        CardType，卡片类型代码
*                   = 0x4400，Mifare_UltraLight
*                  = 0x0400，Mifare_One(S50)
*                   = 0x0200，Mifare_One(S70)
*                  = 0x0800，Mifare_Pro(X))
*                   = 0x4403，Mifare_DESFire
* 返回值:1 ,成功 0，失败
 *******************************************************************************************/
char PcdRequest ( u8 Req_code, u8 * CardType )
{
   char Status;  
	 u8 buffer[max]; 
   u32 Len;

   ClearBitMask ( Status2Reg, 0x08 );	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
   WriteRawRC ( BitFramingReg, 0x07 );	//发送的最后一个字节的 七位
   SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号

   buffer[0] =Req_code;		//存入 卡片命令字

   Status = PcdComMF522(PCD_TRANSCEIVE,buffer,1,buffer,&Len );	//寻卡  
	//printf("Len = %u\n",Len);
   //if ((Status ==True)&&(Len == 0x10) )	//寻卡成功返回卡类型 
	if (Status ==True)	
   {    
       * CardType=buffer[0];
       * (CardType+1)=buffer[1];
	   //printf("Len = %u\n",Len);
   }
	 
   else if(Status == Fault)
     Status =Fault;

   return Status;
	
}

 
/*******************************************************************************************
*函数功能:防冲撞
*参数:接收ID号缓冲区
*返回值:1,成功，0，失败
*
********************************************************************************************/
char PcdAnticoll ( u8 * Snr )
{
	
    char Status;
    u8 uc, Snr_check = 0;
    u8 buffer[max]; 
	  u32 Len;  
    ClearBitMask ( Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    WriteRawRC ( BitFramingReg, 0x00);		//清理寄存器 停止收发
    ClearBitMask ( CollReg, 0x80 );			//清ValuesAfterColl所有接收的位在冲突后被清除
   
   buffer[ 0 ] = 0x93;	//卡片防冲突命令
   buffer[ 1 ] = 0x20;
   
    Status =PcdComMF522(PCD_TRANSCEIVE,buffer, 2,buffer,&Len);//与卡片通信
	
    if (Status==True )		//通信成功
    {
		
			for ( uc = 0; uc < 4; uc++ )
			{
				* ( Snr + uc )  = buffer[ uc ];			//读出UID
				Snr_check ^= buffer[ uc ];
//				printf("buffer = %u\n",buffer[uc]);
				
			}
//			printf("Snr = %u\n",*Snr);
//			printf("buf = %u\n",*buffer);
			
      if ( Snr_check !=buffer[ uc ] )
				Status =Fault;    
				 
    }
    
    SetBitMask ( CollReg, 0x80 );
		
		
    return Status;
		
		
}


/*******************************************************************************************
函数功能:用RC522计算CRC16
参数:intdata，计算CRC16的数组
     Len，计算CRC16的数组字节长度
     0utData，存放计算结果存放的首地址
*返回值:无
 *******************************************************************************************/
void CalulateCRC ( u8 * intData, u8 Len, u8 * outData )
{
	  u8 k,j;
	
	
    ClearBitMask(DivIrqReg,0x04);
	
    WriteRawRC(CommandReg,PCD_IDLE);
	
    SetBitMask(FIFOLevelReg,0x80);
	
    for (k= 0;k<Len;k++)
	    WriteRawRC ( FIFODataReg, * (intData +k) );   

    WriteRawRC ( CommandReg, PCD_CALCCRC );
	
    k= 0xFF;
	
    do 
    {
        j= ReadRC(DivIrqReg );
        k--;
    } while ( (k!= 0) && ! (j& 0x04 ) );
		
   outData[0] = ReadRC(CRCResultRegL);
   outData[1] = ReadRC(CRCResultRegM);
		
		
}


/*******************************************************************************************
函数功能:选定卡片
参数:pSnr，卡片序列号，4字节
*返回值: 1,成功,0，失败
*
********************************************************************************************/
char PcdSelect ( u8 * pSnr )
{
    char ucN;
    u8 uc;
	u8 buffer[max]; 
    u32  ulLen;
    
    
    buffer[0] = PICC_ANTICOLL1;
    buffer[1] = 0x70;
    buffer[6] = 0;
	
    for ( uc = 0; uc < 4; uc ++ )
    {
    	buffer[ uc + 2 ] = * ( pSnr + uc );
    	buffer[ 6 ] ^= * ( pSnr + uc );
//		
//		printf("Select = %u\n",buffer[uc+2]);
	}
	//printf("Select = %u\n",*buffer);
		
    CalulateCRC (buffer, 7, &buffer[ 7 ] );//计算数据的长度
  
    ClearBitMask ( Status2Reg, 0x08 );	//给对应的寄存器数据清除

    ucN = PcdComMF522 ( PCD_TRANSCEIVE,buffer, 9,buffer, &ulLen );//写入数据
	//printf("ucN = %u\n",ucN);
    //if ( ( ucN ==True ) && ( ulLen == 0x18 ) )
	if ( ucN ==True )
      ucN =True ;  
    else
      ucN =Fault;    
		
    return ucN;
		
		
}
/*******************************************************************************************
*函数功能:验证卡片密码
*参数:mode，密码验证模式
*        0x60，验证A密钥
*        0x61，验证B密钥
*        u8 Addr，块地址
*        Key，密码
*        Str，卡片序列号，4字节
*返回值: 1,成功,0，失败
 *******************************************************************************************/
char PcdAuthState ( u8 mode, u8 Addr, u8 * Key, u8 * str )
{
    char Status;
	u8 uc, buffer[max];
    u32 Len;
    
	
    buffer[ 0 ] =mode;
    buffer[ 1 ] = Addr;
	
    for ( uc = 0; uc < 6; uc ++ )
	   buffer[uc + 2 ] = * ( Key + uc );   
	
    for ( uc = 0; uc < 6; uc ++ )
	    buffer[ uc + 8 ] = * (str + uc );   

    Status = PcdComMF522 ( PCD_AUTHENT,buffer, 12,buffer, &Len );
	printf("buffer = %X Len = %X\n",buffer[0],Len);
    if ( ( Status !=True ) || ( ! ( ReadRC ( Status2Reg ) & 0x08 ) ) )
      Status =Fault;   
    	
    return Status;
		
		
}


/*******************************************************************************************
*函数功能:写数据到M1卡一块
*参数:u8 addr，块地址
*     Data，写入的数据，16字节
*返回值:1,成功，0，失败
 *******************************************************************************************/
char PcdWrite ( u8 addr, u8 * Data )
{
    char Status;
	u8 uc,buffer[max];
    u32 Len;
         
    buffer[ 0 ] = PICC_WRITE;
    buffer [ 1 ] =addr;
	
    CalulateCRC (buffer, 2, &buffer[ 2 ] );
    
   Status = PcdComMF522 ( PCD_TRANSCEIVE,buffer, 4,buffer, & Len );
    
    if ((Status != True  )||(Len!=4)||((buffer[ 0 ] & 0x0F ) != 0x0A ) )
      Status =Fault;  
        
    if (Status==True  )
    {
			//memcpy(ucComMF522Buf, pData, 16);
      for ( uc = 0; uc < 16; uc ++ )
			  buffer[uc] = * ( Data + uc );  
			
      CalulateCRC ( buffer, 16, &buffer[ 16 ] );

     Status = PcdComMF522 ( PCD_TRANSCEIVE,buffer, 18,buffer,&Len );
		
		if ( ( Status != True )||(Len !=4)||((buffer[0]&0x0F)!=0x0A))
			;
        Status =Fault;   
			
    } 
		
		
    return Status;
		
		
}


/*******************************************************************************************
*函数功能:读取M1卡一块数据
*参数:u8 Aaddr，块地址
*       Data，读出的数据，16字节
*返回值:1,成功,0,失败
*
********************************************************************************************/
char PcdRead (u8 addr,u8 *Data )
{
    char Status;
	u8 L, buffer[max]; 
    u32 Len;
   
    buffer[0]=PICC_READ;
    buffer[1]=addr;
	
    CalulateCRC (buffer, 2, &buffer[ 2 ] ); 
   
    Status = PcdComMF522 ( PCD_TRANSCEIVE,buffer, 4,buffer,&Len );
	
    if ((Status==True  ) && (Len == 0x90 ) )
    {
			for (L= 0; L< 16;L++ )
        * (Data+L) = buffer[L];   
    }
		
    else
      Status =Fault;   
    
		
    return Status;
			
}
/*******************************************************************************************
*函数功能:命令卡片进入休眠状态
*参数:无
*返回值:1，成功，0，失败
********************************************************************************************/
char PcdHalt(void)
{
	u8 buffer[max]; 
	u32  Len;
  

  buffer[0]=PICC_HALT;
  buffer[1]=0;
	
  CalulateCRC (buffer, 2, &buffer[ 2 ] );
 	PcdComMF522(PCD_TRANSCEIVE,buffer, 4,buffer, &Len);

  return True ;
	
}
/****************************************************************************************************************
*函数功能:RC522测试
*参数:无
*返回值:无
******************************************************************************************************************/

void RC522_test(void)
{
	char Str[30];

  u8 Card_ID[4];       
  while (1)
  { 
	PcdRequest (PICC_REQALL, Card_ID); //寻卡		                                                 //若失败再次寻卡
	while(PcdAnticoll(Card_ID)==True )                                                                   //防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）
	{
		sprintf(Str,"\r\nID: %02X%02X%02X%02X",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);	
		printf ("%s\r\n",Str);	
		if(PcdSelect(Card_ID) == True)//选择卡片成功
		{
			printf("success select\n");
			if(PcdAuthState(PICC_AUTHENT1A,PICC_WRITE,0,Card_ID) == True)
			{
				printf("success PcdAuthState\n");
			}
			
		}else if(PcdSelect(Card_ID) == Fault){
			printf("fail select\n");
		}		
	}
	
  }
	
}

