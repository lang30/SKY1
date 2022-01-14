#include "card_reader.h"
#include "beep.h"
#include "RC522/STC8A_RC522.h"
#include "HT1621/STC8A_HT1621.h"
#include "seglcd.h"
#include "wcm_param.h"
#include "reader_cpu.h"
#include "xrcard.h"
#include <string.h>
#include "STC8A_Delay.h"

#define DefaultBlock \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

volatile TReader Reader;

volatile bool data GCardOn = false;
volatile bool data GOperOn = false;
volatile bool data Factory = false;
volatile byte GNumBlock[16] = DefaultBlock;

extern byte GDataType;

// GetCardArea 通过扇区，扇区块号获得卡区号
byte GetCardArea(byte sector, byte block) {
    return 4 * sector + block;
}

const CardTypeM1 = 0x0400;

// { 1. 寻卡判断卡类型 }
static error FindCard(pbyte CardSN) {
    // var
    static byte oTypeBuf[2] = {0, 0};
    static byte oCode;
    static word oType;
    u16 CardType_Buf;
    static unsigned char recive_data[2];

    // if (GCardOn) 
    if (Factory) {
        oCode = PICC_REQIDL;
    } else {
        oCode = PICC_REQALL;
    }

    if (PcdRequest(oCode, oTypeBuf) != SUCCESS) {
        return 1;
    }
    oType = (oTypeBuf[0] << 8) + oTypeBuf[1];

    switch (oType) {
    case 0x0200: // Mifare_One(S70)
        // 暂不支持
        return 301;
    case 0x0400: // Mifare_One(S50)
        GCPUCard = false;
        break;
    case 0x0800: // CPU(Pro_X_Sup)
        GCPUCard = true;
        break;
    default:
        return 301;
    }

    delay_ms(20);

    Reader.Purse.CardType = oType;

    if (PcdAnticoll(CardSN) != SUCCESS) {
        return 302;
    }

    if (PcdSelect(CardSN) != SUCCESS) {
        return 303;
    }
    
    if(CardSN[0] == 0x1C && CardSN[1] == 0x2E && CardSN[2] == 0x54 && CardSN[3] == 0x6A )
    // if(CardSN[0] == 0xB2 && CardSN[1] == 0x33 && CardSN[2] == 0xBF && CardSN[3] == 0x3E )
    {
        Ht1621Clear(0, 32);
        Beep.BeepOnce();
        delay_ms(500);
        
        SetFactory();
        Ht1621WrOneDat(17, 0x01);
        Ht1621WrOneDat(9, 0x08);
        Ht1621WrOneDat(7, 0x08);
        Ht1621WrOneDat(5, 0x08);
        Ht1621WrOneDat(3, 0x08);
        Ht1621WrOneDat(1, 0x08);
        Beep.BeepTime(1200);
        PcdHalt(); 
        Factory = true;
        return 1;
    }
    Factory = false;
    
    if(PosParam.M1Purse.DataType == 1)
    {    
        PcdHalt();    
        SetBitMask(BitFramingReg,0x07);  
        recive_data[0]=0x40;
        PcdComMF522 (PCD_TRANSCEIVE, recive_data, 1, recive_data, &CardType_Buf);
        if((recive_data[0]&0xf)==0xA)
        {
            PcdReset(); 
            Ht1621Clear(0, 32);
            Ht1621WrOneDat(9,0x06);Ht1621WrOneDat(10,0x0D);
            Ht1621_DisplayNum(2, 0, 1);
            Ht1621_DisplayNum(3, 0, 'd');
            Ht1621WrOneDat(17, 0x08);

            Beep.BeepGap(3);
            delay_ms(2000);

            return 1;
        }
        PcdRequest(0x52, oTypeBuf);
        PcdAnticoll(CardSN);							
        PcdSelect(CardSN);
    }		

    if (oType == 0x0400) {
        return 0;
    }

    // 发送 RATS
    if (CardReset(CPUReadBuf, oTypeBuf) != SUCCESS) {
        return 304;
    }

    // 选取文件
    if (Pcd_Cmd(CmdSelectFile, 7, CPUReadBuf, oTypeBuf) != SUCCESS) {
        PcdReset();
        return 305;
    }

    if ((CPUReadBuf[oTypeBuf[0]] << 8 | CPUReadBuf[oTypeBuf[0] + 1]) != 0x9000) {
        PcdReset();
        return 305;
    }

    return 0;
}

static void HaltCard() {
    PcdHalt();
}

// WatiOver 等待卡片移走，操作结束
static void WaitOver() {

    if (!GCardOn) {
        if (GCPUCard) {
            PcdReset();
            return;
        }
        HaltCard();
        return;
    }

    while (1) {
        delay_ms(50);
        if (GCPUCard) {
            if (CmdReadCPURand() != nil) {
                break;
            }
            continue;
        }

        if (GCardOn) {
            HaltCard();
        }

        if (FindCard(Reader.Purse.EmbedSN) != nil) {
            break;
        }
    }
    if (GCPUCard) {
        PcdReset();
        return;
    }
    HaltCard();
}

const byte M1AuthModeKeyA = 0x60;

static error DoAuthM1(pbyte ACardSN) {
    return PcdAuthState(M1AuthModeKeyA, GetCardArea(PosParam.M1Purse.Sector, 3), PosParam.M1Purse.Key, ACardSN);
}

// ReadNum 读取卡片（M1卡）
static error ReadNum(pbyte ACardSN) {
    // var
    static error err;

    // 验证扇区密钥
    if (GCPUCard) {
        err = DoAuthCPU(ACardSN);
        if (err != nil) {
            return err;
        }
    } else {
        err = DoAuthM1(ACardSN);
        if (err != nil) {
            return 401;
        }
    }

    // 读取身份块 1
    if (GCPUCard) {
        err = CmdReadCPUFile(0x86, &GNumBlock, 0x10);
    } else {
        err = PcdRead(GetCardArea(PosParam.M1Purse.Sector, 1), &GNumBlock);
    }
    if (err != nil) {
        return 501;
    }


    // 72 01 60 04 ff bb e0 00 00 03 fc ff ff 00 00 95
    err = DecodeNumBlock(ACardSN, &GNumBlock);
    if (err != nil) {
        return 502;
    }

    memmove(Reader.Purse.EmbedSN, ACardSN, 4);
    memmove(&Reader.Purse.AuthCode, &GNumBlock[2], 2);

    GOperOn = (GNumBlock[5] != 0xBB);
    if (GOperOn) {
        return 0;
    }

    return 0;
}

// ReadMoney 读取卡片金额
static error ReadMoney() {
    // var
    error err, err2;
    static byte oBlock0[16] = DefaultBlock;
    static byte oBlock2[16] = DefaultBlock;
    static uint16 oSerial0, oSerial2;
    static uint32 oBalance0, oBalance2;

    oSerial0 = 0;
    oSerial2 = 0;
    oBalance0 = 0;
    oBalance2 = 0;

    // 读取金额块 0
    if (GCPUCard) {
        err = CmdReadCPUFile(0x88, &oBlock0, 0x10);
    } else {
        err = PcdRead(GetCardArea(PosParam.M1Purse.Sector, 0), &oBlock0);
    }
    if (err != nil) {
        return 503;
    }

    // 读取金额块 2
    if (GCPUCard) {
        err = CmdReadCPUFile(0x87, &oBlock2, 0x10);
    } else {
        err = PcdRead(GetCardArea(PosParam.M1Purse.Sector, 2), &oBlock2);
    }
    if (err != nil) {
        return 504;
    }

    // 84 37 10 27 00 c8 00 00 00 00 00 00 00 00 00 5b
    err = DecodeMoneyBlock(Reader.Purse.EmbedSN, &oBlock0, &oBalance0, &oSerial0);
    err2 = DecodeMoneyBlock(Reader.Purse.EmbedSN, &oBlock2, &oBalance2, &oSerial2);

    if ((err != nil) && (err2 != nil)) {
        return 505;
    }
    if ((err != nil) ||
        ((err == nil) && (err2 == nil) && (oBalance2 < oBalance0))) {
        Reader.Purse.Balance = oBalance2;
        Reader.Purse.TradeSerial = oSerial2;
        return nil;
    }

    Reader.Purse.Balance = oBalance0;
    Reader.Purse.TradeSerial = oSerial0;

    return nil;
}

// PrepaySmall 小额预付
error PrepaySmall(uint16 Amount, byte IsFirst) {

    static uint32 oBalance;
    static byte oBlock[16] = DefaultBlock;
    static error err = 0;

    if (Reader.Purse.Balance < (uint32)Amount) {
        // return 1;
        LCD.WriteDataD(17, 0x02);  // 停
        LCD.WriteBalance(&Reader.Purse.Balance);
        LCD.WriteDataD(11, 0x01 + 0x08); // 计时 + 元
        LCD.WriteDataD(17, 0x08); // 错误
        delay_ms(200);
        return 1;
    }

    oBalance = Reader.Purse.Balance - (uint32)Amount;
    

    if (IsFirst != 0) {
        Reader.Purse.TradeSerial++;
    }

    EncodeMoneyBlock(Reader.Purse.EmbedSN, oBlock, &oBalance, &Reader.Purse.TradeSerial);

    if (GCPUCard) {
        err = CmdWriteCPUBlock(0x88, oBlock);
        if (err != nil) {
            return 506;
        }
        err = CmdWriteCPUBlock(0x87, oBlock);
        if (err != nil) {
            return 507;
        }
    } else {
        err = PcdWrite(GetCardArea(PosParam.M1Purse.Sector, 0), oBlock);
        if (err != nil) {
            return 506;
        }

        err = PcdWrite(GetCardArea(PosParam.M1Purse.Sector, 2), oBlock);
        if (err != nil) {
            return 507;
        }
    }

    Reader.Purse.Balance = oBalance;

    return 0;
}
static void RFCheck()
{
    if(ReadRawRC(CommandReg) != 0x20){
        delay_ms(10);
        if(ReadRawRC(CommandReg) != 0x20){    
            LCD.ShowError(101);
        }
    }
}

TPurse NewPurse() {
    TPurse T;
    T.CardType = 0x0400;
    memset(T.EmbedSN, 0, 4);
    T.TradeSerial = 0;
    T.Balance = 0;
    return T;
}

TReader *NewReader() {

    Reader.Find = FindCard;  
    Reader.Halt = HaltCard;


    Reader.ReadNum = ReadNum;
    Reader.ReadMoney = ReadMoney;

    Reader.PrepaySmall = PrepaySmall;

    Reader.WaitOver = WaitOver;

    Reader.Purse = NewPurse();

    Reader.RFCheck = RFCheck;   

    memset(CPUReadBuf, 0, 64);

    return &Reader;
}