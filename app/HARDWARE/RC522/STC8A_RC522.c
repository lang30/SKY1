#include "STC8A_RC522.h"
#include "STC8A_SPI.h"
#include "STC8A_GPIO.h"
#include "STC8A_Switch.h"
#include "STC8A_Delay.h"
#include "string.h"

u8 Pcb;					//CPU��APDUָ������
void RC522_Init(void)
{
//	NVIC_SPI_Init(ENABLE,Priority_3);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	
	SPI_SW(SPI_P22_P23_P24_P25);	//SPI_P12_P13_P14_P15,SPI_P22_P23_P24_P25,SPI_P74_P75_P76_P77,SPI_P35_P34_P33_P32
	SPSTAT = SPIF + WCOL;   //��0 SPIF��WCOL��־
    RC522_NSS = 1;
	
    PcdReset();
	PcdAntennaOff();
	delay_ms(2);
	PcdAntennaOn();
}
//*************************************************************************
// ������	��GetCard()
// ����		��IC�����,����������
// ���		��req_code[IN]:Ѱ����ʽ, 0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�,0x26 = Ѱδ��������״̬�Ŀ�
// ����		��pTagType[OUT]����Ƭ���ʹ���
//			      	0x4400 = Mifare_UltraLight 
//			      	0x0400 = Mifare_One(S50)
//                	0x0200 = Mifare_One(S70)
//               	0x0800 = Mifare_Pro(X)
//                	0x4403 = Mifare_DESFire
//			  		snr[OUT]:IC��ID
// ����		���ɹ����� ST_OK
//*************************************************************************
void GetCard(u8 Reqcode, u8* pTagType, u8* pSnr)
{
    PcdRequest(Reqcode, pTagType);  				//Ѱ��
    PcdAnticoll(pSnr);								//����ײ, ��ȡID
	PcdSelect(pSnr);								//ѡ��Ƭ
}                            
/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//         pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 
   
   ClearBitMask(Status2Reg,0x08);    // �����������ͷ�������״̬��־
   WriteRawRC(BitFramingReg,0x07);   // ����λ��֡�ĵ���
   SetBitMask(TxControlReg,0x03);
//   
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//   UART_send_byte(status);
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   return status;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ClearBitMask(Status2Reg,0x08);    
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }

    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
    ClearBitMask(Status2Reg,0x08);
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                   
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          p [OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          p [IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    //char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    //status = 
	  PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
    RC522_RESET = 0;   //RST522_0;
    delay_us(100); 
    RC522_RESET = 1;     //RST522_1;RST522_1;
    delay_us(200);  
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_us(200);  
  
    WriteRawRC(ModeReg,0x3D);            //?Mifare???,CRC???0x6363
    WriteRawRC(TReloadRegL,30);         //?30?????           
    WriteRawRC(TReloadRegH,0);          
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);
    WriteRawRC(ControlReg,0x10);

    ClearBitMask(TestPinEnReg, 0x80);//off MX and DTRQ out
    WriteRawRC(TxAutoReg,0x40);
   
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
	unsigned char ucAddr;
	unsigned char ucResult=0;
	ucAddr = ((Address<<1)&0x7E)|0x80;
	RC522_NSS = 0;
	SPI_WriteNBytes(&ucAddr,1);
	SPI_ReadNBytes(&ucResult,1);
	RC522_NSS = 1;
	return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
	unsigned char ucAddr;
	u8 write_buffer[2]={0};
	ucAddr = ((Address<<1)&0x7E);
	write_buffer[0] = ucAddr;
	write_buffer[1] = value;
	RC522_NSS = 0;
	SPI_WriteNBytes(write_buffer,2);
	RC522_NSS = 1;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pIn [IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOut [OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);
   
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }
    
    i = 800 ; //600;//????????,??M1???????25ms
    do 
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
         }
         else
         {   status = MI_ERR;   }
        
   }

   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE); 
   return status;
}

/////////////////////////////////////////////////////////////////////
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}
/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}
//*************************************************************************
// ������	��CpuReset()
// ����		��CPU��ר�ø�λ
// ���		����
// ����		��Data_Out ����ĸ�λ������Ϣ������
// ����		���ɹ�����9000
//*************************************************************************
u16 CardReset(u8 * Data_Out,u8 *  Len)
{
	char status = MI_OK;	
	status = PcdRats(Data_Out,Len);										//��Ƭ��λ
	if(status)
	{
		return MI_ERR;
	}
	else

		return MI_OK;
}
//*************************************************************************
// ������	��PcdRats
// ����		��ת��APDU�����ʽ
// ���		�� ��
// ����		�� DataOut ��������ݣ�	Len ������ݵĳ���
// ����		���ɹ�����MI_OK
//*************************************************************************
char PcdRats(u8 * DataOut,u8 * Len)
{
	char status = MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);	// ���У��ɹ���־,���MFCrypto1Onλ
	memset(ucComMF522Buf, 0x00, MAXRLEN);

	ucComMF522Buf[0] = 0xE0;		
	ucComMF522Buf[1] = 0x51;				

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,DataOut,&unLen);// ���յ��Ŀ�Ƭ���ͺű���
	if (status == MI_OK)
	{
		Len[0]= unLen/8-2;
		status = MI_OK;
	}	 
	else
		status = MI_ERR;	 
	return status;								//���ؽ��
}
//*************************************************************************
// ������	��Pcd_Cmd
// ����		��ִ����λ��������ָ��
// ���		��pDataIn : Ҫִ�е�ָ�� In_Len  ָ�����ݳ���					
// ����		��pDataOut�����ִ�к�ķ�������   Out_Len��������ݳ���
// ����		��MI_OK
//*************************************************************************
u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len)
{
	char status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	u8 i;

	ClearBitMask(Status2Reg,0x08);					// ���У��ɹ���־,���MFCrypto1Onλ
	memset(ucComMF522Buf, 0x00, MAXRLEN);

 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = Pcb;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = pDataIn[0];				// CLA
	ucComMF522Buf[3] = pDataIn[1];				// INS 			
	ucComMF522Buf[4] = pDataIn[2];				// P1						 
	ucComMF522Buf[5] = pDataIn[3];				// P2					
	ucComMF522Buf[6] = pDataIn[4];				// LEN	
	
	for(i=0;i<ucComMF522Buf[6];i++)				//DATA
	{
		ucComMF522Buf[7+i] = pDataIn[5+i];	  
	}							
    CalulateCRC(ucComMF522Buf,In_Len+2,&ucComMF522Buf[In_Len+2]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,In_Len+4,pDataOut,&unLen); 							
	if (status == MI_OK)
	{
		Out_Len[0] = unLen/8-4;	 //���յ����ݵĳ���,��Ҫǰ��ͺ���ĸ������ֽڣ����Ƿ��ص���������		
		return MI_OK;
	}
	else
	{
	    Out_Len[0] = unLen/8-4;
		if((pDataOut[2]==0x90)&&(pDataOut[3]==0x00))
		    return MI_OK;
		else
		    return MI_ERR;
	}
}
//*************************************************************************
// ������	��PcdSwitchPCB(void)
// ����		���л������
// ���		��
// ����		��
// ����		���ɹ�����MI_OK
//*************************************************************************
void PcdSwitchPCB(void)
{
	switch(Pcb)
	{
		case 0x00:
			Pcb=0x0A;
			break;
		case 0x0A:
			Pcb=0x0B;
			break;
		case 0x0B:
			Pcb=0x0A;
			break;
        default:
            Pcb=0x0A;
			break;
	}
}

//void RC522_Config(unsigned char Card_Type)
//{
//	   ClearBitMask(Status2Reg,0x08);
//     WriteRawRC(ModeReg,0x3D);//3F
//     WriteRawRC(RxSelReg,0x86);//84
//     WriteRawRC(RFCfgReg,0x7F);   //4F
//   	 WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
//	   WriteRawRC(TReloadRegH,0);
//     WriteRawRC(TModeReg,0x8D);
//	   WriteRawRC(TPrescalerReg,0x3E);
////	   WriteRawRC(TxAutoReg,0x40);//???
//	   delay_ms(5);//delay_10ms(1);

//     PcdAntennaOn();
//}

//���RC522 SPIͨѶ�Ƿ�����
//void RC522_SPI_Check()
//{
//    if(ReadRawRC(ModeReg)!=0x3D)
//    {
//        if(ReadRawRC(TReloadRegL)!=0x30)
//        {
//            Ht1621WrOneData(18,0X01); 			//!!!��
//            Ht1621WrOneData(17,0x08);           //����

//            Ht1621WrOneData(9,0x00);           //-
//            Ht1621WrOneData(10,0x02);     
//            Ht1621WrOneData(7,0x00);           //r
//            Ht1621WrOneData(8,0x06);           
//            Ht1621_DisplayNum(3,0,'f');         //f
//            Ht1621_DisplayNum(4,0,1);           //i
//            Ht1621_DisplayNum(5,0,'d');         //d

//            Ht1621WrOneData(13,0x00);           
//            Ht1621WrOneData(14,0x00); 
//            Ht1621WrOneData(15,0x00);           
//            Ht1621WrOneData(16,0x00);          

//            delay_ms(1000);
//        }
//        delay_ms(1000);
//    }
//}