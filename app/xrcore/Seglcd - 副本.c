#include "seglcd.h"
#include "STC8A_I2C.h"
#include "WCM_param.h"
#include "STC8A_Delay.h"
#include "beep.h"

// { HT1621 }
#define ClearAll Ht1621Clear(0, 32)

//#define WriteModeTime SegLCD_WriteData(11, 0x01) // 计时
//#define WriteFull SegLCD_WriteData(11, 0x02)     // 满
//#define WriteFree SegLCD_WriteData(11, 0x04)     // 免费用量
//#define WriteYuan SegLCD_WriteData(11, 0x08)     // 元

//#define WriteModePulse SegLCD_WriteData(12, 0x01) // 计量
//#define WriteLimit SegLCD_WriteData(12, 0x02)     // 限量
//#define WriteNo SegLCD_WriteData(12, 0x04)        // 不
//#define WriteL SegLCD_WriteData(12, 0x08)         // 升

//#define WriteOK SegLCD_WriteData(17, 0x01)    // OK
//#define WriteStop SegLCD_WriteData(17, 0x02)  // 停
//#define WriteLine SegLCD_WriteData(17, 0x04)  // ----
//#define WriteError SegLCD_WriteData(17, 0x08) // 错误
//#define WriteOKLine SegLCD_WriteData(17, 0x01 + 0x04)

//#define EraseLine SegLCD_WriteData(17, 0x00)

//#define WriteWarn SegLCD_WriteData(18, 0x01)   // !!!
//#define WriteTotal SegLCD_WriteData(18, 0x04)  // 合计
//#define WriteAmount SegLCD_WriteData(18, 0x08) // 金额


//#define ShowColon SegLCD_WriteData(18,0x02)    //显示 : Colon 
//#define HideColon SegLCD_WriteData(18,0x00)  
// { - (显示日期用) }
//#define WriteStrike      
//    SegLCD_WriteData(18, 0x00); 
//    SegLCD_WriteData(5, 0x0);   
//    SegLCD_WriteData(6, 0x02)

TLCD LCD;


// ClearNum 清除一个数字
//void ClearNum(u8 seat) small
//{
//    u8 seg1, seg2;
//    switch (seat) {
//    case 1: seg1 = 10; seg2 = 9;  
//        break;
//    case 2: seg1 = 8;  seg2 = 7;
//        break;
//    case 3: seg1 = 6;  seg2 = 5;
//        break;
//    case 4: seg1 = 4;  seg2 = 3;
//        break;
//    case 5: seg1 = 2;  seg2 = 1;
//        break;
//    case 6: seg1 = 14; seg2 = 13;
//        break;
//    case 7: seg1 = 16; seg2 = 15;
//        break;
//    }
//    SegLCD_WriteData(seg1, 0);
//    SegLCD_WriteData(seg2, 0);
//}

// ..... 显示结束帧
//void ShowOverFrame()
//{
//    ClearAll;
//    SegLCD_WriteData(9, 0x08);
//    SegLCD_WriteData(7, 0x08);
//    SegLCD_WriteData(5, 0x08);
//    SegLCD_WriteData(3, 0x08);
//    SegLCD_WriteData(1, 0x08);
//}

// Light 是否开启背光
//static void SetLight(byte AOn)
//{
//    if (P60 != AOn) {
//        P60 = AOn;
//    }
//}

// ShowVersion 显示版本信息
//static void ShowVersion(uint32_t num)
//{
//	SetLight(on);
//    // 显示 -
//    SegLCD_WriteData ( 9, 0x0 );
//    SegLCD_WriteData ( 10, 0x02 );

//    SegLCD_WriteNum ( 2, 0, num/100000%10 );
//    SegLCD_WriteNum ( 3, 0, num/10000%10 );
//    SegLCD_WriteNum ( 4, 0, num/1000%10 );
//    SegLCD_WriteNum ( 5, 1, num/100%10 );

//    SegLCD_WriteNum ( 6, 0, num/10%10 );
//    SegLCD_WriteNum ( 7, 0, num%10 );
//    // WriteOKLine;
//}

//static TClock DefaultClock() small 
//{
//    TClock c;
//    c.Year = 0x2006;
//    c.Month = 0x01;
//    c.Day = 0x02;
//    c.Hour = 0x13;
//    c.Minute = 0x04;
//    c.Second = 0x05;
//    return c;
//}

// PCF8563 02h: 秒 分 时 日 星期(不用) 月 年
//volatile byte ClockBuf[7] = {0x05, 0x04, 0x13, 0x02, 0x01, 0x01, 0x06};

// SetClock 设置新时钟（保存到存储器）
//void SetClock(TClock *c) 
//{
//    ClockBuf[6] = (byte)((*c).Year % 0x100);
//    ClockBuf[5] = (*c).Month;
//    ClockBuf[3] = (*c).Day;
//    ClockBuf[2] = (*c).Hour;
//    ClockBuf[1] = (*c).Minute;
//    ClockBuf[0] = (*c).Second;
//    I2C_WriteNbyte(0x02, ClockBuf, 7);
//}

//static TClock GetClock() 
//{
//    static TClock c;

//    I2C_ReadNbyte(0x02, ClockBuf, 7);

//    if (ClockBuf[6] == 0x00) {
//        c = DefaultClock();
//        SetClock(&c);
//        return c;
//    }

//    c.Year = 0x2000 + ClockBuf[6];
//    c.Month = ClockBuf[5];
//    c.Day = ClockBuf[3];
//    c.Hour = ClockBuf[2];
//    c.Minute = ClockBuf[1];
//    c.Second = ClockBuf[0];
//    return c;
//}

// ShowTime 显示时间
//static void ShowTime() small {
    // var
//    static TClock c;

//    c = GetClock();

//    SegLCD_WriteData(5, 0x00);      //清除第3位
//    SegLCD_WriteData(6, 0x00);

//    SegLCD_WriteData(11, 0x0);
//    SegLCD_WriteData(12, 0x0); // 清合计、金额

//    SegLCD_WriteNum(1, 0, c.Hour / 0x10);
//    SegLCD_WriteNum(2, 0, c.Hour % 0x10);

//	ShowColon;  	          //显示“ ：	

//    SegLCD_WriteNum(4, 0, c.Minute / 0x10);
//    SegLCD_WriteNum(5, 0, c.Minute % 0x10);

//    SegLCD_WriteNum(6, 0, c.Second / 0x10);
//    SegLCD_WriteNum(7, 0, c.Second % 0x10);
//}
// ShowDate 显示日期
//static void ShowDate() small 
//{
//    // var
//    static TClock c;

//    c = GetClock();

//    SegLCD_WriteData(11, 0x0);
//    SegLCD_WriteData(18, 0x0); 

//    SegLCD_WriteNum(1, 0, c.Month / 0x10);
//    SegLCD_WriteNum(2, 0, c.Month % 0x10);

//    WriteStrike;    // 显示 -

//    SegLCD_WriteNum(4, 0, c.Day / 0x10);
//    SegLCD_WriteNum(5, 0, c.Day % 0x10);

//    SegLCD_WriteNum(6, 0, (c.Year-0x2000) / 0x10);
//    SegLCD_WriteNum(7, 0, (c.Year-0x2000) % 0x10);

//    SegLCD_WriteData(17, 0); // 清 -
//}


// ShowCardErr 显示卡错误
//static void ShowError(word ErrCode) {
//    
//    SegLCD_WriteNum(1, 0, 'e');
//    ClearNum(2);
//    SegLCD_WriteNum(3, 0, (ErrCode / 100) % 10);
//    SegLCD_WriteNum(4, 0, (ErrCode / 10) % 10);
//    SegLCD_WriteNum(5, 0, ErrCode % 10);
//    ClearNum(6);
//    ClearNum(7);
//    WriteError;
//    SegLCD_WriteData(11, 0);
//    SegLCD_WriteData(18, 0);
//    Beep.BeepGap(3);
//    delay_ms(200);
//}

// ShowAuthCode 显示授权编号
//static void ShowAuthCode(word AuthCode) {
//    ClearAll;
//    ClearNum(1);
//    SegLCD_WriteNum(2, 1, AuthCode / 0x1000);
//    SegLCD_WriteNum(3, 1, (AuthCode / 0x100) % 0x10);
//    SegLCD_WriteNum(4, 1, (AuthCode / 0x10) % 0x10);
//    SegLCD_WriteNum(5, 1, AuthCode % 0x10);
//}

// 0 识别卡片成功并且有余额，显示 OK
// 1 预扣金额成功显示 工作模式，清0计数显示，擦除 0
//static void ShowStep(byte ACode) {
//    if (ACode == 0) {
//        WriteOK;
//        return;
//    }

//    WriteLine;
//    SegLCD_WriteNum(6, 0, 0);
//    SegLCD_WriteNum(7, 0, 0);
//}

// WriteBalance 更新余额字段
//static void WriteBalance(uint32 *Balance) {
//    // var
//    static byte oHundreds, oTens, oUnits, oPoint1, oPoint2;
//    static byte oDot1, oDot2;

//    // 100.00 / 1000.0
//    if (*Balance > 100000) {
//        oDot1 = 0;
//        oDot2 = 1;
//        oHundreds = (*Balance / 100000) % 10;
//        oTens = (*Balance / 10000) % 10;
//        oUnits = (*Balance / 1000) % 10;
//        oPoint1 = (*Balance / 100) % 10;
//        oPoint2 = (*Balance / 10) % 10;
//    } else {
//        oDot1 = 1;
//        oDot2 = 0;
//        oHundreds = (*Balance / 10000) % 10;
//        oTens = (*Balance / 1000) % 10;
//        oUnits = (*Balance / 100) % 10;
//        oPoint1 = (*Balance / 10) % 10;
//        oPoint2 = *Balance % 10;
//    }

//    if (oHundreds > 0) {
//        SegLCD_WriteNum(1, 0, oHundreds);
//        SegLCD_WriteNum(2, 0, oTens);
//    } else {
//        ClearNum(1);
//        if (oTens > 0) {
//            SegLCD_WriteNum(2, 0, oTens);
//        } else {
//            ClearNum(2);
//        }
//    }

//    SegLCD_WriteNum(3, oDot1, oUnits);
//    SegLCD_WriteNum(4, oDot2, oPoint1);
//    SegLCD_WriteNum(5, 0, oPoint2);
//    SegLCD_WriteData(18, 0x08);
//}

// WriteUserPulse 更新计脉冲字段
//static void WriteUserPulse(pUint32 ACounted) {

//    byte oValue;
//    oValue = *ACounted % 100;

//    SegLCD_WriteNum(6, 0, oValue / 10);
//    SegLCD_WriteNum(7, 0, oValue % 10);
//}

// WriteUserTime 更新计时字段
//static void WriteUserTime(byte ASec) {
//    SegLCD_WriteNum(6, 0, (ASec / 10) % 10);
//    SegLCD_WriteNum(7, 0, ASec % 10);
//}

// WriteCardProper 写功能卡属性
//static void WriteCardProper(byte AProper) {
//    SegLCD_WriteNum(6, 0, (AProper / 0x10) % 0x10);
//    SegLCD_WriteNum(7, 0, AProper % 0x10);
//}

//static void WriteFlagWork(byte AMode) {
//    if (AMode == 0) {
//        SegLCD_WriteData(11, 0x01 + 0x08); // 计时 / 元
//    } else {
//        WriteModePulse; // 计量
//    }
//}

// ShowBalance 显示卡余额
//static void ShowBalance(uint32 Balance) {

//    WriteAmount;

//    WriteBalance(&Balance);

//    WriteYuan;
//    SegLCD_WriteData(17, 0);
//    ClearNum(6);
//    ClearNum(7);
//}

// ShowTimeCounted 显示计时器计数 MM:ss zz
//static void ShowTimeCounted(uint32 AMSCounted) {

//    byte oMin = (AMSCounted / 1000) / 60;
//    byte oSec = (AMSCounted / 1000) % 60;
//    byte oMs = AMSCounted % 1000;

//    SegLCD_WriteNum(1, 0, oMin / 10);
//    SegLCD_WriteNum(2, 0, oMin % 10);
//    ShowColon;
//    SegLCD_WriteNum(4, 0, oSec / 10);
//    SegLCD_WriteNum(5, 0, oSec % 10);

//    SegLCD_WriteNum(6, 0, oMs / 100);
//    SegLCD_WriteNum(7, 0, (oMs % 100) / 10);

//    WriteModeTime;
//}

// ShowSecCounted 显示计时器(显示毫秒)计数 MM:ss zz
//static void ShowSecCounted(uint32 AMSCounted) {
//    byte  oMin = AMSCounted / 3600;
//    byte oSec = (AMSCounted % 3600) / 60;
//    byte oMs = (AMSCounted % 3600)%60;

//    SegLCD_WriteNum(1, 0, oMin / 10);
//    SegLCD_WriteNum(2, 0, oMin % 10);
//    ShowColon;
//    SegLCD_WriteNum(4, 0, oSec / 10);
//    SegLCD_WriteNum(5, 0, oSec % 10);

//    SegLCD_WriteNum(6, 0, oMs / 10);
//    SegLCD_WriteNum(7, 0, oMs % 10);

//    WriteModeTime;
//}

// ShowPulseCounted 显示累计脉冲数
//static void ShowPulseCounted(uint32 ACounted) {

//    byte oMillions = (ACounted / 1000000) % 10;
//    byte oHundredThousands = (ACounted / 100000) % 10;
//    byte oTenThousands = (ACounted / 10000) % 10;
//    byte oThousands = (ACounted / 1000) % 10;
//    byte oHundreds = (ACounted / 100) % 10;
//    byte oTens = (ACounted / 10) % 10;
//    byte oUnits = ACounted % 10;

//    SegLCD_WriteNum(1, 0, oMillions);
//    SegLCD_WriteNum(2, 0, oHundredThousands);
//    SegLCD_WriteNum(3, 0, oTenThousands);
//    SegLCD_WriteNum(4, 0, oThousands);
//    SegLCD_WriteNum(5, 0, oHundreds);
//    SegLCD_WriteNum(6, 0, oTens);
//    SegLCD_WriteNum(7, 0, oUnits);

//    WriteModePulse;
//}

// WriteValue 显示数值字段
//static void WriteUint16(uint16 AValue) {

//    byte oTenThousands = (AValue / 10000) % 10;
//    byte oThousands = (AValue / 1000) % 10;
//    byte oHundreds = (AValue / 100) % 10;
//    byte oTens = (AValue / 10) % 10;
//    byte oUnits = AValue % 10;

//    SegLCD_WriteNum(1, 0, oTenThousands);
//    SegLCD_WriteNum(2, 0, oThousands);
//    SegLCD_WriteNum(3, 0, oHundreds);
//    SegLCD_WriteNum(4, 0, oTens);
//    SegLCD_WriteNum(5, 0, oUnits);
//}

// Init 初始化屏幕
void InitSegLCD() 
{
    Ht1621_Init();
    Ht1621Clear(0, 32);
    
    CreateLCD();
    ClearAll;
}

// Clear 清屏
// static void Clear() {
//     ClearAll;
// }

// 全显测试
static void ShowAll() 
{
    Ht1621ShowAll();
}

//static void ShowSector()
//{
//    static byte sSector;
//    ClearAll;
//    sSector = PosParam.M1Purse.Sector;

//    Ht1621ShowDash(2);
//    SegLCD_WriteNum(3, 0, sSector/10);
//    SegLCD_WriteNum(4, 0, sSector%10);
//    Ht1621ShowDash(5);
//}

// CreateLCD 创建显示屏模块
void CreateLCD() {
//    LCD.Clear = Clear;
    LCD.ShowAll= ShowAll;

//    LCD.SetLight = SetLight;

//    LCD.ShowVersion = ShowVersion;
//    LCD.ShowSector = ShowSector;
//    LCD.ShowDate = ShowDate;
//    LCD.ShowTime = ShowTime;

//    LCD.ShowBalance = ShowBalance;

//    LCD.ShowTimeCounted = ShowTimeCounted;
//    LCD.ShowSecCounted = ShowSecCounted;
//    LCD.ShowPulseCounted = ShowPulseCounted;
//    LCD.ShowError = ShowError;
//    LCD.ShowAuthCode = ShowAuthCode;
//    LCD.ShowOver = ShowOverFrame;
//    LCD.ShowStep = ShowStep;
//    
//    LCD.WriteDataD = SegLCD_WriteData;
//    LCD.WriteUint16 = WriteUint16;
//    LCD.WriteBalance = WriteBalance;
//    LCD.WriteUserTime = WriteUserTime;
//    LCD.WriteUserPulse = WriteUserPulse;

//    LCD.WriteCardProper = WriteCardProper;

//    LCD.GetClock = GetClock;
//    LCD.SetClock = SetClock;
}