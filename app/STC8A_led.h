#ifndef __STC8A_LED_H_
#define __STC8A_LED_H_

#include "config.h"

#define LED_OFF    P20=1
#define LED_ON     P20=0


struct LED_s
{
    FuncVoid Flash;
    FuncVoid Stop;
};
extern struct LED_s LED;

void Run_LED(void);
void SystemError_LED(void);
void St_LED(void);
void Init_LED(void);

#endif
