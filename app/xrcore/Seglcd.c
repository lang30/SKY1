#include "seglcd.h"
#include "STC8A_I2C.h"
#include "WCM_param.h"
#include "STC8A_Delay.h"
#include "beep.h"

// { HT1621 }
#define ClearAll Ht1621Clear(0, 32)

TLCD LCD;



// Init 初始化屏幕
void InitSegLCD() 
{
    Ht1621_Init();
    Ht1621Clear(0, 32);
    
    CreateLCD();
    ClearAll;
}

// 全显测试
static void ShowAll() 
{
    Ht1621ShowAll();
}


void CreateLCD() 
{
    LCD.ShowAll= ShowAll;
}