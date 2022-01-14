#ifndef __STC8A_PCF8563__H__
#define __STC8A_PCF8563__H__

#include "config.h"
#include "STC8A_I2C.h"


void PCF8563_GetTime(u8 *tmp);    //获取时间
void PCF8563_SetTime(u8 *timeSet);  //设置时间
u8 ToDec(u8 bcd);
u8 ToBcd(u8 hex);

#endif