#include "pos_timer.h"
#include "STC8A_Timer.h"

volatile uint32 data TimeMSCounted;
volatile uint32 data PulseCounted;

void OnTime(void) interrupt TIMER3_VECTOR {
    TimeMSCounted++;
}

void OnPulse(void) interrupt TIMER4_VECTOR {
    PulseCounted++;
}

void SetTimeCounter(bool AOn) {
    if (AOn) {
        TimeMSCounted = 0;
        Timer3_Run(1);
    } else {
        Timer3_Run(0);
    }
}

void SetPulseCounter(bool AOn) {
    if (AOn) {
        PulseCounted = 0;
        Timer4_Run(1);
    } else {
        Timer4_Run(0);
    }
}

uint32 GetTimeMSCounted() {
    return TimeMSCounted;
}