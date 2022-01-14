#include "xrcard.h"
#include "hex.h"
#include "des.h"
#include <string.h>

byte GDataType = 0;
volatile bool data GCPUCard = 0;

static error VerifyBlock(pbyte ACardSN, pbyte ABlock, bool AMoneyBlock) small {
    // var
    static byte oMix[5];
    byte oHeader = 0;
    byte oFooter = 0;

    memmove(oMix, ACardSN, 4);
    oMix[4] = 0x5A;

    // 检查首尾校验位 ( 方案 0 / 1 )
    oHeader = BytesXor(ABlock, 1, 14);
    oFooter = BytesAdd(ABlock, 1, 14);
    if ( ((GDataType == 1) || GCPUCard ) &&
        ((AMoneyBlock) || (!AMoneyBlock && (ABlock[5] == 0xBB)))) {
        oHeader = oHeader ^ BytesXor(oMix, 0, 5);
        oFooter = oFooter + BytesAdd(oMix, 0, 5);
    }
    oFooter = oFooter ^ 0xFF;

    if (oHeader != ABlock[0]) {
        return 1;
    }

    if (oFooter != ABlock[15]) {
        return 1;
    }

    return 0;
}

static void SignBlock(pbyte ACardSN, pbyte ABlock, bool AMoneyBlock) small {
    // var
    static byte oMix[5];
    byte oHeader = 0;
    byte oFooter = 0;

    memmove(oMix, ACardSN, 4);
    oMix[4] = 0x5A;

    // 检查首尾校验位 ( 方案 0 / 1 )
    oHeader = BytesXor(ABlock, 1, 14);
    oFooter = BytesAdd(ABlock, 1, 14);

    if (((GDataType == 1) || GCPUCard ) &&
        ((AMoneyBlock) || (!AMoneyBlock && (ABlock[5] == 0xBB)))) {
        oHeader = oHeader ^ BytesXor(oMix, 0, 5);
        oFooter = oFooter + BytesAdd(oMix, 0, 5);
    }

    oFooter = oFooter ^ 0xFF;

    ABlock[0] = oHeader;
    ABlock[15] = oFooter;
}

error DecodeNumBlock(pbyte ACardSN, pbyte ABlock) small {
    // var
    static error err;

    err = VerifyBlock(ACardSN, ABlock, false);
    if (err != nil) {
        return err;
    }

    // 检查卡属性校验位
    if (ABlock[6] != BytesAddReverse(ABlock, 1, 5)) {
        return 1;
    }

    // 检查电子卡号校验
    if (ABlock[10] != BytesAddReverse(ABlock, 7, 3)) {
        return 1;
    }

    return 0;
}

error DecodeMoneyBlock(pByte ACardSN,
                       pbyte ABlock,
                       uint32 *ABalance,
                       uint16 *ATradeSerial) small {
    // var
    static error err;

    err = VerifyBlock(ACardSN, ABlock, true);
    if (err != nil) {
        return err;
    }

    if (ABlock[1] != BytesAdd(ABlock, 2, 3)) {
        return 1;
    }

    if (ABlock[5] != (ABlock[1] ^ 0xff)) {
        return 1;
    }

    *ABalance =
        ((uint32)ABlock[4] << 16) + ((uint32)ABlock[3] << 8) + ABlock[2];
    *ATradeSerial = ((uint16)ABlock[9] << 8) + ABlock[13];

    return 0;
}

void EncodeMoneyBlock(pByte ACardSN, pByte ABlock, uint32 *ABalance, uint16 *ATradeSerial) small {

    ABlock[4] = (byte)(*ABalance >> 16);
    ABlock[3] = (byte)(*ABalance >> 8);
    ABlock[2] = (byte)(*ABalance);

    ABlock[9] = (byte)(*ATradeSerial >> 8);
    ABlock[13] = (byte)(*ATradeSerial);

    ABlock[1] = BytesAdd(ABlock, 2, 3);
    ABlock[5] = ABlock[1] ^ 0xff;

    SignBlock(ACardSN, ABlock, true);
}

// { CPU 卡专用}
// 0x00: 9F | 90 0F  60 00 00 00 00 00  | 00   00 00 00 00 00 00
// 0x10: [header 55] | 5A 14 [key 14 04 72 C5 66 8C] | [footer 0B] D9 00 00 00 00 00
error VerifyCPUKeyUnsafe(pByte ACardSN, pbyte AFileBuf, pByte APosKey) {

    static byte oKey[6];
    static byte oI;
    static byte oHeader, oFooter;

    memmove(oKey, &AFileBuf[0x13], 6);

    // 01 23 45 67 89 5A, 得到 oKey: 05 51 80 01 05 51
    for (oI = 0; oI < 5; oI++) {
        oKey[oI] = oKey[oI] ^ (0x01 + 0x22 * oI);
    }
    oKey[5] = oKey[5] ^ 0x5A;

    // 比较密钥
    for (oI = 0; oI < 6; oI++) {
        if (oKey[oI] != APosKey[oI]) {
            return 1;
        }
    }

    oHeader = BytesXor(AFileBuf, 0x11, 8) ^ BytesXor(ACardSN, 0, 4);
    oFooter = BytesAdd(AFileBuf, 0x11, 8) + BytesAdd(ACardSN, 0, 4);

    if (AFileBuf[0x10] != oHeader) {
        return 1;
    }

    if (AFileBuf[0x19] != oFooter) {
        return 1;
    }

    return 0;
}

error GenCPUExtSign(pbyte ACardSN, pbyte ARand, pbyte AKey, pbyte ADst) {

    static byte oI;
    static byte oSrc[8];
    static byte oKey[8];
    static byte oBuf[8];

    memset(ADst, 0, 8);

    // 待加密8字节数据
    memmove(oSrc, ARand, 4);
    memset(&oSrc[4], 0, 4);

    //oBuf: S0 S1 5A 5A 5A 5A S2 S3
    memmove(oBuf, &ACardSN[2], 2);
    memset(&oBuf[2], 0x5A, 4);
    memmove(&oBuf[6], ACardSN, 2);
    //oKey: K0 K1 K2 K3 K4 K5 5A 5A
    memmove(oKey, AKey, 6);
    memset(&oKey[6], 0x5A, 2);

    // 生成8字节密钥
    for (oI = 0; oI < 8; oI++) {
        oKey[oI] = oKey[oI] ^ oBuf[oI];
    }

    des(oSrc, oKey, 0, ADst);

    return 0;
}