
// - - * * * * * * * HT1621 ���������� * * * * * * *
// - - * MCU: STM32F1 ��Ƭ�� *
// - - * MCU-Crystal:  *
// - - * Version: 00 �汾 *
// - - * Last Updata: *
// - - * Author: Zhanglang *
// - - * Description: HT1621 ����������   *
// - - * C ���Գ��� *
// - - * * * * * * * * * * * * * * * * * * * * * * *

/*  HT1621����ԭ�� * 
������Ļ����32x4=128���Σ�ÿ���εľ���λ������SEG��Address��ͬ�����ģ�
ÿһ��SEG�����ĸ��Σ�ÿһ��Addr��ַ��Ӧ��λ���ݣ�
D3-D0��λ��ǰ����λ�ں󣬱���Ҫ����SEG0���ߵ�COM0�ε�����
ֻ�����ַ0x000000д��0x0001���ɵ�����
2017-3-29 10:01:12 */
/*��Zhang_51 ��������ֲ������2020-12-7 23:02:56*/
/********************************************************
��������: HT1621оƬ���Գ���
˵ ���������Գ����ܹ�����HT1621��ÿһ���ֶΣ�
���ε���ÿһ���ֶ�
������
********************************************************/
#ifndef __HT1621_H__
#define __HT1621_H__

#include "config.h"

//#define uchar unsigned char
//#define u32 unsigned int

#define BIAS 0x52 //0b1000 0101 0010 1/3duty 4com
#define SYSDIS 0X00 //0b1000 0000 0000 ����ϵͳ������LCDƫѹ������
#define SYSEN 0X02 //0b1000 0000 0010 ��ϵͳ����
#define LCDOFF 0X04 //0b1000 0000 0100 ��LCDƫѹ
#define LCDON 0X06 //0b1000 0000 0110 ��LCDƫѹ
#define XTAL 0x28 //0b1000 0010 1000 �ⲿ��ʱ��
#define RC256 0X30 //0b1000 0011 0000 �ڲ�ʱ��
#define TONEON 0X12 //0b1000 0001 0010 ���������
#define TONEOFF 0X10 //0b1000 0001 0000 �ر��������
#define WDTDIS 0X0A //0b1000 0000 1010 ��ֹ���Ź�

#define HT1621_LED_ON   P60 = 1   // - - LED����
#define HT1621_LED_OFF  P60 = 0
#define HT1621_CS_ON    P61 = 1	  // - - HT1621ʹ������
#define HT1621_CS_OFF   P61 = 0
#define HT1621_WR_ON    P62 = 1   // - - HT1621дʹ������
#define HT1621_WR_OFF   P62 = 0
#define HT1621_DAT_ON   P63 = 1    // - - HT1621��������
#define HT1621_DAT_OFF  P63 = 0
//#define HT1621_RD_ON  = GPIO_SetBits(GPIOB,GPIO_Pin_0);
//#define HT1621_RD_OFF = GPIO_SetBits(GPIOB,GPIO_Pin_0);

// - - �������,����
extern
u8 const Ht1621Tab[];
extern
u8 const Ht1621Tab_Num[];

// - - �����ӳ�����
void Ht1621_Init(void); // - - ��ʼ�� HT1621
void Ht1621Wr_Dat(u8 Dat,u8 cnt); // - - HT1621 д�����ݺ���
void Ht1621WrCmd(u8 Cmd); // - - HT1621 д�������
void Ht1621WrOneDat(u8 Addr,u8 Dat); // - - HT1621 ��ָ����ַд�����ݺ���
void Ht1621WrAllDat(u8 Addr,u8 *p,u8 cnt); // - - HT1621 ����д�����ݺ���
void Ht1621_DisplayNum(u8 seat,u8 Sym,u8 num);    //��ʾ����, Sym=1 ������
//void Ht1621_DisplaySymbol(u8 seat,const char *Sym);  //��ʾLCD����, �Ƚ��ַ���ʽ
void Ht1621_DisplaySymbol(u8 Sym);  //��ʾLCD����
void Ht1621Clear(u8 Addr,u8 cnt); // - - HT1621 ����
void Ht1621ShowAll(void); // - - HT1621 ȫ�Բ���
void Ht1621ShowDash(u8 seat); 

#endif


