#include "hex.h"

byte BytesAdd(pByte ABytes, word APosition, word ACount) small {
    // var
    byte result = 0;
    word oI;

    for (oI = APosition; oI < APosition + ACount; oI++) {
        result = result + ABytes[oI];
    }

    return result;
}

byte BytesXor(pByte ABytes, word APosition, word ACount) small {
    // var
    byte result = 0;
    word oI;

    for (oI = APosition; oI < APosition + ACount; oI++) {
        result = result ^ ABytes[oI];
    }

    return result;
}

byte BytesAddReverse(pByte ABytes, word APosition, ACount) small {
    return BytesAdd(ABytes, APosition, ACount) ^ 0xFF;
}

byte ByteToBcd(byte AValue) {
    return (AValue / 10) * 0x10 + (AValue % 10);
}

byte BcdToByte(byte AValue) {
    return (AValue / 0x10) * 10 + (AValue % 0x10);
}