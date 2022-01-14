#include "beep.h"
#include "STC8A_Delay.h"
#include "STC8A_GPIO.h"

sbit PBeep = P2 ^ 7; // P27 控制蜂鸣器
sbit PLED  =  P6 ^ 0;  // P60 控制LED背光

#define BeepOn PBeep = 1
#define BeepOff PBeep = 0
#define LedSwitch PLED = !PLED

TBeep Beep;

// DoBeepOnce 蜂鸣器响一声
void DoBeepOnce() small {
    BeepOn;
    delay_ms(80);
    BeepOff;
}

// DoBeepOnce 蜂鸣器响一声
void DoBeepTime(int ms) small{
    BeepOn;
    delay_ms(ms);
    BeepOff;
}

// DoBeepTwinkle 蜂鸣器响 | 屏幕闪
void DoBeepTwinkle() small {
    LedSwitch;
    DoBeepOnce();
    LedSwitch;
}

// DoBeepGap 蜂鸣器间隔响  1时 可能会不响
void DoBeepGap(int ATimes) small {
    while (ATimes > 0) {
        ATimes -= 1;
        DoBeepOnce();
        delay_ms(80);
    }
}

// InitHelper 初始化 Helper
void InitBeep() 
{
    Beep.BeepGap = DoBeepGap;
    Beep.BeepOnce = DoBeepOnce;
    Beep.BeepTime = DoBeepTime;

    DoBeepGap(2);
}