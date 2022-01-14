#ifndef __HEX_H__
#define __HEX_H__

#include "config.h"

// { 异或 }
byte BytesXor(pByte ABytes, word APosition, word ACount) small;
// { 累加和 }
byte BytesAdd(pByte ABytes, word APosition, word ACount) small;
// { 累加和取反 }
byte BytesAddReverse(pByte ABytes, word APosition, ACount) small;

// { 数值转为BCD编码 }
byte ByteToBcd(byte AValue);
// { BCD编码转为数值 }
byte BcdToByte(byte AValue);

#endif