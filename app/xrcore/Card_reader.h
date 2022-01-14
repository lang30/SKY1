#ifndef __CARD_READER_H__
#define __CARD_READER_H__

#include "config.h"

typedef struct SReader idata TReader;

// FuncRead 读卡函数
typedef error (*FuncCard)(pbyte ACardSN);
typedef error (*FuncCmd)();
typedef error (*FuncPrepaySmall)(uint16 Amount, byte IsFirst);

typedef struct SPurse {
    byte EmbedSN[4];
    word CardType;      // 卡类型
    word AuthCode;      // 授权代码
    uint16 Num;         // 卡编号
    uint32 Balance;     // 卡余额
    uint16 TradeSerial; // 交易序号
} TPurse;

// SReader 读卡器
struct SReader {
    // { 用户钱包 }
    TPurse Purse;

    // { 发现卡片 }
    FuncCard Find;

    // { 读取身份（含验证密钥） }
    FuncCard ReadNum;
    // { 读取余额 }
    FuncCmd ReadMoney;
    // { 小额预扣款 }
    FuncPrepaySmall PrepaySmall;

    // { 等待卡移走操作结束 }
    Proc WaitOver;

    Proc Halt;

    Proc RFCheck;
};

// Reader 读卡器模块
extern TReader Reader;

// GCardOn 是否卡片在读卡器上
extern bool data GCardOn;
// GOperOn 是否卡片为管理卡
extern bool data GOperOn;
extern byte GNumBlock[16];

TReader *NewReader();

#endif