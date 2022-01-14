/*---------------------------------------------------------------------*/
/* ---       兴日科技 FM1208 CPU卡 读卡程序                       -------*/
/* 请注意！     此文件代码为高度保密，                                    */
/*---------------------------------------------------------------------*/

#include "STC8A_FM1208.h"
#include "STC8A_RC522.h"
#include "string.h"
#include "STC8A_EEPROM.h"
#include "des.h"
#include "STC8A_CardProc.h"
#include "STC8A_Delay.h"
#include "STC8A_HT1621.h"
#include "STC8A_Timer.h"

u8 xdata g_ucTempbuf[MAXRLEN]; 
u8 CardData_1[16];          //卡块1数据缓存
//////////////////CPU卡的指令////////////////////////////////
extern u32 T4_cnt;
extern u32 Time_Num;
extern u8 Card_ID[4];
extern u8 DeductionMode;          //消费模式
unsigned char code Choose_MF_File[7]={0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00};   //选择MF文件
unsigned char code Get_Random[5]={0x00, 0x84, 0x00, 0x00, 0x04};   //获取随机数
unsigned char Verify_DES[13]={0x00, 0x82, 0x00, 0x14, 0x08, 0,0,0,0, 0,0,0,0};   //获取随机数
unsigned char code read_DF16[5]={0x00,0xB0,0x86,0x00,0x10};
unsigned char code read_DF17[5]={0x00,0xB0,0x87,0x00,0x10};
unsigned char code read_DF18[5]={0x00,0xB0,0x88,0x00,0x10};
unsigned char code write_DF16[5]={0x00,0xD6,0x86,0x00,0x10};
unsigned char code write_DF17[5]={0x00,0xD6,0x87,0x00,0x10};
unsigned char code write_DF18[5]={0x00,0xD6,0x88,0x00,0x10};


/********************************************************************
 * @brief      CPU 卡处理函数 
 * @param[in]  
 * @return     
 * @note      
*********************************************************************/ 
void CPUCardProsing()
{
    u8 tt[2];
    u8 i;
    char status;
	unsigned char cipher1[8];	//密文缓存
	u8 CardKey_Buff[6];        //卡秘钥缓存；
	unsigned char key_tab[8];   //密钥
	unsigned char Cleartext_tab[8];  //明文
	unsigned char CARDid_5A[8];
	u32 CardMoney;
    u8 Sum_NOT, Sum_XOR;        //和取反，和异或
	u16 tmp;
	u16 uConsumeAmount;
	u8 EEPROM_Array_buff[2];      //消费费率缓存

    memset(g_ucTempbuf, 0x00, MAXRLEN);	        //缓存数组清0
    if(CardReset(g_ucTempbuf,tt)!=MI_OK)		  //CPU卡复位函数
	{  return; }
    printf("ATS:");
    for(i=0; i<tt[0]; i++)
    {
       printf("%b02X",g_ucTempbuf[i]); 
    }
    printf("\r\n");
    //////////// 选择MF 文件 ///////////////////
    status = MI_OK;
    status=Pcd_Cmd(Choose_MF_File, 7, g_ucTempbuf, tt);
    if(status)	//1时出错了
    {
        memset(g_ucTempbuf, 0x00, MAXRLEN);	//清0				
        printf("MF_File error! \r\n");
        return;						//返回	
    }
    printf("MF_File: ");
    for(i=0; i<tt[0]; i++)
    {  printf("%b02X",g_ucTempbuf[i]);  }
    printf("\r\n");
    //////////////获取随机数 //////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//清0	
    status = MI_OK;
    status=Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
    if(status)	//1时出错了
    {		
        printf("Get_Random error! \r\n");
        return;						//返回	
    }
    printf("Get_Random: ");             //显示返回值
    for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
    printf("\r\n");
	//////////////DES 验证///////////////////
	for(i=0; i<4; i++)
	{	
		Cleartext_tab[i] = g_ucTempbuf[i+2];      //随机数加上 0000 组成 DES 数据
		Cleartext_tab[i+4] = 0x00;
	}	
    EEPROM_read_n(CardKEY_Addr,CardKey_Buff,6);      //从EEPROM中读出存储的秘钥CardKey_Buff，  
	for(i=0; i<6; i++){
		key_tab[i] = CardKey_Buff[i];
	}
	key_tab[6] = 0X5A;	key_tab[7] = 0X5A;       //尾部加上 5A5A 补齐8个字节
    
    //  用卡和5A 组合
	CARDid_5A[0]=Card_ID[2]; CARDid_5A[1]=Card_ID[3]; 
	CARDid_5A[2]=0X5A; CARDid_5A[3]=0X5A; CARDid_5A[4]=0X5A; CARDid_5A[5]=0X5A; 
	CARDid_5A[6]=Card_ID[0]; CARDid_5A[7]=Card_ID[1];

	for(i=0; i<8; i++)    
	{	key_tab[i] = key_tab[i] ^ CARDid_5A[i]; }    // 8字节密钥 异或 卡和5A 组合数据  得到 密钥

	des(Cleartext_tab, key_tab, 0, cipher1);    //加密
    printf("DES: ");             //显示返回值
    for(i=0; i<8; i++)
    {  printf("%b02X",cipher1[i]);
	   Verify_DES[i+5] = cipher1[i];
	}
    printf("\r\n");

	memset(g_ucTempbuf, 0x00, MAXRLEN);	//清0	
    status = MI_OK;
    status=Pcd_Cmd(Verify_DES, sizeof(Verify_DES), g_ucTempbuf, tt);  //验证DES 
    if(status)	//1时出错了
    {
		printf("Verify_DES error! \r\n");
        return;						//返回
	}
	for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
	printf("\r\n");
    ///////////////////读取 16 文件 ////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//清0	
    status = MI_OK;
    status=Pcd_Cmd(read_DF16, sizeof(read_DF16), g_ucTempbuf, tt);
	printf("DF16: ");             //显示返回值
	for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
	printf("\r\n");
    ///////////////////读取 17 文件 ////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//清0	
    status = MI_OK;
    status=Pcd_Cmd(read_DF17, sizeof(read_DF17), g_ucTempbuf, tt);
	printf("DF17: ");             //显示返回值
	for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
	printf("\r\n");
    ///////////////////读取 18 文件 ////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//清0	
    status = MI_OK;
    status=Pcd_Cmd(read_DF18, sizeof(read_DF18), g_ucTempbuf, tt);
	printf("DF18: ");             //显示返回值
	for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
	printf("\r\n");
	
    /////////////////////////////////////////////////////////////////////
	for(i=0; i<16; i++)
	{	CardData_1[i] = g_ucTempbuf[i+2]; }

	Sum_NOT = 0; Sum_XOR = 0;
	for(i=1; i<15; i++)      
	{   Sum_NOT += CardData_1[i];    Sum_XOR ^= CardData_1[i]; }  
	Sum_NOT = ~(Sum_NOT+Card_ID[0]+Card_ID[1]+Card_ID[2]+Card_ID[3]+0x5A);
	Sum_XOR = Sum_XOR^Card_ID[0]^Card_ID[1]^Card_ID[2]^Card_ID[3]^0x5A;
	if(((Sum_NOT & 0xFF) != CardData_1[15]) && (Sum_XOR != CardData_1[0]))
		{CardDat_Error(3,2);  False_Alarm();   return;}
	if(CardData_1[1] != ((CardData_1[2] +CardData_1[3] +CardData_1[4] ) & 0xFF))
		{CardDat_Error(3,3);  False_Alarm();    return;}
	if(CardData_1[5] != ((~(CardData_1[2] +CardData_1[3] +CardData_1[4])) & 0xFF))  // X1F=X11……X1E累加和的值再取反（进位忽略）
		{CardDat_Error(3,4);  False_Alarm();    return;}
	CardMoney = (u32)CardData_1[4]<<16 | (u32)CardData_1[3]<<8 | CardData_1[2] ;

	tmp = 0x9000;
	while (tmp == 0x9000)
	{
		tmp = 0;
		memset(g_ucTempbuf, 0x00, 8);
		Disp_money(CardMoney);
		delay_ms(500);   //刷卡延时，看清卡内余额；
		if(CardMoney>=99999)              //消费金额超额，非法数据终止；
        {
			tmp = 0x9000;
            for (;tmp == 0x9000;)        //卡不拿走一直显示；
            {
                Disp_money(CardMoney);
                Ht1621WrOneDat(11,0x0A);    // 符号显示：元  满
                Ht1621WrOneDat(12,0x02);    // 符号显示：元  限量
                Ht1621WrOneDat(17,0x08);    // 符号显示：错误
                False_Alarm();
                delay_ms(300);
				memset(g_ucTempbuf, 0x00, 8);
                Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
				tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];
            }
            return;
        }
		EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);     //读取设定的消费费率,
        uConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1]; 
        if(CardMoney == 0 || (CardMoney < uConsumeAmount))  //余额不足显示；
        {	
            tmp = 0x9000;
            for (;tmp == 0x9000;)        //卡不拿走一直显示；
            {
                Ht1621WrOneDat(17,0x06); //小于预扣款，显示停
                Disp_money(CardMoney);
                delay_ms(300);
                Ht1621Clear(0,32); // - - HT1621 清屏
                delay_ms(300);
                Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
				tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];
            }
            return;
        } 
        EEPROM_read_n(DeductionMode_Addr,&DeductionMode,1);
        if(DeductionMode == 0x60)        //进入计量消费模式
        {
            Hall_Deduction_Mode_CPU(CardMoney);
        }
        else if(DeductionMode == 0x30)
        {
            Timer_Deduction_Mode_CPU(CardMoney);
        }

		Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
		tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];
	}
}


void Cut_payment_CPU(u8 money)
{
	u8 i;
    u8 sum = 0;
    u8 sum1 = 0;
    u8 Card_Data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	u8 Card_Data_write[21];
	char status;
	u8 tt[2];

    Card_Data[4] = (u8)(((u8)(money>>16)) & 0xFF);
    Card_Data[3] = (u8)(((u8)(money>>8)) & 0xFF);
    Card_Data[2] = ((u8)(money) & 0xff);

    Card_Data[1] = (Card_Data[2]+Card_Data[3]+Card_Data[4]) & 0xff;//X01=X02+X03+X04的累加和的值（进位忽略）
    Card_Data[5] = (~(Card_Data[2]+Card_Data[3]+Card_Data[4])) & 0xff; // ⑺X05=X02+X03+X04的累加和的值再取反（进位忽略） 
    Card_Data[13] = CardData_1[13]+1;	Card_Data[14] = CardData_1[14]+1;
    for(i=1;i<15;i++)
    {
        sum += Card_Data[i];
        sum1 ^= Card_Data[i]; 
    }
    Card_Data[15] = ~(sum+Card_ID[0]+Card_ID[1]+Card_ID[2]+Card_ID[3]+0x5A) & 0xFF; 
    Card_Data[0] = sum1^Card_ID[0]^Card_ID[1]^Card_ID[2]^Card_ID[3]^0x5A;    // X1F=X11……X1E累加和的值再取反（进位忽略）
    
	for(i=1;i<5;i++)
    { Card_Data_write[i] = write_DF18[i]; }
	for(i=0;i<16;i++)
    { Card_Data_write[i+5] = Card_Data[i]; }
    status = MI_OK;
    status=Pcd_Cmd(Card_Data_write, sizeof(Card_Data_write), g_ucTempbuf, tt);

	for(i=1;i<5;i++)
    { Card_Data_write[i] = write_DF17[i]; }
	for(i=0;i<16;i++)
    { Card_Data_write[i+5] = Card_Data[i]; }	
    status = MI_OK;
    status=Pcd_Cmd(Card_Data_write, sizeof(Card_Data_write), g_ucTempbuf, tt);
}

/*********************************************************
 * @brief      霍尔计量消费模式
 * @details    读出EEPROM存储的消费霍尔量；
 *             第二部验证：读取卡金额数据处理 CardMoney
 *             第三步验证： X1A=X17+X18+X19的累加和的值再取反（进位忽略）
 * @param[in]  CardMoney: 卡金额；Sectors：当前扇区
 * @return     
 * @date       2021-8-7 10:41:03      
***********************************************************/ 
void Hall_Deduction_Mode_CPU(u32 hCardMoney)
{
    u8 status;
    u8 EEPROM_Array_buff[2];
    u16 Consume_Hall;
    u16 hConsumeAmount;         //消费费率
    u16 now_TIM_GetCounter = 0; 
    u16 last_TIM_GetCounter = 0;    //上一次计数器值
    u16 last_Update_value = 0;      //上一次更新的值
	u8 tt[2];
	u8 tmp;

    EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);      //读出消费费率；
    hConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    EEPROM_read_n(Consume_Hall_Addr,EEPROM_Array_buff,2);      //读出flash存储的消费霍尔量；
    Consume_Hall = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    
    hCardMoney = hCardMoney - hConsumeAmount;        //先扣费 
    Cut_payment_CPU(hCardMoney);      //扣费写卡

    Ht1621WrOneDat(12,0x01);      //显示计量
    Timer4_Run(1);                         //打开霍尔计数器
    Valve_ON; 
    status = MI_OK;                             //打开电磁阀
    for (;status == MI_OK;)         //卡不拿走一直显示；
    {
        if(T4_cnt>=65535)   //计数器溢出处理
        {
            T4_cnt = last_Update_value;
            last_TIM_GetCounter = 0;
            last_Update_value = 0;
        }
        now_TIM_GetCounter = T4_cnt;       //获取最新计数值
        last_Update_value = (now_TIM_GetCounter - last_TIM_GetCounter) + last_Update_value;  //累加更新的值
        last_TIM_GetCounter = now_TIM_GetCounter;     //保存本次读取的计时器值
        if(last_Update_value >= Consume_Hall)        //如果更新的计数到指定的值
        {
            hCardMoney = hCardMoney - hConsumeAmount; 
            Cut_payment_CPU(hCardMoney);      //扣费写卡
            if(hCardMoney == 0 || hCardMoney < hConsumeAmount)
            {   Valve_OFF;    
                break;    }           
            last_Update_value = 0;       //重新计数更新的值
        }
        Ht1621_DisplayNum(6,0,T4_cnt/10%10);
        Ht1621_DisplayNum(7,0,T4_cnt%10); 
        Disp_money(hCardMoney); 
        
		Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
		tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];
    }
    T4_cnt = 0;
    Timer4_Run(0);                         //打开霍尔计数器
    Valve_OFF; 
}

/*********************************************************
 * @brief      霍尔计量消费模式
 * @details    读出EEPROM存储的消费霍尔量；
 *             第二部验证：读取卡金额数据处理 CardMoney
 *             第三步验证： X1A=X17+X18+X19的累加和的值再取反（进位忽略）
 * @param[in]  CardMoney: 卡金额；Sectors：当前扇区
 * @return     
 * @date       2021-8-7 10:41:03
***********************************************************/ 
void Timer_Deduction_Mode_CPU(u32 tCardMoney)
{
    u8 status;
    u8 EEPROM_Array_buff[2];
    u16 Consume_Time;
    u16 tConsumeAmount;         //消费费率
	u8 tmp;
	u8 tt[2];
    //CardMoney = ReadCard_Money(CardData); 

    EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);      //读出消费费率；
    tConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    EEPROM_read_n(Consume_Time_Addr,EEPROM_Array_buff,2);      //读出flash存储的消费霍尔量；
    Consume_Time = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];

    tCardMoney = tCardMoney - tConsumeAmount;        //先扣费 
    Cut_payment_CPU(tCardMoney);      //扣费写卡

    Ht1621WrOneDat(11,0x01);   // 符号显示： 计时
    Timer3_Run(1);
    Valve_ON;  
    status=MI_OK;                            //打开电磁阀
    tmp = 0x9000;
    for (;tmp == 0x9000;)        //卡不拿走一直显示；
    {
        if(tCardMoney !=0 && Time_Num/100 >= Consume_Time)   // Time_Num/100-->最小100毫秒
        {
            tCardMoney = tCardMoney - tConsumeAmount;        //扣费 
            Cut_payment_CPU(tCardMoney);      //扣费写卡
            if(tCardMoney == 0 || tCardMoney < tConsumeAmount)
            {   Valve_OFF;    
                break;    } 
            Time_Num = 0;
        }
        Disp_money(tCardMoney);         
        Ht1621_DisplayNum(6,0,Time_Num/1000%10);
        Ht1621_DisplayNum(7,0,Time_Num/100%10);

        Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
		tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];    
    }
    Timer3_Run(0);
    Time_Num = 0;
    Valve_OFF; 
}
















		if(!ArraySequal)   //如果块0 块2不数据相等
		{
			if(Data_validation(BinaryFile_data) != MI_OK)
			{	
				if(Data_validation(BinaryFile_data_b) != MI_OK)
				{	
					Error_flag++;
					if(Error_flag==16)
					{	
						Error_flag=0;
						CardDat_Error(4,'c');  False_Alarm();   return	;}
				// 块2 数据验证通过; 读取块2 金额
				CardMoney = (u32)BinaryFile_data_b[4]<<16 | (u32)BinaryFile_data_b[3]<<8 | BinaryFile_data_b[2] ;
			}
			else if(Data_validation(BinaryFile_data_b) != MI_OK)  //如果块0 数据验证通过； 块2 验证不通过，直接取块0的值
			{  
				CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2];    
			}
			//块0 块2 数据验证通， 做金额对比，取金额最小的值；
			else{
				CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2];
				CardMoney2 = (u32)BinaryFile_data_b[4]<<16 | (u32)BinaryFile_data_b[3]<<8 | BinaryFile_data_b[2];  //移位要加上强制转换
				if(CardMoney > CardMoney2)  CardMoney = CardMoney2;
			}
		}
		else   //块0 块2数据相等， 直接取块0 的值
		{   
			if(Data_validation(BinaryFile_data) != MI_OK)
			{   
				Error_flag++;
				if(Error_flag==16)
				{	
					Error_flag=0;
					CardDat_Error(4,'c');  False_Alarm();   return	;}
			}
			CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2] ;  //移位要加上强制转换
		}
		Disp_money_CPU(CardMoney);