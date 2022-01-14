/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include  "STC8A_SPI.h"

u8 	SPI_RxTimerOut;
u8 	SPI_BUF_type SPI_RxBuffer[SPI_BUF_LENTH];


//========================================================================
// ����: void	SPI_Init(SPI_InitTypeDef *SPIx)
// ����: SPI��ʼ������.
// ����: SPIx: �ṹ����,��ο�spi.h��Ķ���.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void	SPI_Init(SPI_InitTypeDef *SPIx)
{
	if(SPIx->SPI_SSIG == ENABLE)			SPCTL &= ~(1<<7);	//enable SS, conform Master or Slave by SS pin.
	else									SPCTL |=  (1<<7);	//disable SS, conform Master or Slave by SPI_Mode
	SPI_Start(SPIx->SPI_Enable);
	SPI_FirstBit_Set(SPIx->SPI_FirstBit);
	SPI_Mode_Set(SPIx->SPI_Mode);
	SPI_CPOL_Set(SPIx->SPI_CPOL);
	SPI_CPHA_Set(SPIx->SPI_CPHA);
	SPI_Clock_Select(SPIx->SPI_Speed);
	
	SPI_RxTimerOut = 0;
}

//========================================================================
// ����: void	SPI_SetMode(u8 mode)
// ����: SPI��������ģʽ����.
// ����: mode: ָ��ģʽ, ȡֵ SPI_Mode_Master �� SPI_Mode_Slave.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void	SPI_SetMode(u8 mode)
{
	if(mode == SPI_Mode_Slave)
	{
		SPCTL &= ~(1<<4);	//��������Ϊ�ӻ�����
		SPCTL &= ~(1<<7);	//SS����ȷ������
	}
	else
	{
		SPCTL |= (1<<4);	//ʹ�� SPI ����ģʽ
		SPCTL |= (1<<7);	//����SS���Ź���
	}
}

//========================================================================
// ����: void SPI_WriteByte(u8 dat)
// ����: SPI����һ���ֽ�����.
// ����: dat: Ҫ���͵�����.
// ����: none.
// �汾: V1.0, 2020-09-14
//========================================================================
void SPI_WriteByte(u8 dat)
{
    SPDAT = dat;
    while((SPSTAT & SPIF) == 0) ;
    SPSTAT = SPIF + WCOL;   //��0 SPIF��WCOL��־
}

u8 SPI_ReadByte(void)
{
    SPDAT = 0xff;
    while((SPSTAT & SPIF) == 0) ;
    SPSTAT = SPIF + WCOL;   //��0 SPIF��WCOL��־
    return (SPDAT);
}

/**
  * @brief  ��SPI����д���ֽ�����
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @param  p_TxData �������ݻ������׵�ַ
  * @param	sendDataNum ���������ֽ���
  * @retval ���ݷ���״̬
  *		@arg 0 ���ݷ��ͳɹ�
  * 	@arg -1 ���ݷ���ʧ��
  */
u8 SPI_WriteNBytes(u8 *p_TxData,u8 sendDataNum)
{
	u16 retry=0;
	while(sendDataNum--)
	{
		SPDAT =  *p_TxData++;		//����һ���ֽ�
		while((SPSTAT & SPIF) == 0);	//�ȴ��������
		{		
			retry++;
			if(retry>2000)  return -1;		
		}	
		retry = 0;
		SPSTAT = SPIF + WCOL;			//��0 SPIF��WCOL��־
	}
	return 0;
}
/**
  * @brief  ��SPI���߶�ȡ���ֽ�����
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @param  p_RxData ���ݴ����ַ
  * @param	readDataNum ��ȡ�����ֽ���
  * @retval ���ݶ�ȡ״̬
  *		@arg 0 ���ݶ�ȡ�ɹ�
  * 	@arg -1 ���ݶ�ȡʧ��
  */
u8 SPI_ReadNBytes(u8 *p_RxData,u8 readDataNum)
{
	u16 retry=0;
	u8 dadt=0;
	while(readDataNum--){
		SPDAT = 0xFF;
		while((SPSTAT & SPIF) == 0);
		{
			retry++;
			if(retry>2000)return -1;
		}
		retry = 0;
		SPSTAT = SPIF + WCOL;   //��0 SPIF��WCOL��־
		*p_RxData++ = SPDAT;
	}	
	return 0;
}