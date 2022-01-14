#ifndef __XRCARD_H__
#define __XRCARD_H__

#include "config.h"

// M1数据类型
extern byte GDataType;
// 当前读卡类型是否为CPU
extern bool GCPUCard;

// { 解析身份块数据 }
error DecodeNumBlock(pbyte ACardSN, pbyte ABlock) small;

// { 解析金额块数据 }
error DecodeMoneyBlock(pByte ACardSN,
                       pbyte ABlock,
                       uint32 *ABalance,
                       uint16 *ATradeSerial) small;

// { 生成金额块数据 }
void EncodeMoneyBlock(pByte ACardSN, pByte ABlock, uint32 *ABalance, uint16 *ATradeSerial) small;

error VerifyCPUKeyUnsafe(pByte ACardSN, pbyte AFileBuf, pbyte APosKey);

// { 生成外部认证签名 }
error GenCPUExtSign(pbyte ACardSN, pbyte ARand, pbyte AKey, pbyte ADst);

#endif