/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include  "STC8A_SPI.h"

u8 	SPI_RxTimerOut;
u8 	SPI_BUF_type SPI_RxBuffer[SPI_BUF_LENTH];


//========================================================================
// 函数: void	SPI_Init(SPI_InitTypeDef *SPIx)
// 描述: SPI初始化程序.
// 参数: SPIx: 结构参数,请参考spi.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-11-22
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
// 函数: void	SPI_SetMode(u8 mode)
// 描述: SPI设置主从模式函数.
// 参数: mode: 指定模式, 取值 SPI_Mode_Master 或 SPI_Mode_Slave.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	SPI_SetMode(u8 mode)
{
	if(mode == SPI_Mode_Slave)
	{
		SPCTL &= ~(1<<4);	//重新设置为从机待机
		SPCTL &= ~(1<<7);	//SS引脚确定主从
	}
	else
	{
		SPCTL |= (1<<4);	//使能 SPI 主机模式
		SPCTL |= (1<<7);	//忽略SS引脚功能
	}
}

//========================================================================
// 函数: void SPI_WriteByte(u8 dat)
// 描述: SPI发送一个字节数据.
// 参数: dat: 要发送的数据.
// 返回: none.
// 版本: V1.0, 2020-09-14
//========================================================================
void SPI_WriteByte(u8 dat)
{
    SPDAT = dat;
    while((SPSTAT & SPIF) == 0) ;
    SPSTAT = SPIF + WCOL;   //清0 SPIF和WCOL标志
}

u8 SPI_ReadByte(void)
{
    SPDAT = 0xff;
    while((SPSTAT & SPIF) == 0) ;
    SPSTAT = SPIF + WCOL;   //清0 SPIF和WCOL标志
    return (SPDAT);
}

/**
  * @brief  向SPI总线写多字节数据
  * @param  SPIx 需要使用的SPI
  * @param  p_TxData 发送数据缓冲区首地址
  * @param	sendDataNum 发送数据字节数
  * @retval 数据发送状态
  *		@arg 0 数据发送成功
  * 	@arg -1 数据发送失败
  */
u8 SPI_WriteNBytes(u8 *p_TxData,u8 sendDataNum)
{
	u16 retry=0;
	while(sendDataNum--)
	{
		SPDAT =  *p_TxData++;		//发送一个字节
		while((SPSTAT & SPIF) == 0);	//等待发送完成
		{		
			retry++;
			if(retry>2000)  return -1;		
		}	
		retry = 0;
		SPSTAT = SPIF + WCOL;			//清0 SPIF和WCOL标志
	}
	return 0;
}
/**
  * @brief  从SPI总线读取多字节数据
  * @param  SPIx 需要使用的SPI
  * @param  p_RxData 数据储存地址
  * @param	readDataNum 读取数据字节数
  * @retval 数据读取状态
  *		@arg 0 数据读取成功
  * 	@arg -1 数据读取失败
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
		SPSTAT = SPIF + WCOL;   //清0 SPIF和WCOL标志
		*p_RxData++ = SPDAT;
	}	
	return 0;
}