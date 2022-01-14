#ifndef __POS_TIMER_H__
#define __POS_TIMER_H__

#include "config.h"

// { 开/关 时间计数器 }
void SetTimeCounter(bool AValue);

// { 开/关 脉冲计数器 }
void SetPulseCounter(bool AValue);

extern uint32 data TimeMSCounted;
extern uint32 data PulseCounted;

uint32 GetTimeMSCounted();

#endif