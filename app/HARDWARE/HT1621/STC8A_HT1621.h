
// - - * * * * * * * HT1621 段码屏驱动 * * * * * * *
// - - * MCU: STM32F1 单片机 *
// - - * MCU-Crystal:  *
// - - * Version: 00 版本 *
// - - * Last Updata: *
// - - * Author: Zhanglang *
// - - * Description: HT1621 段码屏驱动   *
// - - * C 语言程序 *
// - - * * * * * * * * * * * * * * * * * * * * * * *

/*  HT1621驱动原理 * 
整个屏幕共有32x4=128个段，每个段的具体位置是由SEG和Address共同决定的，
每一条SEG线有四个段，每一个Addr地址对应四位数据，
D3-D0高位在前，低位在后，比如要控制SEG0段线的COM0段点亮，
只需向地址0x000000写入0x0001即可点亮。
2017-3-29 10:01:12 */
/*从Zhang_51 工程中移植过来，2020-12-7 23:02:56*/
/********************************************************
功能描述: HT1621芯片测试程序
说 明：本测试程序能够测试HT1621的每一个字段，
依次点亮每一个字段
参数：
********************************************************/
#ifndef __HT1621_H__
#define __HT1621_H__

#include "config.h"

//#define uchar unsigned char
//#define u32 unsigned int

#define BIAS 0x52 //0b1000 0101 0010 1/3duty 4com
#define SYSDIS 0X00 //0b1000 0000 0000 关振系统荡器和LCD偏压发生器
#define SYSEN 0X02 //0b1000 0000 0010 打开系统振荡器
#define LCDOFF 0X04 //0b1000 0000 0100 关LCD偏压
#define LCDON 0X06 //0b1000 0000 0110 打开LCD偏压
#define XTAL 0x28 //0b1000 0010 1000 外部接时钟
#define RC256 0X30 //0b1000 0011 0000 内部时钟
#define TONEON 0X12 //0b1000 0001 0010 打开声音输出
#define TONEOFF 0X10 //0b1000 0001 0000 关闭声音输出
#define WDTDIS 0X0A //0b1000 0000 1010 禁止看门狗

#define HT1621_LED_ON   P60 = 1   // - - LED背光
#define HT1621_LED_OFF  P60 = 0
#define HT1621_CS_ON    P61 = 1	  // - - HT1621使能引脚
#define HT1621_CS_OFF   P61 = 0
#define HT1621_WR_ON    P62 = 1   // - - HT1621写使能引脚
#define HT1621_WR_OFF   P62 = 0
#define HT1621_DAT_ON   P63 = 1    // - - HT1621数据引脚
#define HT1621_DAT_OFF  P63 = 0
//#define HT1621_RD_ON  = GPIO_SetBits(GPIOB,GPIO_Pin_0);
//#define HT1621_RD_OFF = GPIO_SetBits(GPIOB,GPIO_Pin_0);

// - - 定义变量,数组
extern
u8 const Ht1621Tab[];
extern
u8 const Ht1621Tab_Num[];

// - - 定义子程序函数
void Ht1621_Init(void); // - - 初始化 HT1621
void Ht1621Wr_Dat(u8 Dat,u8 cnt); // - - HT1621 写入数据函数
void Ht1621WrCmd(u8 Cmd); // - - HT1621 写入命令函数
void Ht1621WrOneDat(u8 Addr,u8 Dat); // - - HT1621 在指定地址写入数据函数
void Ht1621WrAllDat(u8 Addr,u8 *p,u8 cnt); // - - HT1621 连续写入数据函数
void Ht1621_DisplayNum(u8 seat,u8 Sym,u8 num);    //显示数字, Sym=1 带符号
//void Ht1621_DisplaySymbol(u8 seat,const char *Sym);  //显示LCD符号, 比较字符方式
void Ht1621_DisplaySymbol(u8 Sym);  //显示LCD符号
void Ht1621Clear(u8 Addr,u8 cnt); // - - HT1621 清屏
void Ht1621ShowAll(void); // - - HT1621 全显测试
void Ht1621ShowDash(u8 seat); 

#endif


