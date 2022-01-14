#ifndef __BEEP_H__
#define __BEEP_H__

#include "config.h"

typedef struct SBeep TBeep;

struct SBeep {
    ProcUint32 Delay;
    ProcByte BeepGap;
    Procs BeepOnce;
    ProcsUint16 BeepTime;
};

extern TBeep Beep;

void InitBeep();

#endif