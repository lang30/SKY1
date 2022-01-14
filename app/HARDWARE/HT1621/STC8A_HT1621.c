//СLCD����
#include "STC8A_Delay.h"
#include "string.h"
#include "STC8A_HT1621.h"



u8 const Ht1621Tab[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
u8 const Ht1621Tab_Num[]={0x7D,0x60,0X3E,0x7A,0x63,0x5B,0x5F,0x70,0x7F,0x7B,0x77,0x4F,0x1D,0x6E,0x1F,0x17}; //СLCD����


//void delay_us(unsigned char us)		//@22.1184MHz
//{
//	unsigned char i;
//	i  = us * 3;
//	while (--i);
//}
/********************************************************
�������ƣ�void Ht1621_Init(void)
��������: HT1621��ʼ��
ȫ�ֱ�������
����˵������
����˵������
�� ����1.0
˵ ������ʼ����Һ���������ֶξ���ʾ
********************************************************/
void Ht1621_Init(void)
{
	HT1621_CS_ON;
	HT1621_WR_ON;
	HT1621_DAT_ON;
	delay_ms(5);
	Ht1621WrCmd(BIAS);
	Ht1621WrCmd(RC256); // - - ʹ���ڲ�����
	Ht1621WrCmd(SYSDIS); // - - ����ϵͳ������LCDƫѹ������
	Ht1621WrCmd(WDTDIS); // - - ��ֹ���Ź�
	Ht1621WrCmd(SYSEN); // - - ��ϵͳ����
	Ht1621WrCmd(LCDON); // - - ���������
}

/******************************************************
�������ƣ�void Ht1621Wr_Dat(u8 Dat,u8 cnt)
��������: HT1621����д�뺯��
ȫ�ֱ�������
����˵����DatΪ���ݣ�cntΪ����λ��
����˵������
˵ ����д���ݺ���,cntΪ��������λ��,���ݴ���Ϊ��λ��ǰ
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
�������ƣ�void Ht1621WrCmd(u8 Cmd)
��������: HT1621����д�뺯��
ȫ�ֱ�������
����˵����CmdΪд����������
����˵������
˵ ����д�������ʶλ100
********************************************************/
void Ht1621WrCmd(u8 Cmd)
{
	HT1621_CS_OFF;
	delay_us(10);
	Ht1621Wr_Dat(0x80,4); // - - д�������־100
	Ht1621Wr_Dat(Cmd,8); // - - д����������
	HT1621_CS_ON;
	delay_us(10);
}

/********************************************************
�������ƣ�void Ht1621WrOneDat(u8 Addr,u8 Dat)
��������: HT1621��ָ����ַд�����ݺ���
ȫ�ֱ�������
����˵����AddrΪд���ʼ��ַ��DatΪд������
����˵������
˵ ������ΪHT1621������λ4λ������ʵ��д������Ϊ�����ĺ�4λ
********************************************************/
void Ht1621WrOneDat(u8 Addr,u8 Dat)
{
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - д�����ݱ�־101
	Ht1621Wr_Dat(Addr<<2,6); // - - д���ַ����
	Ht1621Wr_Dat(Dat<<4,4); // - - д������
	HT1621_CS_ON;
	delay_us(5);
}

/********************************************************
�������ƣ�void Ht1621WrAllDat(u8 Addr,u8 *p,u8 cnt)
��������: HT1621����д�뷽ʽ����
ȫ�ֱ�������
����˵����AddrΪд���ʼ��ַ��*pΪ����д������ָ�룬
cntΪд����������
����˵������
˵ ����HT1621������λ4λ���˴�ÿ������Ϊ8λ��д������
������8λ����
********************************************************/
void Ht1621WrAllDat(u8 Addr,u8 *p,u8 cnt)
{
	u8 i;
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - д�����ݱ�־101
	Ht1621Wr_Dat(Addr<<2,6); // - - д���ַ����
	for (i=0;i<cnt;i++)
	{
		Ht1621Wr_Dat(*p,8); // - - д������
		p++;
	}
	HT1621_CS_ON;
	delay_us(5);
}
/********************************************************
�������ƣ�void Ht1621Clear(u8 Addr,u8 cnt)
��������: HT1621����д��0��ʽ���������Ļ
ȫ�ֱ�������
����˵����AddrΪд���ʼ��ַ��cntΪд����������
����˵������

˵ ����HT1621������λ4λ���˴�ÿ������Ϊ8λ��д������
������8λ����
********************************************************/
void Ht1621Clear(u8 Addr,u8 cnt)
{
	u8 i;
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - д�����ݱ�־101
	Ht1621Wr_Dat(Addr<<2,6); // - - д���ַ����
	for (i=0;i<cnt;i++)
	{
		Ht1621Wr_Dat(0,8); // - - д������
	}
	HT1621_CS_ON;
	delay_us(5);
}
/********************************************************
�������ƣ�void Ht1621ShowAll()
��������: HT1621����д��F��ʽ����ȫ����ʾ
ȫ�ֱ�������
********************************************************/
void Ht1621ShowAll()
{
	u8 i;
	HT1621_CS_OFF;
	Ht1621Wr_Dat(0xa0,3); // - - д�����ݱ�־101
	Ht1621Wr_Dat(0<<2,6); // - - д���ַ����
	for (i=0;i<32;i++)
	{
		Ht1621Wr_Dat(0xf,8); // - - д������
	}
	HT1621_CS_ON;
	delay_us(5);
}
/********************************************************
�������ƣ�void Ht1621Dash()
��������: ��seatλ �� -
ȫ�ֱ�������
********************************************************/
void Ht1621ShowDash(u8 seat)
{
	u8 seg1,seg2;
	switch (seat){               						// ͨ��swith�жϵ�nλ��SEG ���ߣ�
			case 1: seg1 = 10; seg2 = 9; break;
			case 2: seg1 = 8; seg2 = 7; break;
			case 3: seg1 = 6; seg2 = 5; break;
			case 4: seg1 = 4; seg2 = 3; break;
			case 5: seg1 = 2; seg2 = 1; break;
			case 6: seg1 = 14; seg2 = 13; break;
			case 7: seg1 = 16; seg2 = 15; break;
	}
	Ht1621WrOneDat(seg1, 0x02);      //��SEGx ������ʾ�����ֵĵ�4 COM �ε�����
	Ht1621WrOneDat(seg2, 0);	//��SEGx ������ʾ�����ֵĵ�4 COM �ε�����  ��SEG���һ��������ʾ��
}

/****************************************
HT1621 C51 ������ʾ���� �� 
seat ѡ��Ҫ��ʾ�ĵ�Nλ����  num ��ʾ����
Sym=1 ��������ʾ
*****************************************/
void Ht1621_DisplayNum(u8 seat,u8 Sym,u8 num) 
{
	u8 seg1,seg2;
	switch (seat){               						// ͨ��swith�жϵ�nλ��SEG ���ߣ�
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
		Ht1621WrOneDat(seg1,Ht1621Tab_Num[num]+0x80);      //��SEGx ������ʾ�����ֵĵ�4 COM �ε�����
		Ht1621WrOneDat(seg2,Ht1621Tab_Num[num]+0x80>>4);	//��SEGx ������ʾ�����ֵĵ�4 COM �ε�����  ��SEG���һ��������ʾ��
	}
	else
	{
		Ht1621WrOneDat(seg1,Ht1621Tab_Num[num]);       //��SEGx ������ʾ�����ֵĵ�4 COM �ε�����
		Ht1621WrOneDat(seg2,Ht1621Tab_Num[num]>>4);	//��SEGx ������ʾ�����ֵĵ�4 COM �ε�����  ��SEG���һ��������ʾ��
	}
}
/****************************************
HT1621 C51 ���ַ��ź��� �� 
seat ѡ��Ҫ��ʾ�ĵ�Nλ����
Sym ������ʾ
*****************************************/
void Ht1621_DisplaySymbol(u8 Sym)  //������ʾLCD����
{
	u8 seg,dat;
	switch(Sym){
		case '>': seg = 17; dat = 0x08; break;            // ������ʾ�� ��
		case '<': seg = 17; dat = 0x04; break;            // ������ʾ�� ��
		case '*': seg = 17; dat = 0x02; break;            // ������ʾ�� *
		case '#': seg = 17; dat = 0x01; break;            // ������ʾ�� #
		case 'g': seg = 18; dat = 0x08; break;            // ������ʾ�� ��ʧ
		case 't': seg = 18; dat = 0x04; break;            // ������ʾ�� ͣ
		case '^': seg = 18; dat = 0x02; break;            // ������ʾ�� ?
		case '@': seg = 18; dat = 0x01; break;            // ������ʾ�� ?
		case 'k': seg = 0; dat = 0x08; break;            // ������ʾ�� ��ֵ
		case '!': seg = 0; dat = 0x04; break;            // ������ʾ�� ��
		case 'c': seg = 0; dat = 0x02; break;            // ������ʾ�� ����
		case 'F': seg = 0; dat = 0x01; break;            // ������ʾ�� ��
		case 'n': seg = 25; dat = 0x08; break;            // ������ʾ�� �ڲ�����
		case 'o': seg = 25; dat = 0x04; break;            // ������ʾ�� ok
		case 's': seg = 25; dat = 0x02; break;            // ������ʾ�� ��ʱ
		case 'l': seg = 25; dat = 0x01; break;            // ������ʾ�� ����
	}
	Ht1621WrOneDat(seg,dat);
}




/*if(seat==2 && strcmp(Sym,":")==0)       //�Ƚ��ַ���ʽ
	{
		Ht1621WrOneDat(5,0x80);      //��SEGx ������ʾ�����ֵĵ�4 COM �ε�����
		Ht1621WrOneDat(6,0x80>>4);	  //��SEGx ������ʾ�����ֵĵ�4 COM �ε�����  ��SEG���һ��������ʾ��
	}*/