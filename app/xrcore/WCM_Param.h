#ifndef __POS_PARAM_H__
#define __POS_PARAM_H__

#include "config.h"

// CardPurseParam 钱包扇区参数
typedef struct SCardPurseParam {
    byte Sector;   // 扇区
    byte DataType; // 数据方案 0 | 1
    byte KeyType;  // 密钥类型 1 随机密钥 | 7 固定密钥
    byte Key[6];   // 密钥
} TCardPurseParam;

// TPosParam 设备参数
typedef struct SPosParam {
    
    // { 基本参数 }
    TCardPurseParam M1Purse;
    TCardPurseParam M1PurseSub;
    TCardPurseParam CPUPurse;
    
    word AuthCode;
    byte PosIP[2];
    
    // { 工作参数 }
    word PosType;      // 设备类型
    byte MeasureMode;  // 计量模式
    uint32 DayLimit;   // 日限额
    byte ControlMode;  // 控制模式 0:单路 1:双路
    

    word StopSecond;   // 停机时间
    word RefundSecond; // 退款时间
    
    word OutPulse;     // 单路脉冲
    word OutFee;       // 单路费用
    
    word Out1Pulse;    // 一路脉冲
    word Out1Fee;      // 一路费用
    word Out2Pulse;    // 二路脉冲
    word Out2Fee;      // 二路费用
    
    word BootSecond;   // 刷卡延迟扣款秒
    byte FreeUse[4];
    uint32 Amtall;     // 消费总额
    
    Proc SaveBaseParam;
    Proc SaveWorkParam;

    Proc ReadBaseParam;
    Proc ReadWorkParam;
 
} TPosParam;

extern TPosParam PosParam;
extern byte xdata CPUReadBuf[64];

void InitPosParam();
void SetFactory();

#endif
