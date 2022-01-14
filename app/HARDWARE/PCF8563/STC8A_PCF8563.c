#include "STC8A_pcf8563.h"
#include "STC8A_UART.h"
#include "string.h"


//u8  sec ,min ,hour ,week ,day ,month ,year;
//struct _date Date; //ʱ����Ϣ�ṹ

/******************************************************************
 - �������ƣ�u8 PCF8563_GetTime(struct _date *date)
 - ����˵������ȡ����ʱ�������Ϣ
 - ����˵����struct _date *date ʱ����Ϣ�ṹ
 - ����˵������
 - ��ע˵����ʹ��51��Ƭ�����ⲿ����Ƶ��:22.1184MHZ-1Tģʽ
 ******************************************************************/
void PCF8563_GetTime(u8 *tmp)
{
    I2C_ReadNbyte(0x02, tmp, 7);
	
	tmp[0] &= 0x7f; 			//������Чλ
	tmp[0]   = ToDec(tmp[0]);			//BCD��תʮ������
	
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
 - �������ƣ�u8 PCF8563_SetTime(struct _date *date)
 - ����˵������������ʱ�������Ϣ
 - ����˵����struct _date *date ʱ����Ϣ�ṹ
 - ����˵������
 - ��ע˵����ʹ��51��Ƭ�����ⲿ����Ƶ��:22.1184MHZ-1Tģʽ
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
 - �������ƣ�u8 ToDec(u8 bcd)
 - ����˵������BCD��ת��Ϊ����ʱ��
 - ����˵����BCD��
 - ����˵��������ʱ��
 - ��ע˵����ʹ��51��Ƭ�����ⲿ����Ƶ��:22.1184MHZ-1Tģʽ
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
 - �������ƣ�u8 ToBcd(u8 hex)
 - ����˵������ʱ��ת��ΪBCD��
 - ����˵��������ʱ��ֵ
 - ����˵����BCD��
 - ��ע˵����ʹ��51��Ƭ�����ⲿ����Ƶ��:22.1184MHZ-1Tģʽ
******************************************************************/
u8 ToBcd(u8 hex)
{
  u8 a,b,bcd;
  a=hex /10;
	b=hex %10;
  bcd =(a<<4)|b;
	return bcd;
}

