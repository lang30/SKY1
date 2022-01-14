#ifndef __READER_CPU_H__
#define __READER_CPU_H__

#include "config.h"

extern byte xdata CPUReadBuf[64];
extern byte xdata CPUReadLen[2];
extern byte xdata CPURand[4];

extern byte code CmdSelectFile[7];
extern byte CmdReadFile[5];
extern byte code CmdReadRand[5];
extern byte CmdAuthExtSign[13];

// { 读取文件 }
error CmdReadCPUFile(byte ADocID, pbyte AData, byte ALen);
// { 写16字节区块数据 }
error CmdWriteCPUBlock(byte ADocID, pbyte AData);
// { 读取随机数 }
error CmdReadCPURand();
// { 认证密钥 }
error DoAuthCPU(pbyte ACardSN);

#endif