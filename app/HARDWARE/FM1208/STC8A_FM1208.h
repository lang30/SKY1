#ifndef __STC8A_FM1208__H__
#define __STC8A_FM1208__H__

#include "config.h"


void CPUCardProsing(void);
char Choose_MF_File(void);
char Get_Random(u8 *pRandom_array);
char Verify_DES(u8 *Random,u8 *key);
void read_BinaryFile(u8 DFile,u8* pDataOut,u8 Out_Len);
char UserKey_Verify(u8 *UserKey);
char Data_validation(u8 *vData);
void Disp_money_CPU(u32 money);
void Timer_Deduction_Mode_CPU(u32 *tCardMoney);
void Hall_Deduction_Mode_CPU(u32 *hCardMoney);
void Cut_payment_CPU(u32 *money);

#endif


