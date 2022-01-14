#ifndef __SEGLCD_H__
#define __SEGLCD_H__

#include "config.h"
#include "HT1621/STC8A_HT1621.h"

#define SegLCD_WriteData Ht1621WrOneDat
#define SegLCD_WriteNum Ht1621_DisplayNum

// TClock 时钟 (BCD格式)
typedef struct Clock {
    word Year;
    byte Month;
    byte Day;
    byte Hour;
    byte Minute;
    byte Second;
} TClock;

typedef TClock (*FuncGetClock)();
typedef void (*ProcSetClock)(TClock *c);

typedef struct SLCD TLCD;

typedef void (*ProcWriteData)(byte Addr, byte Data);
typedef void (*ProcWriteNum)(byte APos, byte HasDot, byte AValue);

struct SLCD 
{
    FuncVoid Clear;                 // { 清屏 }
    FuncVoid ShowAll;               // { 全显 }
    // ProcUint32 ShowVersion;     // { 显示版本号 }
    // Proc ShowSector;            // { 显示M1扇区号, 扣费类型}
    // Procs ShowDate;             // { 显示日期 }
    // Procs ShowTime;             // { 显示时间 }
    // ProcByte SetLight;          // { 控制背光 }

    // ProcUint32 ShowTimeCounted;     // { 调试：显示累计时间 }     
    // ProcUint32 ShowSecCounted;      // { 调试：显示累计时间 }
    // ProcUint32 ShowPulseCounted;    // { 调试：显示累计脉冲 }

    // ProcUint32 ShowBalance;         // { 显示余额 }

    // ProcWriteData WriteDataD;       // { 更新数据 }
    // ProcUint16 WriteUint16;         // { 更新数值 }
    // ProcPUint32 WriteBalance;       // { 更新余额字段 }
    // ProcByte WriteWorkTag;          // { 更新标志字段 }
    // ProcPUint32 WriteUserPulse;     // { 更新脉冲计数字段 }
    // ProcByte WriteUserTime;         // { 更新时间计数字段 }
    
    // ProcUint16 ShowError;           // { 显示错误代码 }
    // ProcUint16 ShowAuthCode;        // { 显示授权号 }
    // ProcByte WriteCardProper;       // { 更新数字字段(16进制) }
    
    // ProcByte ShowStep;            // { 显示操作进度 }
    // Proc ShowOver;                // { 显示操作结束 }

    // FuncGetClock GetClock;
    // ProcSetClock SetClock;
};
void InitSegLCD(void);
void CreateLCD(void);

extern TLCD LCD;

#endif