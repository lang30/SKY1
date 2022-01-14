#include "reader_cpu.h"
#include "rc522/STC8A_RC522.h"
#include "WCM_param.h"
#include "xrcard.h"
#include <string.h>

byte xdata CPUReadBuf[64];
byte xdata CPUReadLen[2];
byte xdata CPURand[4];

byte code CmdSelectFile[7] = {0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00};
// B0 读文件
volatile byte CmdReadFile[5] = {0x00, 0xB0, 0x85, 0x00, 0x10};
// D6 写文件
volatile byte CmdWriteFile[5] = {0x00, 0xD6, 0x87, 0x00, 0x10};
// 读 84 随机数
byte code CmdReadRand[5] = {0x00, 0x84, 0x00, 0x00, 0x04};
byte CmdAuthExtSign[13] = {0x00, 0x82, 0x00, 0x14, 0x08, 0, 0, 0, 0, 0, 0, 0, 0};

// ReadCPUFile 读文件
error CmdReadCPUFile(byte ADocID, pbyte AData, byte ALen) {

    error err;
    byte oI;

    CmdReadFile[2] = ADocID;
    CmdReadFile[4] = ALen;

    memset(CPUReadBuf, 0, MAXRLEN);

    err = Pcd_Cmd(CmdReadFile, 5, CPUReadBuf, CPUReadLen);
    if (err != nil) {
        return 1;
    }

    for (oI = 0; oI < CPUReadLen[0] - 2; oI++) {
        AData[oI] = CPUReadBuf[oI + 2];
    }

    return 0;
}

// CmdWriteCPUBlock 写16字节数据块（老方案）
error CmdWriteCPUBlock(byte ADocID, pbyte AData) {

    error err;
    byte data oCmdBuf[0x05 + 0x10];

    CmdWriteFile[2] = ADocID;
    CmdWriteFile[4] = 0x10;

    memmove(oCmdBuf, CmdWriteFile, 0x05);
    memmove(&oCmdBuf[5], AData, 0x10);

    err = Pcd_Cmd(oCmdBuf, 0x15, CPUReadBuf, CPUReadLen);
    if (err != nil) {
        return 1;
    }

    return 0;
}

// CmdReadCPURand 读取4字节随机数
error CmdReadCPURand() {

    static error err;

    memset(CPURand, 0, 4);

    err = Pcd_Cmd(CmdReadRand, 5, CPUReadBuf, CPUReadLen);
    if (err != nil) {
        return 1;
    }

    if ((CPUReadBuf[6] << 8 | CPUReadBuf[7]) != 0x9000) {
        return 1;
    }

    memmove(CPURand, &CPUReadBuf[2], 4);
    return 0;
}

// CmdAuthCPUExt 外部认证
error CmdAuthCPUExt(pbyte ASign) small {

    memmove(&CmdAuthExtSign[5], ASign, 8);

    if (Pcd_Cmd(CmdAuthExtSign, 13, CPUReadBuf, CPUReadLen) != SUCCESS) {
        return 1;
    }
    return 0;
}

// DoAuthCPU 认证CPU卡密钥
error DoAuthCPU(pbyte ACardSN) {

    static byte oBuf[32];
    static byte oSign[8];
    static error err;

    err = CmdReadCPUFile(0x85, oBuf, 0x20);
    if (err != nil) {
        return 411;
    }

    err = VerifyCPUKeyUnsafe(ACardSN, oBuf, PosParam.M1Purse.Key);
    if (err != nil) {
        return 412;
    }

    err = CmdReadCPURand();
    if (err != nil) {
        return 413;
    }

    err = GenCPUExtSign(ACardSN, CPURand, PosParam.M1Purse.Key, oSign);
    if (err != nil) {
        return 414;
    }

    err = CmdAuthCPUExt(oSign);
    if (err != nil) {
        return 415;
    }

    return 0;
}