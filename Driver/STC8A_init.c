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
//                                IO口配置
//========================================================================
void	GPIO_config(void)
{
	P0_MODE_IO_PU(GPIO_Pin_All);		//P0 设置为准双向口
	P1_MODE_IN_HIZ(GPIO_Pin_LOW);		//P1.0~P1.3 设置为高阻输入
	P1_MODE_IO_PU(GPIO_Pin_HIGH);		//P1.4~P1.7 设置为准双向口
	P2_MODE_IO_PU(GPIO_Pin_All);		//P2 设置为准双向口
	P3_MODE_IO_PU(GPIO_Pin_LOW);		//P3.0~P3.3 设置为准双向口
	P3_MODE_IO_PU(GPIO_Pin_HIGH);		//P3.4~P3.7 设置为准双向口
	P4_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_7);	//P4.0,P4.6,P4.7 设置为准双向口
	P6_MODE_IO_PU(GPIO_Pin_All);		//P6 设置为准双向口
	P7_MODE_IO_PU(GPIO_Pin_All);		//P7 设置为准双向口
	
	
//端口使用前要设置，初始化；
	P2_MODE_OUT_PP(GPIO_Pin_0|GPIO_Pin_6|GPIO_Pin_7);			//P2.0  LED | P2.6  电磁阀|P2.7  BEEP 设置为推挽输出
	P6_MODE_OUT_PP(GPIO_Pin_0);			//P6.0  LCD背光 设置为推挽输出
//初始化电平
	P20 = 1;
	P26 = 0;
	P27 = 0;
	P60 = 0;
//Timer4 计数器 io口初始化
	P0_MODE_IN_HIZ(GPIO_Pin_6); 	//P0.6 设置为高阻输入
	P0_PULL_UP_ENABLE(GPIO_Pin_6);		//P0.6 开启内部上拉
}

//========================================================================
//                               定时器配置
//========================================================================
void Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;						//结构定义

	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000UL);		//中断频率, 1000次/秒
	TIM_InitStructure.TIM_Run       = ENABLE;					//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);					//初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer0_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

//	//定时器1做16位自动重装, 中断频率为20000HZ，中断函数从P6.6取反输出10KHZ方波信号.
//	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
//	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//指定时钟源, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
//	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
//	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 20000);			//中断频率, 20000次/秒
//	TIM_InitStructure.TIM_Run       = ENABLE;					//是否初始化后启动定时器, ENABLE或DISABLE
//	Timer_Inilize(Timer1,&TIM_InitStructure);					//初始化Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
//	NVIC_Timer1_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

//	//定时器2做16位自动重装, 中断频率为10000HZ，中断函数从P6.5取反输出5KHZ方波信号.
//	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
//	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
//	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 10000);				//初值
//	TIM_InitStructure.TIM_Run       = ENABLE;					//是否初始化后启动定时器, ENABLE或DISABLE
//	Timer_Inilize(Timer2,&TIM_InitStructure);					//初始化Timer2	  Timer0,Timer1,Timer2,Timer3,Timer4
//	NVIC_Timer2_Init(ENABLE,NULL);		//中断使能, ENABLE/DISABLE; 无优先级

	//定时器3做16位自动重装, 1毫秒产一次中断；
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;					//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0x9AA9;		     //初值, 9AA9 1毫秒产生中断
	TIM_InitStructure.TIM_Run       = DISABLE;					//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer3,&TIM_InitStructure);					//初始化Timer3	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer3_Init(ENABLE,NULL);		//中断使能, ENABLE/DISABLE; 无优先级

	//定时器4做用作计数器(对引脚T4/P0.6外部脉冲进行计数)。T4T3M->T4_C/T 置1
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_Ext;	        //指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;					//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0xffff;					//初值 ; 多少个脉冲产生中断， 这里设置1个脉冲产生中断；
	TIM_InitStructure.TIM_Run       = DISABLE;					//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer4,&TIM_InitStructure);					//初始化Timer4	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer4_Init(ENABLE,NULL);			//中断使能, ENABLE/DISABLE; 无优先级
	
}

//========================================================================
//                              ADC初始化
//========================================================================

//========================================================================
//                              串口初始化
//========================================================================
void UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//使用波特率, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

//	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率,     110 ~ 115200
//	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
//	UART_Configuration(UART2, &COMx_InitStructure);		//初始化串口2 USART1,USART2,USART3,USART4
//	NVIC_UART2_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

//========================================================================
//                               I2C初始化
//========================================================================
void I2C_config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;
	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//主从选择   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;						//I2C功能使能,   ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;					//主机使能自动发送,  ENABLE, DISABLE
	I2C_InitStructure.I2C_Speed     = 16;								//总线速度=Fosc/2/(Speed*2+4),      0~63
	I2C_Init(&I2C_InitStructure);
	NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_0);		//主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	I2C_SW(I2C_P14_P15);					//I2C_P14_P15,I2C_P24_P25,I2C_P33_P32}
}
//========================================================================
//                               SPI初始化
//========================================================================
void SPI_config(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	SPI_InitStructure.SPI_Enable    = ENABLE;						//SPI启动    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;						//片选位     ENABLE, DISABLE
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;					//移位方向   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;		//主从选择   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;			//时钟相位   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;		//数据边沿   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Speed     = SPI_Speed_4;			//SPI速度    SPI_Speed_4, SPI_Speed_16, SPI_Speed_64, SPI_Speed_128
	SPI_Init(&SPI_InitStructure);
	//NVIC_SPI_Init(ENABLE,Priority_3);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3  
																				//RC522   开启NVIC_SPI_Init会有影响；
}

//========================================================================
//                             比较器初始化
//========================================================================

//========================================================================
//                             外部中断初始化
//========================================================================
void Exti_config(void)
{
	EXTI_InitTypeDef	Exti_InitStructure;							//结构定义

	Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT0,&Exti_InitStructure);				//初始化
	NVIC_INT0_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT1,&Exti_InitStructure);				//初始化
	NVIC_INT1_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	NVIC_INT2_Init(ENABLE,NULL);		//中断使能, ENABLE/DISABLE; 无优先级
	NVIC_INT3_Init(ENABLE,NULL);		//中断使能, ENABLE/DISABLE; 无优先级
	NVIC_INT4_Init(ENABLE,NULL);		//中断使能, ENABLE/DISABLE; 无优先级
}

//========================================================================
//                             外部中断初始化
//========================================================================

//========================================================================
//                               功能脚切换
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
//                                系统初始化
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

