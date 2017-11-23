#include "rc522.h"


 /*************************************
 *  ʹ��SPI��RC522ͨ��
 *
 *
 **************************************/
/********************************************************************************
*��������:SPI��������
*����:�����͵�����
*����ֵ:��
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
*��������:SPI������
*����:��
*����ֵ:����������
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
*��������:��RC522�Ĵ���
*����:address �Ĵ����ĵ�ַ
*����ֵ:�Ĵ����ĵ�ǰֵ
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
*��������:дRC522�Ĵ���
*����:Addresss �Ĵ����ĵ�ַ
     Value д��Ĵ�����ֵ
*����ֵ:��
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
*��������:��RC522�Ĵ�����λ
*����:Reg �Ĵ�����ֽ��
          mask ��λ��ֵ
* ����ֵ:��
 *******************************************************************************************/
void SetBitMask(u8 Reg,u8 Mask )  
{
    u8 Temp;
	
    Temp=ReadRC(Reg);
	
    WriteRawRC(Reg,Temp|Mask);         // set bit mask
	
}

/*******************************************************************************************
*��������:��RC522�Ĵ�������
*����:Reg RC522�ĵ�ַ
     Mask ���������
*����ֵ:��
*******************************************************************************************/
void ClearBitMask(u8 Reg,u8 Mask )  
{
    u8 Temp;
	
    Temp=ReadRC(Reg);
	
    WriteRawRC(Reg,Temp&(~Mask));  // clear bit mask
	
}
/*******************************************************************************************
*��������:������
*����:��
*����ֵ:��
********************************************************************************************/
void PcdAntennaOn(void)
{
    u8 temp;
	
    temp=ReadRC(TxControlReg);
	
    if (!(temp&0x03))
			SetBitMask(TxControlReg,0x03);		
}
/*******************************************************************************************
*��������:�ر�����
*����:��
*����ֵ:
*
 *******************************************************************************************/
void PcdAntennaOff ( void )
{
  ClearBitMask(TxControlReg,0x03 );
	
}


/*******************************************************************************************
*��������:��λRC522
*����:��
*����ֵ:��
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
	
  WriteRawRC(ModeReg, 0x3D );            //���巢�ͺͽ��ճ���ģʽ ��Mifare��ͨѶ��CRC��ʼֵ0x6363
	
  WriteRawRC(TReloadRegL,30);          //16λ��ʱ����λ    
	WriteRawRC(TReloadRegH,0);			     //16λ��ʱ����λ
	
  WriteRawRC(TModeReg,0x8D);				   //�����ڲ���ʱ��������
	
  WriteRawRC(TPrescalerReg,0x3E);			 //���ö�ʱ����Ƶϵ��
	
	WriteRawRC(TxAutoReg,0x40);				   //���Ʒ����ź�Ϊ100%ASK	
	

}

/*******************************************************************************************
*��������:����RC522�Ĺ�����ʽ
*����:CardType:������ʽ
*����ֵ:��
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
		PcdAntennaOn();//������		
   }

}


/*******************************************************************************************
*��������:ͨ��RC522��ISO14443��ͨѶ
*����:    Command��RC522������
*         intData��RC522���͵���Ƭ������
*         intLen���������ݵ��ֽڳ���
*         outData�����յ��Ŀ�Ƭ��������
*         outLen���������ݵĳ���
*����ֵ:
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
       case PCD_AUTHENT:		//Mifare��֤
          IrqEn=0x12;		//��������ж�����ErrIEn  ��������ж�IdleIEn
          WaitFor=0x10;		//��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
          break;
			 
       case PCD_TRANSCEIVE:		//���շ��� ���ͽ���
          IrqEn   = 0x77;		//����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
          WaitFor = 0x30;		//Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
          break;
			 
       default:
         break;
			 
    }
   
    WriteRawRC( ComIEnReg,IrqEn|0x80);		//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴ 
    ClearBitMask( ComIrqReg,0x80 );			//Set1��λ����ʱ��CommIRqReg������λ����
    WriteRawRC( CommandReg,PCD_IDLE );		//д��������
    SetBitMask( FIFOLevelReg,0x80);			//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����
    
    for (L= 0; L<intLen;L++ )
		  WriteRawRC(FIFODataReg,Data [L]);    		//д���ݽ�FIFOdata
			
    WriteRawRC(CommandReg,Command);					//д����
   
    
    if (Command == PCD_TRANSCEIVE )
			SetBitMask(BitFramingReg,0x80);  				//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч
    
    L= 1000;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
		
    do 														//��֤ ��Ѱ���ȴ�ʱ��	
    {
         N = ReadRC ( ComIrqReg );							//��ѯ�¼��ж�
         L--;
    } while ((L!=0)&&(!(N&0x01))&&(!(N&WaitFor ) ) );		//�˳�����i=0,��ʱ���жϣ���д��������
		
    ClearBitMask ( BitFramingReg, 0x80 );					//��������StartSendλ
		
    if (L!= 0 )
    {
			if ( ! ( ReadRC ( ErrorReg ) & 0x1B ) )			//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
			{
				Status =True ;
				
				if ( N & IrqEn & 0x01 )					//�Ƿ�����ʱ���ж�
				  Status =Irq_falg ;   
					
				if (Command ==PCD_TRANSCEIVE )
				{
					N = ReadRC ( FIFOLevelReg );			//��FIFO�б�����ֽ���
					
					LastBits = ReadRC(ControlReg ) & 0x07;	//�����յ����ֽڵ���Чλ��
					
					if(LastBits )
						* outLen=(N - 1 )*8+LastBits;   	//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
					else
						*outLen=N * 8;   					//�����յ����ֽ������ֽ���Ч
					
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
*��������:Ѱ��
*������Card_code��Ѱ����ʽ
*                    = 0x52��Ѱ��Ӧ�������з���14443A��׼�Ŀ�
*                     = 0x26��Ѱδ��������״̬�Ŀ�
*        CardType����Ƭ���ʹ���
*                   = 0x4400��Mifare_UltraLight
*                  = 0x0400��Mifare_One(S50)
*                   = 0x0200��Mifare_One(S70)
*                  = 0x0800��Mifare_Pro(X))
*                   = 0x4403��Mifare_DESFire
* ����ֵ:1 ,�ɹ� 0��ʧ��
 *******************************************************************************************/
char PcdRequest ( u8 Req_code, u8 * CardType )
{
   char Status;  
	 u8 buffer[max]; 
   u32 Len;

   ClearBitMask ( Status2Reg, 0x08 );	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
   WriteRawRC ( BitFramingReg, 0x07 );	//���͵����һ���ֽڵ� ��λ
   SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�

   buffer[0] =Req_code;		//���� ��Ƭ������

   Status = PcdComMF522(PCD_TRANSCEIVE,buffer,1,buffer,&Len );	//Ѱ��  
	//printf("Len = %u\n",Len);
   //if ((Status ==True)&&(Len == 0x10) )	//Ѱ���ɹ����ؿ����� 
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
*��������:����ײ
*����:����ID�Ż�����
*����ֵ:1,�ɹ���0��ʧ��
*
********************************************************************************************/
char PcdAnticoll ( u8 * Snr )
{
	
    char Status;
    u8 uc, Snr_check = 0;
    u8 buffer[max]; 
	  u32 Len;  
    ClearBitMask ( Status2Reg, 0x08 );		//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
    WriteRawRC ( BitFramingReg, 0x00);		//����Ĵ��� ֹͣ�շ�
    ClearBitMask ( CollReg, 0x80 );			//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����
   
   buffer[ 0 ] = 0x93;	//��Ƭ����ͻ����
   buffer[ 1 ] = 0x20;
   
    Status =PcdComMF522(PCD_TRANSCEIVE,buffer, 2,buffer,&Len);//�뿨Ƭͨ��
	
    if (Status==True )		//ͨ�ųɹ�
    {
		
			for ( uc = 0; uc < 4; uc++ )
			{
				* ( Snr + uc )  = buffer[ uc ];			//����UID
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
��������:��RC522����CRC16
����:intdata������CRC16������
     Len������CRC16�������ֽڳ���
     0utData����ż�������ŵ��׵�ַ
*����ֵ:��
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
��������:ѡ����Ƭ
����:pSnr����Ƭ���кţ�4�ֽ�
*����ֵ: 1,�ɹ�,0��ʧ��
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
		
    CalulateCRC (buffer, 7, &buffer[ 7 ] );//�������ݵĳ���
  
    ClearBitMask ( Status2Reg, 0x08 );	//����Ӧ�ļĴ����������

    ucN = PcdComMF522 ( PCD_TRANSCEIVE,buffer, 9,buffer, &ulLen );//д������
	//printf("ucN = %u\n",ucN);
    //if ( ( ucN ==True ) && ( ulLen == 0x18 ) )
	if ( ucN ==True )
      ucN =True ;  
    else
      ucN =Fault;    
		
    return ucN;
		
		
}
/*******************************************************************************************
*��������:��֤��Ƭ����
*����:mode��������֤ģʽ
*        0x60����֤A��Կ
*        0x61����֤B��Կ
*        u8 Addr�����ַ
*        Key������
*        Str����Ƭ���кţ�4�ֽ�
*����ֵ: 1,�ɹ�,0��ʧ��
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
*��������:д���ݵ�M1��һ��
*����:u8 addr�����ַ
*     Data��д������ݣ�16�ֽ�
*����ֵ:1,�ɹ���0��ʧ��
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
*��������:��ȡM1��һ������
*����:u8 Aaddr�����ַ
*       Data�����������ݣ�16�ֽ�
*����ֵ:1,�ɹ�,0,ʧ��
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
*��������:���Ƭ��������״̬
*����:��
*����ֵ:1���ɹ���0��ʧ��
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
*��������:RC522����
*����:��
*����ֵ:��
******************************************************************************************************************/

void RC522_test(void)
{
	char Str[30];

  u8 Card_ID[4];       
  while (1)
  { 
	PcdRequest (PICC_REQALL, Card_ID); //Ѱ��		                                                 //��ʧ���ٴ�Ѱ��
	while(PcdAnticoll(Card_ID)==True )                                                                   //����ײ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����
	{
		sprintf(Str,"\r\nID: %02X%02X%02X%02X",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);	
		printf ("%s\r\n",Str);	
		if(PcdSelect(Card_ID) == True)//ѡ��Ƭ�ɹ�
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

