#include "wcm_param.h"
#include "beep.h"
#include "STC8A_EEPROM.h"
#include "seglcd.h"
#include "hex.h"
#include <string.h>

#define DoReadN EEPROM_read_n
#define DoWriteN EEPROM_write_n

volatile TPosParam PosParam;

static byte xdata BlockBuf[0x50];

error ReadBlock(uint16 Address) {
    DoReadN(Address, &BlockBuf, 0x50);
    if (BlockBuf[0x48] != BytesXor(BlockBuf, 0, 48)) {
        return 1;
    }
    if (BlockBuf[0x49] != BytesAddReverse(BlockBuf, 0, 48)) {
        return 1;
    }
    return 0;
}

void SignBlock() {
    BlockBuf[0x48] = BytesXor(BlockBuf, 0, 48);
    BlockBuf[0x49] = BytesAddReverse(BlockBuf, 0, 48);
}

// { 基础参数 0x00~0x49 }

// 00 M1Purse.Sector          M1卡大钱包号
// 01 M1Purse.DataType        M1卡大钱包数据类型 0 普通 1 防复制
// 02 M1Purse.KeyType         M1卡大钱包密码系列: 1 随机 7 固定
// 03 M1Purse.Key[6]          M1卡大钱包密钥

// 09 M1PurseSub.Sector       M1卡小钱包号
// 10 M1PurseSub.DataType     M1卡小钱包数据类型
// 11 M1PurseSub.KeyType      M1卡小钱包密码系列
// 12 M1PurseSub.Key[6]       M1卡小钱包密钥

// 18 CPUPurse.Sector         CPU卡钱包号
// 19 CPUPurse.DataType       CPU卡数据类型
// 20 CPUPurse.KeyType        CPU卡密钥类型
// 21 CPUPurse.Key[6]         CPU卡密钥

// 27 AuthCode[2]             授权号
// 29 PosIP[2]                设备编号

void ReadBaseParam() {
    if (ReadBlock(0x0000) != 0) {
        return;
    }

    PosParam.M1Purse.Sector = BlockBuf[0];
    PosParam.M1Purse.DataType = BlockBuf[1];
    PosParam.M1Purse.KeyType = BlockBuf[2];
    memmove(PosParam.M1Purse.Key, &BlockBuf[3], 6);

    PosParam.M1PurseSub.Sector = BlockBuf[9];
    PosParam.M1PurseSub.DataType = BlockBuf[10];
    PosParam.M1PurseSub.KeyType = BlockBuf[11];
    memmove(PosParam.M1PurseSub.Key, &BlockBuf[12], 6);

    PosParam.CPUPurse.Sector = BlockBuf[18];
    PosParam.CPUPurse.DataType = BlockBuf[19];
    PosParam.CPUPurse.KeyType = BlockBuf[20];
    memmove(PosParam.CPUPurse.Key, &BlockBuf[21], 6);

    memmove(&PosParam.AuthCode, &BlockBuf[27], 2);
    memmove(PosParam.PosIP, &BlockBuf[29], 2);
  
}

void SaveBaseParam() {

    memset(BlockBuf, 0x00, 0x50);

    BlockBuf[0] = PosParam.M1Purse.Sector;
    BlockBuf[1] = PosParam.M1Purse.DataType;
    BlockBuf[2] = PosParam.M1Purse.KeyType;
    memmove(&BlockBuf[3], PosParam.M1Purse.Key, 6);

    BlockBuf[9] = PosParam.M1PurseSub.Sector;
    BlockBuf[10] = PosParam.M1PurseSub.DataType;
    BlockBuf[11] = PosParam.M1PurseSub.KeyType;
    memmove(&BlockBuf[12], PosParam.M1PurseSub.Key, 6);

    BlockBuf[18] = PosParam.CPUPurse.Sector;
    BlockBuf[19] = PosParam.CPUPurse.DataType;
    BlockBuf[20] = PosParam.CPUPurse.KeyType;
    memmove(&BlockBuf[21], PosParam.CPUPurse.Key, 6);

    memmove(&BlockBuf[27], &PosParam.AuthCode, 2);
    memmove(&BlockBuf[29], PosParam.PosIP, 2);

    SignBlock();

    DoWriteN(0x0000, BlockBuf, 0x50);
}

// { 业务参数 0x50~0x99 }

// 00 PosType[2]              设备类型 0 单路 1 双路 3 全双路 6 吹风机 4 洗衣机
// 02 MeasureMode             计量模式 脉冲/时间
// 03 DayLimit[3]             每日限额金额量
// 06 ControlMode             控制模式 00 单路/ 01 双路
// 07 StopSecond[2]           停机时间
// 09 RefundSecond[2]         超时退款时间
// 11 OutPulse[2]             脉冲
// 13 OutFee[2]               费用
// 15 Out1Pulse[2]            1路扣款脉冲
// 17 Out1Fee[2]              1路预扣金额
// 19 Out2Pulse[2]            2路扣款脉冲
// 21 Out2Fee[2]              2预扣金额
// 23 BootSecond[2]           延迟扣款时间

void ReadWorkParam() {
    if (ReadBlock(0x0050) != 0) {
        return;
    }

    memmove(&PosParam.PosType, BlockBuf, 2);
    memmove(&PosParam.MeasureMode, &BlockBuf[2], 1);
    memmove(&PosParam.DayLimit, &BlockBuf[4], 4); // 待确定值正确
    memmove(&PosParam.ControlMode, &BlockBuf[8], 1);
    memmove(&PosParam.StopSecond, &BlockBuf[9], 2);
    memmove(&PosParam.RefundSecond, &BlockBuf[11], 2);
    memmove(&PosParam.OutPulse, &BlockBuf[13], 2);
    memmove(&PosParam.OutFee, &BlockBuf[15], 2);
    memmove(&PosParam.Out1Pulse, &BlockBuf[17], 2);
    memmove(&PosParam.Out1Fee, &BlockBuf[19], 2);
    memmove(&PosParam.Out2Pulse, &BlockBuf[21], 2);
    memmove(&PosParam.Out2Fee, &BlockBuf[23], 2);
    memmove(PosParam.FreeUse, &BlockBuf[25], 4);
}

void SaveWorkParam() {
    memset(BlockBuf, 0x00, 0x50);

    memmove(BlockBuf, &PosParam.PosType, 2);
    memmove(&BlockBuf[2], &PosParam.MeasureMode, 1);
    memmove(&BlockBuf[4], &PosParam.DayLimit, 4);
    memmove(&BlockBuf[8], &PosParam.ControlMode, 1);
    memmove(&BlockBuf[9], &PosParam.StopSecond, 2);
    memmove(&BlockBuf[11], &PosParam.RefundSecond, 2);
    memmove(&BlockBuf[13], &PosParam.OutPulse, 2);
    memmove(&BlockBuf[15], &PosParam.OutFee, 2);
    memmove(&BlockBuf[17], &PosParam.Out1Pulse, 2);
    memmove(&BlockBuf[19], &PosParam.Out1Fee, 2);
    memmove(&BlockBuf[21], &PosParam.Out2Pulse, 2);
    memmove(&BlockBuf[23], &PosParam.Out2Fee, 2);
    memmove(&BlockBuf[25], &PosParam.FreeUse, 4);

    SignBlock();

    DoWriteN(0x0050, BlockBuf, 0x50);
}

const byte DefaultKey[6] = {0x05, 0x51, 0x80, 0x01, 0x05, 0x51};

void InitPosParam() {

    PosParam.SaveBaseParam = SaveBaseParam;
    PosParam.SaveWorkParam = SaveWorkParam;

    // PosParam.M1Purse.Sector = 8;
    // PosParam.M1Purse.DataType = 0;
    // PosParam.M1Purse.KeyType = 7;
    // memmove(PosParam.M1Purse.Key, DefaultKey, 6);

    // PosParam.M1PurseSub.Sector = 9;
    // PosParam.M1PurseSub.DataType = 0;
    // PosParam.M1PurseSub.KeyType = 7;
    // memmove(PosParam.M1PurseSub.Key, DefaultKey, 6);

    // // todo: 判断CPU引脚短接，使用1501读卡参数

    // PosParam.AuthCode = 0x6004;
    // PosParam.PosIP[0] = 0xFF;
    // PosParam.PosIP[1] = 0xFF;

    // PosParam.MeasureMode = 0x00;
    // PosParam.OutPulse = 0x01;
    // PosParam.OutFee = 0x01;

    // PosParam.SaveBaseParam();
    // PosParam.SaveWorkParam();

    ReadBaseParam();
    ReadWorkParam();
}

void SetFactory()
{
    PosParam.M1Purse.Sector = 8;
    PosParam.M1Purse.DataType = 0;
    PosParam.M1Purse.KeyType = 7;
    memmove(PosParam.M1Purse.Key, DefaultKey, 6);

    PosParam.M1PurseSub.Sector = 9;
    PosParam.M1PurseSub.DataType = 0;
    PosParam.M1PurseSub.KeyType = 7;
    memmove(PosParam.M1PurseSub.Key, DefaultKey, 6);

    // todo: 判断CPU引脚短接，使用1501读卡参数

    PosParam.AuthCode = 0x6004;
    PosParam.PosIP[0] = 0xFF;
    PosParam.PosIP[1] = 0xFF;

    PosParam.MeasureMode = 0x00;
    PosParam.OutPulse = 0x01;
    PosParam.OutFee = 0x01;

    PosParam.SaveBaseParam();
    PosParam.SaveWorkParam();

    ReadBaseParam();
    ReadWorkParam();
}