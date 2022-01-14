#include "config.h"
#include "STC8A_Delay.h"
#include "oper.h"
#include "beep.h"
#include "seglcd.h"
#include "card_reader.h"
#include "wcm_param.h"
#include "ht1621/STC8A_HT1621.h"
#include "pos_timer.h"
#include "pos_io.h"

#include <string.h>


extern volatile byte DefendUID;

void DoSetMeatureMode() {

    LCD.ShowOver();

    if ((GNumBlock[7] == 0x00) || (GNumBlock[7] == 0x20) || (GNumBlock[7] == 0x60)) {
        PosParam.MeasureMode = 1;
        LCD.WriteDataD(12, 0x01); // 计量
    } else {
        PosParam.MeasureMode = 0;
        LCD.WriteDataD(11, 0x01); // 计时
    }
    LCD.WriteCardProper(0xF1);
    Ht1621_DisplayNum(3, 0, GNumBlock[7]/16);//显示设置的模式
    Ht1621_DisplayNum(4, 0, GNumBlock[7]%16); 

    PosParam.SaveWorkParam();
    Beep.BeepOnce();

    Reader.WaitOver();
    LCD.WriteDataD(11, 0x00);
    LCD.WriteDataD(12, 0x00);
}

// 2021-10-14 12:37:00
// 43 01 60 04  ff 9c ff 21  10 14 ba 12  37 ff ff
void DoSetClock() {
    TClock c;
    c = LCD.GetClock();
    c.Year = 0x2100 + GNumBlock[7];
    c.Month = GNumBlock[8];
    c.Day = GNumBlock[9];

    c.Hour = GNumBlock[11];
    c.Minute = GNumBlock[12];
    c.Second = 0;

    LCD.SetClock(&c);
}

void LCDFlicker(ProcUint16 LCDshow,uint32 Value){
    LCD.Clear();
    delay_ms(300);
    LCDshow(Value);
    delay_ms(300);
    LCD.Clear();
    delay_ms(300);
    LCDshow(Value);
}

void DoConsumePlusMinus(bit aPlus)
{
    static uint32 data Pulse;
    static uint32 data TimeMSCounted_count;

    Pulse = PosParam.OutPulse;
    LCD.WriteUint16(Pulse);
    Beep.BeepOnce();

    if(aPlus) {              
        if(Pulse == 0xFFFF){                    
            LCDFlicker(LCD.WriteUint16,Pulse);
            return;
        }
        Pulse++;
    }
    else{
        if(Pulse == 0){                    
            LCDFlicker(LCD.WriteUint16,Pulse);
            return; 
        } 
        Pulse--;
    }

    delay_ms(200);
    LCD.WriteUint16(Pulse);

    SetTimeCounter(on);
    while (1)
    {
        if(TimeMSCounted >= 3000)
        {
            if(TimeMSCounted >= 3160 && TimeMSCounted != TimeMSCounted_count)   
            {
                if(aPlus) {
                    if(Pulse == 0xFFFF){                    
                        LCDFlicker(LCD.WriteUint16,Pulse);
                        break;
                    }
                    Pulse++;
                }
                else{ 
                    if(Pulse == 0){                    
                        LCDFlicker(LCD.WriteUint16,Pulse);
                        break;
                    }
                    Pulse--;
                }
                Beep.BeepTime(20);
                TimeMSCounted = 3000;
            }
        }
        LCD.WriteUint16(Pulse);

        if (GCardOn) {
            Reader.Halt();
        }
        if (Reader.Find(Reader.Purse.EmbedSN) != nil) {
            break;
        }
    }
    SetTimeCounter(off);  
    PosParam.OutPulse = Pulse;
}

byte setDefaultKey[6];
// DoOperCardTask 进入功能卡处理流程
void DoOperCardTask() {
    // var
    static byte oCode = 0;

    oCode = GNumBlock[5];

    // if ((oCode != 0xD5) && (PosParam.AuthCode != Reader.Purse.AuthCode)) {
    //     LCD.ShowAuthCode(Reader.Purse.AuthCode);
    //     LCD.WriteCardProper(oCode);
    //     Reader.WaitOver();
    //     return;
    // }

    LCD.Clear();
    LCD.WriteCardProper(oCode);

    switch (oCode) {
        case 0x01:
            break;

        // 费率设置卡（计量/计时）
        // c2 01 60 04 ff 22 79 [DayLimit ff ff] [OutPulse 01] fd | [OutFee 01] ff ff ff 03 6c b2
        case 0x22:
            if ((PosParam.AuthCode != Reader.Purse.AuthCode)) {  //验证授权号
                LCD.ShowAuthCode(Reader.Purse.AuthCode);
                LCD.WriteCardProper(oCode);
                Reader.WaitOver();
                break;
            }
            PosParam.DayLimit = ((uint32)GNumBlock[7] << 8) + (uint32)GNumBlock[8];
            PosParam.OutPulse = GNumBlock[9];
            PosParam.OutFee = GNumBlock[11];
            PosParam.SaveWorkParam();
            Beep.BeepOnce();
            LCD.WriteUint16(PosParam.OutPulse);
            delay_ms(1000);
            LCD.WriteUint16(PosParam.OutFee);
            delay_ms(1000);
            LCD.ShowOver();
            break;

        case 0x24: // 起步消费设置卡
            break;

        case 0x91: // 费率加减微调设置卡
            DoConsumePlusMinus(1);
            break;

        case 0x90: // 费率加减微调设置卡
            DoConsumePlusMinus(0);
            break;

        case 0x9A: // 查询卡
            break;

        case 0x9C: // 时钟设置卡
            DoSetClock();
            LCD.ShowOver();
            Beep.BeepOnce();
            break;

        case 0xD5: // 授权（机器号）设置卡
            memmove(&PosParam.AuthCode, &GNumBlock[2], 2);
            memmove(PosParam.PosIP, &GNumBlock[8], 2);
            PosParam.SaveBaseParam();
            //        LCD.ShowAuthCode(PosParam.AuthCode);
            //        LCD.WriteDataD(17, 0x01);
            LCD.ShowOver();
            Beep.BeepOnce();
            break;

        case 0xD6: // 防复制启停卡
            Beep.BeepOnce();
            LCD.WriteCardProper(oCode);
            LCD.ShowOver();
            delay_ms(500);
            if(GNumBlock[8] == 0x5A)
            {
                PosParam.M1Purse.DataType = 1;
            }
            PosParam.SaveBaseParam(); 
            break;

        case 0xF1: // 计费模式设置卡（00 低频 20 普通计量 60 精度计量 统一为计量模式）
            DoSetMeatureMode();
            return;

        case 0xF2: // 钱包设置卡（修改设备密钥、包号）
            Beep.BeepGap(1);
            LCD.WriteCardProper(oCode);
            LCD.ShowAuthCode(Reader.Purse.AuthCode);
            delay_ms(500);
            PosParam.M1Purse.Sector = GNumBlock[2];
            setDefaultKey[0] = GNumBlock[7];  setDefaultKey[1] = GNumBlock[8]; setDefaultKey[2] = GNumBlock[9];
            setDefaultKey[3] = GNumBlock[11];  setDefaultKey[4] = GNumBlock[12]; setDefaultKey[5] = GNumBlock[13];
            memmove(PosParam.M1Purse.Key, setDefaultKey, 6);
            PosParam.SaveBaseParam();
            break;

        case 0xF3: // 免费用量
            Beep.BeepGap(1);
            LCD.WriteCardProper(oCode);
            LCD.ShowOver();
            PosParam.FreeUse[0] = GNumBlock[11];
            PosParam.FreeUse[1] = GNumBlock[7];
            PosParam.FreeUse[2] = GNumBlock[8];
            PosParam.FreeUse[3] = GNumBlock[9];
            PosParam.SaveWorkParam();
            break;

        case 0xF6: // 双路费率设置卡
            break;

        default:
            LCD.ShowError(404);
            Reader.WaitOver();
            return;
    }

    LCD.WriteCardProper(oCode);
    Reader.WaitOver();
}