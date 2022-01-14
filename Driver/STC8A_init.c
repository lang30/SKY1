#include "STC8A_init.h"
#include "STC8A_GPIO.h"
#include "STC8A_Exti.h"
#include "STC8A_I2C.h"
#include "STC8A_SPI.h"
#include "STC8A_Timer.h"
#include "STC8A_UART.h"
#include "STC8A_Delay.h"
#include "STC8A_Switch.h"
#include "STC8A_NVIC.h"
#include "STC8x_SYSCLK.h"

#include "beep.h"
#include "STC8A_led.h"
#include "seglcd.h"

extern uint32 MAIN_Fosc;

//========================================================================
//                                IO������
//========================================================================
void	GPIO_config(void)
{
	P0_MODE_IO_PU(GPIO_Pin_All);		//P0 ����Ϊ׼˫���
	P1_MODE_IN_HIZ(GPIO_Pin_LOW);		//P1.0~P1.3 ����Ϊ��������
	P1_MODE_IO_PU(GPIO_Pin_HIGH);		//P1.4~P1.7 ����Ϊ׼˫���
	P2_MODE_IO_PU(GPIO_Pin_All);		//P2 ����Ϊ׼˫���
	P3_MODE_IO_PU(GPIO_Pin_LOW);		//P3.0~P3.3 ����Ϊ׼˫���
	P3_MODE_IO_PU(GPIO_Pin_HIGH);		//P3.4~P3.7 ����Ϊ׼˫���
	P4_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_7);	//P4.0,P4.6,P4.7 ����Ϊ׼˫���
	P6_MODE_IO_PU(GPIO_Pin_All);		//P6 ����Ϊ׼˫���
	P7_MODE_IO_PU(GPIO_Pin_All);		//P7 ����Ϊ׼˫���
	
	
//�˿�ʹ��ǰҪ���ã���ʼ����
	P2_MODE_OUT_PP(GPIO_Pin_0|GPIO_Pin_6|GPIO_Pin_7);			//P2.0  LED | P2.6  ��ŷ�|P2.7  BEEP ����Ϊ�������
	P6_MODE_OUT_PP(GPIO_Pin_0);			//P6.0  LCD���� ����Ϊ�������
//��ʼ����ƽ
	P20 = 1;
	P26 = 0;
	P27 = 0;
	P60 = 0;
//Timer4 ������ io�ڳ�ʼ��
	P0_MODE_IN_HIZ(GPIO_Pin_6); 	//P0.6 ����Ϊ��������
	P0_PULL_UP_ENABLE(GPIO_Pin_6);		//P0.6 �����ڲ�����
}

//========================================================================
//                               ��ʱ������
//========================================================================
void Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;						//�ṹ����

	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000UL);		//�ж�Ƶ��, 1000��/��
	TIM_InitStructure.TIM_Run       = ENABLE;					//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);					//��ʼ��Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer0_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

//	//��ʱ��1��16λ�Զ���װ, �ж�Ƶ��Ϊ20000HZ���жϺ�����P6.6ȡ�����10KHZ�����ź�.
//	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
//	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//ָ��ʱ��Դ, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
//	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
//	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 20000);			//�ж�Ƶ��, 20000��/��
//	TIM_InitStructure.TIM_Run       = ENABLE;					//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
//	Timer_Inilize(Timer1,&TIM_InitStructure);					//��ʼ��Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
//	NVIC_Timer1_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

//	//��ʱ��2��16λ�Զ���װ, �ж�Ƶ��Ϊ10000HZ���жϺ�����P6.5ȡ�����5KHZ�����ź�.
//	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
//	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
//	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 10000);				//��ֵ
//	TIM_InitStructure.TIM_Run       = ENABLE;					//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
//	Timer_Inilize(Timer2,&TIM_InitStructure);					//��ʼ��Timer2	  Timer0,Timer1,Timer2,Timer3,Timer4
//	NVIC_Timer2_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�

	//��ʱ��3��16λ�Զ���װ, 1�����һ���жϣ�
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;					//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 0x9AA9;		     //��ֵ, 9AA9 1��������ж�
	TIM_InitStructure.TIM_Run       = DISABLE;					//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer3,&TIM_InitStructure);					//��ʼ��Timer3	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer3_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�

	//��ʱ��4������������(������T4/P0.6�ⲿ������м���)��T4T3M->T4_C/T ��1
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_Ext;	        //ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;					//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 0xffff;					//��ֵ ; ���ٸ���������жϣ� ��������1����������жϣ�
	TIM_InitStructure.TIM_Run       = DISABLE;					//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer4,&TIM_InitStructure);					//��ʼ��Timer4	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer4_Init(ENABLE,NULL);			//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
	
}

//========================================================================
//                              ADC��ʼ��
//========================================================================

//========================================================================
//                              ���ڳ�ʼ��
//========================================================================
void UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//ʹ�ò�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//��ʼ������1 UART1,UART2,UART3,UART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

//	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
//	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
//	UART_Configuration(UART2, &COMx_InitStructure);		//��ʼ������2 USART1,USART2,USART3,USART4
//	NVIC_UART2_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

//========================================================================
//                               I2C��ʼ��
//========================================================================
void I2C_config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;
	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//����ѡ��   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;						//I2C����ʹ��,   ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;					//����ʹ���Զ�����,  ENABLE, DISABLE
	I2C_InitStructure.I2C_Speed     = 16;								//�����ٶ�=Fosc/2/(Speed*2+4),      0~63
	I2C_Init(&I2C_InitStructure);
	NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_0);		//����ģʽ, I2C_Mode_Master, I2C_Mode_Slave; �ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	I2C_SW(I2C_P14_P15);					//I2C_P14_P15,I2C_P24_P25,I2C_P33_P32}
}
//========================================================================
//                               SPI��ʼ��
//========================================================================
void SPI_config(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	SPI_InitStructure.SPI_Enable    = ENABLE;						//SPI����    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;						//Ƭѡλ     ENABLE, DISABLE
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;					//��λ����   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;		//����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;			//ʱ����λ   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;		//���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Speed     = SPI_Speed_4;			//SPI�ٶ�    SPI_Speed_4, SPI_Speed_16, SPI_Speed_64, SPI_Speed_128
	SPI_Init(&SPI_InitStructure);
	//NVIC_SPI_Init(ENABLE,Priority_3);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3  
																				//RC522   ����NVIC_SPI_Init����Ӱ�죻
}

//========================================================================
//                             �Ƚ�����ʼ��
//========================================================================

//========================================================================
//                             �ⲿ�жϳ�ʼ��
//========================================================================
void Exti_config(void)
{
	EXTI_InitTypeDef	Exti_InitStructure;							//�ṹ����

	Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//�ж�ģʽ,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT0,&Exti_InitStructure);				//��ʼ��
	NVIC_INT0_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//�ж�ģʽ,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT1,&Exti_InitStructure);				//��ʼ��
	NVIC_INT1_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	NVIC_INT2_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
	NVIC_INT3_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
	NVIC_INT4_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
}

//========================================================================
//                             �ⲿ�жϳ�ʼ��
//========================================================================

//========================================================================
//                               ���ܽ��л�
//========================================================================
void Switch_config(void)
{
	UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
	UART2_SW(UART2_SW_P10_P11);		//UART2_SW_P10_P11,UART2_SW_P40_P42
	UART3_SW(UART3_SW_P00_P01);		//UART3_SW_P00_P01,UART3_SW_P50_P51
	UART4_SW(UART4_SW_P02_P03);		//UART4_SW_P02_P03,UART4_SW_P52_P53
//	I2C_SW(I2C_P14_P15);					//I2C_P14_P15,I2C_P24_P25,I2C_P76_P77,I2C_P33_P32
//	COMP_SW(CMP_OUT_P34);					//CMP_OUT_P34,CMP_OUT_P41
	SPI_SW(SPI_P22_P23_P24_P25);	//SPI_P12_P13_P14_P15,SPI_P22_P23_P24_P25,SPI_P74_P75_P76_P77,SPI_P35_P34_P33_P32

	LCM_CTRL_SW(LCM_CTRL_P41_P37_P36);	//LCM_CTRL_P41_P44_P43,LCM_CTRL_P41_P37_P36,LCM_CTRL_P41_P42_P40,LCM_CTRL_P40_P37_P36
	LCM_DATA_SW(LCM_D8_NA_P2);		//LCM_D8_NA_P2,LCM_D8_NA_P6 / LCM_D16_P2_P0,LCM_D16_P6_P2,LCM_D16_P2_P7,LCM_D16_P6_P7

	PCA_SW(PCA_P12_P17_P16_P15_P14);	//PCA_P12_P17_P16_P15_P14,PCA_P22_P23_P24_P25_P26,PCA_P74_P70_P71_P72_P73,PCA_P35_P33_P32_P31_P30
	
	PWM15_PWM0_SW(PWM15_SW_P2);		//PWM15_SW_P2,PWM15_SW_P1,PWM15_SW_P6
}

//========================================================================
//                                ϵͳ��ʼ��
//========================================================================
void SYS_Init(void)
{
	extern uint32 Get_SysClk_FRE(void);
    /* Get system clock frequency */
    MAIN_Fosc = Get_SysClk_FRE();
	
	GPIO_config();
	Init_LED();
	
	InitSegLCD();
	LCD.ShowAll();
	
	InitBeep();
}

