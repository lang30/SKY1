//小LCD代码
#include "STC8A_Delay.h"
#include "string.h"
#include "STC8A_HT1621.h"



u8 const Ht1621Tab[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
u8 const Ht1621Tab_Num[]={0x7D,0x60,0X3E,0x7A,0x63,0x5B,0x5F,0x70,0x7F,0x7B,0x77,0x4F,0x1D,0x6E,0x1F,0x17}; //小LCD编码


//void delay_us(unsigned char us)		//@22.1184MHz
//{
//	unsigned char i;
//	i  = us * 3;
//	while (--i);
//}
/********************************************************
函数名称：void Ht1621_Init(void)
功能描述: HT1621初始化
全局变量：无
参数说明：无
返回说明：无
版 本：1.0
说 明：初始化后，液晶屏所有字段均显示
********************************************************/
void Ht1621_Init(void)
{
	HT1621_CS_ON;
	HT1621_WR_ON;
	HT1621_DAT_ON;
	delay_ms(5);
	Ht1621WrCmd(BIAS);
	Ht1621WrCmd(RC256); // - - 使用内部振荡器
	Ht1621WrCmd(SYSDIS); // - - 关振系统荡器和LCD偏压发生器
	Ht1621WrCmd(WDTDIS); // - - 禁止看门狗
	Ht1621WrCmd(SYSEN); // - - 打开系统振荡器
	Ht1621WrCmd(LCDON); // - - 打开声音输出
}

/******************************************************
函数名称：void Ht1621Wr_Dat(u8 Dat,u8 cnt)
功能描述: HT1621数据写入函数
全局变量：无
参数说明：Dat为数据，cnt为数据位数
返回说明：无
说 明：写数据函数,cnt为传送数据位数,数据传送为低位在前
*******************************************************/
void Ht1621Wr_Dat(u8 Dat,u8 cnt)
{
	u8 i;
	for (i=0;i<cnt;i++)
	{
		HT1621_WR_OFF;
		delay_us(10);
		if ((Dat & 0x80) !=0) 
			HT1621_DAT_ON;
        else 
			HT1621_DAT_OFF;
		delay_us(10);
		HT1621_WR_ON;
		delay_us(10);
		Dat <<= 1;
	}
}

/********************************************************
函数名称：void Ht1621WrCmd(u8 Cmd)
功能描述: HT1621命令写入函数
全局变量：无
参数说明：Cmd为写入命令数据
返回说明：无
说 明：写入命令标识位100
********************************************************/
void Ht1621WrCmd(u8 Cmd)
{
	HT1621_CS_OFF;
	delay_us(10);
	Ht1621Wr_Dat(0x80,4); // - - 写入命令标志100
	Ht1621Wr_Dat(Cmd,8); // - - 写入命令数据
	HT1621_CS_ON;
	delay_us(10);
}

/********************************************************
函数名称：void Ht1621WrOneDat(u8 Addr,u8 Dat)
功能描述: HT1621在指定地址写入数据函数
全局变量：无
参数说明：Addr为写入初始地址，Dat为写入数据
返回说明：无
说 明：因为HT1621的数据位4位，所以实际写入数据为参数的后4位
********************************************************/
void Ht1621WrOneDat(u8 Addr,u8 Dat)
{
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - 写入数据标志101
	Ht1621Wr_Dat(Addr<<2,6); // - - 写入地址数据
	Ht1621Wr_Dat(Dat<<4,4); // - - 写入数据
	HT1621_CS_ON;
	delay_us(5);
}

/********************************************************
函数名称：void Ht1621WrAllDat(u8 Addr,u8 *p,u8 cnt)
功能描述: HT1621连续写入方式函数
全局变量：无
参数说明：Addr为写入初始地址，*p为连续写入数据指针，
cnt为写入数据总数
返回说明：无
说 明：HT1621的数据位4位，此处每次数据为8位，写入数据
总数按8位计算
********************************************************/
void Ht1621WrAllDat(u8 Addr,u8 *p,u8 cnt)
{
	u8 i;
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - 写入数据标志101
	Ht1621Wr_Dat(Addr<<2,6); // - - 写入地址数据
	for (i=0;i<cnt;i++)
	{
		Ht1621Wr_Dat(*p,8); // - - 写入数据
		p++;
	}
	HT1621_CS_ON;
	delay_us(5);
}
/********************************************************
函数名称：void Ht1621Clear(u8 Addr,u8 cnt)
功能描述: HT1621连续写入0方式函数清除屏幕
全局变量：无
参数说明：Addr为写入初始地址，cnt为写入数据总数
返回说明：无

说 明：HT1621的数据位4位，此处每次数据为8位，写入数据
总数按8位计算
********************************************************/
void Ht1621Clear(u8 Addr,u8 cnt)
{
	u8 i;
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - 写入数据标志101
	Ht1621Wr_Dat(Addr<<2,6); // - - 写入地址数据
	for (i=0;i<cnt;i++)
	{
		Ht1621Wr_Dat(0,8); // - - 写入数据
	}
	HT1621_CS_ON;
	delay_us(5);
}
/********************************************************
函数名称：void Ht1621ShowAll()
功能描述: HT1621连续写入F方式函数全部显示
全局变量：无
********************************************************/
void Ht1621ShowAll()
{
	u8 i;
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - 写入数据标志101
	Ht1621Wr_Dat(0<<2,6); // - - 写入地址数据
	for (i=0;i<32;i++)
	{
		Ht1621Wr_Dat(0xf,8); // - - 写入数据
	}
	HT1621_CS_ON;
	delay_us(5);
}
/********************************************************
函数名称：void Ht1621Dash()
功能描述: 第seat位 显 -
全局变量：无
********************************************************/
void Ht1621ShowDash(u8 seat)
{
	u8 seg1,seg2;
	switch (seat){               						// 通过swith判断第n位的SEG 段线；
			case 1: seg1 = 10; seg2 = 9; break;
			case 2: seg1 = 8; seg2 = 7; break;
			case 3: seg1 = 6; seg2 = 5; break;
			case 4: seg1 = 4; seg2 = 3; break;
			case 5: seg1 = 2; seg2 = 1; break;
			case 6: seg1 = 14; seg2 = 13; break;
			case 7: seg1 = 16; seg2 = 15; break;
	}
	Ht1621WrOneDat(seg1, 0x02);      //在SEGx 段线显示该数字的的4 COM 段点亮，
	Ht1621WrOneDat(seg2, 0);	//在SEGx 段线显示该数字的的4 COM 段点亮，  两SEG组成一个数字显示；
}

/****************************************
HT1621 C51 数字显示函数 ； 
seat 选择要显示的第N位数，  num 显示的数
Sym=1 带符号显示
*****************************************/
void Ht1621_DisplayNum(u8 seat,u8 Sym,u8 num) 
{
	u8 seg1,seg2;
	switch (seat){               						// 通过swith判断第n位的SEG 段线；
			case 1: seg1 = 10; seg2 = 9; break;
			case 2: seg1 = 8; seg2 = 7; break;
			case 3: seg1 = 6; seg2 = 5; break;
			case 4: seg1 = 4; seg2 = 3; break;
			case 5: seg1 = 2; seg2 = 1; break;
			case 6: seg1 = 14; seg2 = 13; break;
			case 7: seg1 = 16; seg2 = 15; break;
	}
	if(num>=97)
		num = num-87;
	if(Sym==1)
	{
		Ht1621WrOneDat(seg1,Ht1621Tab_Num[num]+0x80);      //在SEGx 段线显示该数字的的4 COM 段点亮，
		Ht1621WrOneDat(seg2,Ht1621Tab_Num[num]+0x80>>4);	//在SEGx 段线显示该数字的的4 COM 段点亮，  两SEG组成一个数字显示；
	}
	else
	{
		Ht1621WrOneDat(seg1,Ht1621Tab_Num[num]);       //在SEGx 段线显示该数字的的4 COM 段点亮，
		Ht1621WrOneDat(seg2,Ht1621Tab_Num[num]>>4);	//在SEGx 段线显示该数字的的4 COM 段点亮，  两SEG组成一个数字显示；
	}
}
/****************************************
HT1621 C51 数字符号函数 ； 
seat 选择要显示的第N位数，
Sym 符号显示
*****************************************/
void Ht1621_DisplaySymbol(u8 Sym)  //单独显示LCD符号
{
	u8 seg,dat;
	switch(Sym){
		case '>': seg = 17; dat = 0x08; break;            // 符号显示： ↓
		case '<': seg = 17; dat = 0x04; break;            // 符号显示： ↑
		case '*': seg = 17; dat = 0x02; break;            // 符号显示： *
		case '#': seg = 17; dat = 0x01; break;            // 符号显示： #
		case 'g': seg = 18; dat = 0x08; break;            // 符号显示： 挂失
		case 't': seg = 18; dat = 0x04; break;            // 符号显示： 停
		case '^': seg = 18; dat = 0x02; break;            // 符号显示： ?
		case '@': seg = 18; dat = 0x01; break;            // 符号显示： ?
		case 'k': seg = 0; dat = 0x08; break;            // 符号显示： 卡值
		case '!': seg = 0; dat = 0x04; break;            // 符号显示： ！
		case 'c': seg = 0; dat = 0x02; break;            // 符号显示： 错误
		case 'F': seg = 0; dat = 0x01; break;            // 符号显示： 满
		case 'n': seg = 25; dat = 0x08; break;            // 符号显示： 内部数据
		case 'o': seg = 25; dat = 0x04; break;            // 符号显示： ok
		case 's': seg = 25; dat = 0x02; break;            // 符号显示： 计时
		case 'l': seg = 25; dat = 0x01; break;            // 符号显示： 计量
	}
	Ht1621WrOneDat(seg,dat);
}




/*if(seat==2 && strcmp(Sym,":")==0)       //比较字符方式
	{
		Ht1621WrOneDat(5,0x80);      //在SEGx 段线显示该数字的的4 COM 段点亮，
		Ht1621WrOneDat(6,0x80>>4);	  //在SEGx 段线显示该数字的的4 COM 段点亮，  两SEG组成一个数字显示；
	}*/