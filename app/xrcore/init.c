#include "init.h"
#include "chip/stc/STC8A_Exti.h"
#include "chip/stc/STC8A_I2C.h"
#include "chip/stc/STC8A_Nvic.h"
#include "chip/stc/STC8A_SPI.h"
#include "chip/stc/STC8A_Switch.h"
#include "chip/stc/STC8A_Timer.h"
#include "helper.h"

#include "chip/rc522/STC8A_RC522.h"

// InitGPIO 初始化IO口
static void InitGPIO() {
    P0_MODE_IO_PU(GPIO_Pin_All);                         // P0 设置为准双向口
    P1_MODE_IN_HIZ(GPIO_Pin_LOW);                        // P1.0~P1.3 设置为高阻输入
    P1_MODE_IO_PU(GPIO_Pin_HIGH);                        // P1.4~P1.7 设置为准双向口
    P2_MODE_IO_PU(GPIO_Pin_All);                         // P2 设置为准双向口
    P3_MODE_IO_PU(GPIO_Pin_LOW);                         // P3.0~P3.3 设置为准双向口
    P3_MODE_IO_PU(GPIO_Pin_HIGH);                        // P3.4~P3.7 设置为准双向口
    P4_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_7); // P4.0,P4.6,P4.7 设置为准双向口
    P6_MODE_IO_PU(GPIO_Pin_All);                         // P6 设置为准双向口
    P7_MODE_IO_PU(GPIO_Pin_All);                         // P7 设置为准双向口

    // 端口使用前要设置，初始化；
    P2_MODE_OUT_PP(GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_7); // P2.0  LED | P2.6  电磁阀|P2.7  BEEP 设置为推挽输出
    P6_MODE_OUT_PP(GPIO_Pin_0);                           // P6.0  LCD背光 设置为推挽输出
                                                          // 初始化电平
    P20 = 1;
    P26 = 0;
    P27 = 0;
    P60 = 0;
    // Timer4 计数器 io口初始化
    P0_MODE_IN_HIZ(GPIO_Pin_6);    // P0.6 设置为高阻输入
    P0_PULL_UP_ENABLE(GPIO_Pin_6); // P0.6 开启内部上拉
}

// InitSPI 初始化总线
static void InitSPI(void) {
    SPI_InitTypeDef oParams;
    oParams.SPI_Enable = ENABLE;        // SPI启动    ENABLE, DISABLE
    oParams.SPI_SSIG = DISABLE;         // 片选位     ENABLE, DISABLE
    oParams.SPI_FirstBit = SPI_MSB;     // 移位方向   SPI_MSB, SPI_LSB
    oParams.SPI_Mode = SPI_Mode_Master; // 主从选择   SPI_Mode_Master, SPI_Mode_Slave
    oParams.SPI_CPOL = SPI_CPOL_High;   // 时钟相位   SPI_CPOL_High,   SPI_CPOL_Low
    oParams.SPI_CPHA = SPI_CPHA_2Edge;  // 数据边沿   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    oParams.SPI_Speed = SPI_Speed_4;    // SPI速度    SPI_Speed_4, SPI_Speed_16, SPI_Speed_64,
                                        // SPI_Speed_128
    SPI_Init(&oParams);
    // NVIC_SPI_Init(ENABLE,Priority_3);		//中断使能,
    // ENABLE/DISABLE; 优先级(低到高)
    // Priority_0,Priority_1,Priority_2,Priority_3 RC522
    // 开启NVIC_SPI_Init会有影响；
}

// InitTimer 初始化定时器
static void InitTimer() {
    // var
    TIM_InitTypeDef oParams;

    oParams.TIM_Mode = TIM_16BitAutoReload;            // 16 位自动重装载模式
    oParams.TIM_ClkOut = DISABLE;                      // 不输出高速脉冲
    oParams.TIM_ClkSource = TIM_CLOCK_1T;              // 1T 模式
    oParams.TIM_Value = 65536UL - (MAIN_Fosc / 1000L); // 定时 1ms

    // { 定时器 0 }
    //  oParams.TIM_Run = DISABLE;
    //  Timer_Inilize(Timer0, &oParams);
    //  NVIC_Timer0_Init(ENABLE, Priority_2);

    // 定时器3

    oParams.TIM_Run = DISABLE;
    Timer_Inilize(Timer3, &oParams);
    NVIC_Timer3_Init(ENABLE, Priority_2);

    // 定时器4 做用作计数器(对引脚T4/P0.6外部脉冲进行计数)。T4T3M->T4_C/T 置1

    oParams.TIM_ClkSource = TIM_CLOCK_Ext;
    oParams.TIM_ClkOut = DISABLE;
    oParams.TIM_Value = 0xffff; // 初值 ; 多少个脉冲产生中断， 这里设置1个脉冲产生中断；
    oParams.TIM_Run = DISABLE;
    Timer_Inilize(Timer4, &oParams);
    NVIC_Timer4_Init(ENABLE, Priority_2);
}

static void InitI2C() {
    I2C_InitTypeDef oParams;
    oParams.I2C_Mode = I2C_Mode_Master; // 主从选择   I2C_Mode_Master, I2C_Mode_Slave
    oParams.I2C_Enable = ENABLE;        // I2C功能使能
    oParams.I2C_MS_WDTA = DISABLE;      // 主机使能自动发送
    oParams.I2C_Speed = 16;             // 总线速度=Fosc/2/(Speed*2+4),      0~63
    I2C_Init(&oParams);
    NVIC_I2C_Init(I2C_Mode_Master, DISABLE, Priority_0); // 主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    I2C_SW(I2C_P14_P15);                                 // I2C_P14_P15,I2C_P24_P25,I2C_P33_P32}
}

// InitEXTI 初始化外部中断
static void InitEXTI(void) {

    EXTI_InitTypeDef oParams;

    oParams.EXTI_Mode = EXT_MODE_Fall; // 中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
    Ext_Inilize(EXT_INT0, &oParams);   // 初始化

    NVIC_INT0_Init(ENABLE, Priority_0); // 中断使能, ENABLE/DISABLE; 优先级(低到高)

    oParams.EXTI_Mode = EXT_MODE_Fall; // 中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
    Ext_Inilize(EXT_INT1, &oParams);   // 初始化

    NVIC_INT1_Init(ENABLE, Priority_0); // 中断使能, ENABLE/DISABLE; 优先级(低到高)
}

// InitSwitch 切换功能引脚
static void InitSwitch(void) {
    // UART1_SW(UART1_SW_P30_P31); // UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
    // UART2_SW(UART2_SW_P10_P11); // UART2_SW_P10_P11,UART2_SW_P40_P42
    // UART3_SW(UART3_SW_P00_P01); // UART3_SW_P00_P01,UART3_SW_P50_P51
    // UART4_SW(UART4_SW_P02_P03); // UART4_SW_P02_P03,UART4_SW_P52_P53

    //	I2C_SW(I2C_P14_P15);
    ////I2C_P14_P15,I2C_P24_P25,I2C_P76_P77,I2C_P33_P32
    /// COMP_SW(CMP_OUT_P34); /CMP_OUT_P34,CMP_OUT_P41

    SPI_SW(SPI_P22_P23_P24_P25); // SPI_P12_P13_P14_P15,SPI_P22_P23_P24_P25,SPI_P74_P75_P76_P77,SPI_P35_P34_P33_P32

    LCM_CTRL_SW(LCM_CTRL_P41_P37_P36); // LCM_CTRL_P41_P44_P43,LCM_CTRL_P41_P37_P36,LCM_CTRL_P41_P42_P40,LCM_CTRL_P40_P37_P36
    LCM_DATA_SW(LCM_D8_NA_P2);         // LCM_D8_NA_P2,LCM_D8_NA_P6 /
                                       // LCM_D16_P2_P0,LCM_D16_P6_P2,LCM_D16_P2_P7,LCM_D16_P6_P7

    PCA_SW(PCA_P12_P17_P16_P15_P14); // PCA_P12_P17_P16_P15_P14,PCA_P22_P23_P24_P25_P26,PCA_P74_P70_P71_P72_P73,PCA_P35_P33_P32_P31_P30

    PWM15_PWM0_SW(PWM15_SW_P2); // PWM15_SW_P2,PWM15_SW_P1,PWM15_SW_P6
}

static void InitLCD() {
    Ht1621_Init();
    Ht1621Clear(0xf, 32);
}

static void InitMCU() {

    InitGPIO();

    InitSPI();

    InitTimer();
    // InitEXTI();
    
    InitI2C();

    InitSwitch();

    RC522_Init();
    PcdSwitchPCB();

    InitLCD();

    EA = 1; // 开放所有中断
}

void InitChip() {
    InitMCU();
    InitHelper();
}