#include "STC8A_pcf8563.h"
#include "STC8A_UART.h"
#include "string.h"


//u8  sec ,min ,hour ,week ,day ,month ,year;
//struct _date Date; //时钟信息结构

/******************************************************************
 - 函数名称：u8 PCF8563_GetTime(struct _date *date)
 - 功能说明：获取所有时间相关信息
 - 参数说明：struct _date *date 时间信息结构
 - 返回说明：无
 - 备注说明：使用51单片机，外部晶体频率:22.1184MHZ-1T模式
 ******************************************************************/
void PCF8563_GetTime(u8 *tmp)
{
    I2C_ReadNbyte(0x02, tmp, 7);
	
	tmp[0] &= 0x7f; 			//屏蔽无效位
	tmp[0]   = ToDec(tmp[0]);			//BCD码转十六进制
	
	tmp[1] &= 0x7f;
	tmp[1]   = ToDec(tmp[1]);
	
	tmp[2] &= 0x3f;
	tmp[2]  = ToDec(tmp[2]);
	
	tmp[3] &= 0x3f;
	tmp[3]   = ToDec(tmp[3]);

	tmp[4] &= 0x07;
	tmp[4]  = ToDec(tmp[4]);
	
	tmp[5] &=0x1f;
	tmp[5] = ToDec(tmp[5]);
	
	tmp[6]  = ToDec(tmp[6]);
}
/******************************************************************
 - 函数名称：u8 PCF8563_SetTime(struct _date *date)
 - 功能说明：设置所有时间相关信息
 - 参数说明：struct _date *date 时间信息结构
 - 返回说明：无
 - 备注说明：使用51单片机，外部晶体频率:22.1184MHZ-1T模式
 ******************************************************************/
void PCF8563_SetTime(u8 *timeSet)
{
	I2C_WriteNbyte(2, &timeSet[5], 1);
	I2C_WriteNbyte(3, &timeSet[4], 1);
	I2C_WriteNbyte(4, &timeSet[3], 1);
	I2C_WriteNbyte(5, &timeSet[2], 1);
	I2C_WriteNbyte(6, &timeSet[6], 1);
	I2C_WriteNbyte(7, &timeSet[1], 1);
	I2C_WriteNbyte(8, &timeSet[0], 1);
}
/******************************************************************
 - 函数名称：u8 ToDec(u8 bcd)
 - 功能说明：将BCD码转换为正常时间
 - 参数说明：BCD码
 - 返回说明：正常时间
 - 备注说明：使用51单片机，外部晶体频率:22.1184MHZ-1T模式
******************************************************************/
u8 ToDec(u8 bcd)
{
  u8 a,b,hex;
  a=bcd /16;
	b=bcd %16;
  hex =a*10+b;
	return hex;
}
/******************************************************************
 - 函数名称：u8 ToBcd(u8 hex)
 - 功能说明：将时间转换为BCD码
 - 参数说明：正常时间值
 - 返回说明：BCD码
 - 备注说明：使用51单片机，外部晶体频率:22.1184MHZ-1T模式
******************************************************************/
u8 ToBcd(u8 hex)
{
  u8 a,b,bcd;
  a=hex /10;
	b=hex %10;
  bcd =(a<<4)|b;
	return bcd;
}

