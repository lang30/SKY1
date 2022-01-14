/*---------------------------------------------------------------------*/
/* ---       ���տƼ� FM1208 CPU�� ��������                       -------*/
/* ��ע�⣡     ���ļ�����Ϊ�߶ȱ��ܣ�                                    */
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
u8 CardData_1[16];          //����1���ݻ���
//////////////////CPU����ָ��////////////////////////////////
extern u32 T4_cnt;
extern u32 Time_Num;
extern u8 Card_ID[4];
extern u8 DeductionMode;          //����ģʽ
unsigned char code Choose_MF_File[7]={0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00};   //ѡ��MF�ļ�
unsigned char code Get_Random[5]={0x00, 0x84, 0x00, 0x00, 0x04};   //��ȡ�����
unsigned char Verify_DES[13]={0x00, 0x82, 0x00, 0x14, 0x08, 0,0,0,0, 0,0,0,0};   //��ȡ�����
unsigned char code read_DF16[5]={0x00,0xB0,0x86,0x00,0x10};
unsigned char code read_DF17[5]={0x00,0xB0,0x87,0x00,0x10};
unsigned char code read_DF18[5]={0x00,0xB0,0x88,0x00,0x10};
unsigned char code write_DF16[5]={0x00,0xD6,0x86,0x00,0x10};
unsigned char code write_DF17[5]={0x00,0xD6,0x87,0x00,0x10};
unsigned char code write_DF18[5]={0x00,0xD6,0x88,0x00,0x10};


/********************************************************************
 * @brief      CPU �������� 
 * @param[in]  
 * @return     
 * @note      
*********************************************************************/ 
void CPUCardProsing()
{
    u8 tt[2];
    u8 i;
    char status;
	unsigned char cipher1[8];	//���Ļ���
	u8 CardKey_Buff[6];        //����Կ���棻
	unsigned char key_tab[8];   //��Կ
	unsigned char Cleartext_tab[8];  //����
	unsigned char CARDid_5A[8];
	u32 CardMoney;
    u8 Sum_NOT, Sum_XOR;        //��ȡ���������
	u16 tmp;
	u16 uConsumeAmount;
	u8 EEPROM_Array_buff[2];      //���ѷ��ʻ���

    memset(g_ucTempbuf, 0x00, MAXRLEN);	        //����������0
    if(CardReset(g_ucTempbuf,tt)!=MI_OK)		  //CPU����λ����
	{  return; }
    printf("ATS:");
    for(i=0; i<tt[0]; i++)
    {
       printf("%b02X",g_ucTempbuf[i]); 
    }
    printf("\r\n");
    //////////// ѡ��MF �ļ� ///////////////////
    status = MI_OK;
    status=Pcd_Cmd(Choose_MF_File, 7, g_ucTempbuf, tt);
    if(status)	//1ʱ������
    {
        memset(g_ucTempbuf, 0x00, MAXRLEN);	//��0				
        printf("MF_File error! \r\n");
        return;						//����	
    }
    printf("MF_File: ");
    for(i=0; i<tt[0]; i++)
    {  printf("%b02X",g_ucTempbuf[i]);  }
    printf("\r\n");
    //////////////��ȡ����� //////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//��0	
    status = MI_OK;
    status=Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
    if(status)	//1ʱ������
    {		
        printf("Get_Random error! \r\n");
        return;						//����	
    }
    printf("Get_Random: ");             //��ʾ����ֵ
    for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
    printf("\r\n");
	//////////////DES ��֤///////////////////
	for(i=0; i<4; i++)
	{	
		Cleartext_tab[i] = g_ucTempbuf[i+2];      //��������� 0000 ��� DES ����
		Cleartext_tab[i+4] = 0x00;
	}	
    EEPROM_read_n(CardKEY_Addr,CardKey_Buff,6);      //��EEPROM�ж����洢����ԿCardKey_Buff��  
	for(i=0; i<6; i++){
		key_tab[i] = CardKey_Buff[i];
	}
	key_tab[6] = 0X5A;	key_tab[7] = 0X5A;       //β������ 5A5A ����8���ֽ�
    
    //  �ÿ���5A ���
	CARDid_5A[0]=Card_ID[2]; CARDid_5A[1]=Card_ID[3]; 
	CARDid_5A[2]=0X5A; CARDid_5A[3]=0X5A; CARDid_5A[4]=0X5A; CARDid_5A[5]=0X5A; 
	CARDid_5A[6]=Card_ID[0]; CARDid_5A[7]=Card_ID[1];

	for(i=0; i<8; i++)    
	{	key_tab[i] = key_tab[i] ^ CARDid_5A[i]; }    // 8�ֽ���Կ ��� ����5A �������  �õ� ��Կ

	des(Cleartext_tab, key_tab, 0, cipher1);    //����
    printf("DES: ");             //��ʾ����ֵ
    for(i=0; i<8; i++)
    {  printf("%b02X",cipher1[i]);
	   Verify_DES[i+5] = cipher1[i];
	}
    printf("\r\n");

	memset(g_ucTempbuf, 0x00, MAXRLEN);	//��0	
    status = MI_OK;
    status=Pcd_Cmd(Verify_DES, sizeof(Verify_DES), g_ucTempbuf, tt);  //��֤DES 
    if(status)	//1ʱ������
    {
		printf("Verify_DES error! \r\n");
        return;						//����
	}
	for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
	printf("\r\n");
    ///////////////////��ȡ 16 �ļ� ////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//��0	
    status = MI_OK;
    status=Pcd_Cmd(read_DF16, sizeof(read_DF16), g_ucTempbuf, tt);
	printf("DF16: ");             //��ʾ����ֵ
	for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
	printf("\r\n");
    ///////////////////��ȡ 17 �ļ� ////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//��0	
    status = MI_OK;
    status=Pcd_Cmd(read_DF17, sizeof(read_DF17), g_ucTempbuf, tt);
	printf("DF17: ");             //��ʾ����ֵ
	for(i=0; i<tt[0]; i++)
    {  
		printf("%b02X",g_ucTempbuf[i+2]);
	}
	printf("\r\n");
    ///////////////////��ȡ 18 �ļ� ////////////////
	memset(g_ucTempbuf, 0x00, MAXRLEN);	//��0	
    status = MI_OK;
    status=Pcd_Cmd(read_DF18, sizeof(read_DF18), g_ucTempbuf, tt);
	printf("DF18: ");             //��ʾ����ֵ
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
	if(CardData_1[5] != ((~(CardData_1[2] +CardData_1[3] +CardData_1[4])) & 0xFF))  // X1F=X11����X1E�ۼӺ͵�ֵ��ȡ������λ���ԣ�
		{CardDat_Error(3,4);  False_Alarm();    return;}
	CardMoney = (u32)CardData_1[4]<<16 | (u32)CardData_1[3]<<8 | CardData_1[2] ;

	tmp = 0x9000;
	while (tmp == 0x9000)
	{
		tmp = 0;
		memset(g_ucTempbuf, 0x00, 8);
		Disp_money(CardMoney);
		delay_ms(500);   //ˢ����ʱ�����忨����
		if(CardMoney>=99999)              //���ѽ���Ƿ�������ֹ��
        {
			tmp = 0x9000;
            for (;tmp == 0x9000;)        //��������һֱ��ʾ��
            {
                Disp_money(CardMoney);
                Ht1621WrOneDat(11,0x0A);    // ������ʾ��Ԫ  ��
                Ht1621WrOneDat(12,0x02);    // ������ʾ��Ԫ  ����
                Ht1621WrOneDat(17,0x08);    // ������ʾ������
                False_Alarm();
                delay_ms(300);
				memset(g_ucTempbuf, 0x00, 8);
                Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
				tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];
            }
            return;
        }
		EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);     //��ȡ�趨�����ѷ���,
        uConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1]; 
        if(CardMoney == 0 || (CardMoney < uConsumeAmount))  //������ʾ��
        {	
            tmp = 0x9000;
            for (;tmp == 0x9000;)        //��������һֱ��ʾ��
            {
                Ht1621WrOneDat(17,0x06); //С��Ԥ�ۿ��ʾͣ
                Disp_money(CardMoney);
                delay_ms(300);
                Ht1621Clear(0,32); // - - HT1621 ����
                delay_ms(300);
                Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
				tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];
            }
            return;
        } 
        EEPROM_read_n(DeductionMode_Addr,&DeductionMode,1);
        if(DeductionMode == 0x60)        //�����������ģʽ
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

    Card_Data[1] = (Card_Data[2]+Card_Data[3]+Card_Data[4]) & 0xff;//X01=X02+X03+X04���ۼӺ͵�ֵ����λ���ԣ�
    Card_Data[5] = (~(Card_Data[2]+Card_Data[3]+Card_Data[4])) & 0xff; // ��X05=X02+X03+X04���ۼӺ͵�ֵ��ȡ������λ���ԣ� 
    Card_Data[13] = CardData_1[13]+1;	Card_Data[14] = CardData_1[14]+1;
    for(i=1;i<15;i++)
    {
        sum += Card_Data[i];
        sum1 ^= Card_Data[i]; 
    }
    Card_Data[15] = ~(sum+Card_ID[0]+Card_ID[1]+Card_ID[2]+Card_ID[3]+0x5A) & 0xFF; 
    Card_Data[0] = sum1^Card_ID[0]^Card_ID[1]^Card_ID[2]^Card_ID[3]^0x5A;    // X1F=X11����X1E�ۼӺ͵�ֵ��ȡ������λ���ԣ�
    
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
 * @brief      ������������ģʽ
 * @details    ����EEPROM�洢�����ѻ�������
 *             �ڶ�����֤����ȡ��������ݴ��� CardMoney
 *             ��������֤�� X1A=X17+X18+X19���ۼӺ͵�ֵ��ȡ������λ���ԣ�
 * @param[in]  CardMoney: ����Sectors����ǰ����
 * @return     
 * @date       2021-8-7 10:41:03      
***********************************************************/ 
void Hall_Deduction_Mode_CPU(u32 hCardMoney)
{
    u8 status;
    u8 EEPROM_Array_buff[2];
    u16 Consume_Hall;
    u16 hConsumeAmount;         //���ѷ���
    u16 now_TIM_GetCounter = 0; 
    u16 last_TIM_GetCounter = 0;    //��һ�μ�����ֵ
    u16 last_Update_value = 0;      //��һ�θ��µ�ֵ
	u8 tt[2];
	u8 tmp;

    EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);      //�������ѷ��ʣ�
    hConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    EEPROM_read_n(Consume_Hall_Addr,EEPROM_Array_buff,2);      //����flash�洢�����ѻ�������
    Consume_Hall = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    
    hCardMoney = hCardMoney - hConsumeAmount;        //�ȿ۷� 
    Cut_payment_CPU(hCardMoney);      //�۷�д��

    Ht1621WrOneDat(12,0x01);      //��ʾ����
    Timer4_Run(1);                         //�򿪻���������
    Valve_ON; 
    status = MI_OK;                             //�򿪵�ŷ�
    for (;status == MI_OK;)         //��������һֱ��ʾ��
    {
        if(T4_cnt>=65535)   //�������������
        {
            T4_cnt = last_Update_value;
            last_TIM_GetCounter = 0;
            last_Update_value = 0;
        }
        now_TIM_GetCounter = T4_cnt;       //��ȡ���¼���ֵ
        last_Update_value = (now_TIM_GetCounter - last_TIM_GetCounter) + last_Update_value;  //�ۼӸ��µ�ֵ
        last_TIM_GetCounter = now_TIM_GetCounter;     //���汾�ζ�ȡ�ļ�ʱ��ֵ
        if(last_Update_value >= Consume_Hall)        //������µļ�����ָ����ֵ
        {
            hCardMoney = hCardMoney - hConsumeAmount; 
            Cut_payment_CPU(hCardMoney);      //�۷�д��
            if(hCardMoney == 0 || hCardMoney < hConsumeAmount)
            {   Valve_OFF;    
                break;    }           
            last_Update_value = 0;       //���¼������µ�ֵ
        }
        Ht1621_DisplayNum(6,0,T4_cnt/10%10);
        Ht1621_DisplayNum(7,0,T4_cnt%10); 
        Disp_money(hCardMoney); 
        
		Pcd_Cmd(Get_Random, sizeof(Get_Random), g_ucTempbuf, tt);
		tmp = g_ucTempbuf[6]<<8 | g_ucTempbuf[7];
    }
    T4_cnt = 0;
    Timer4_Run(0);                         //�򿪻���������
    Valve_OFF; 
}

/*********************************************************
 * @brief      ������������ģʽ
 * @details    ����EEPROM�洢�����ѻ�������
 *             �ڶ�����֤����ȡ��������ݴ��� CardMoney
 *             ��������֤�� X1A=X17+X18+X19���ۼӺ͵�ֵ��ȡ������λ���ԣ�
 * @param[in]  CardMoney: ����Sectors����ǰ����
 * @return     
 * @date       2021-8-7 10:41:03
***********************************************************/ 
void Timer_Deduction_Mode_CPU(u32 tCardMoney)
{
    u8 status;
    u8 EEPROM_Array_buff[2];
    u16 Consume_Time;
    u16 tConsumeAmount;         //���ѷ���
	u8 tmp;
	u8 tt[2];
    //CardMoney = ReadCard_Money(CardData); 

    EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);      //�������ѷ��ʣ�
    tConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    EEPROM_read_n(Consume_Time_Addr,EEPROM_Array_buff,2);      //����flash�洢�����ѻ�������
    Consume_Time = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];

    tCardMoney = tCardMoney - tConsumeAmount;        //�ȿ۷� 
    Cut_payment_CPU(tCardMoney);      //�۷�д��

    Ht1621WrOneDat(11,0x01);   // ������ʾ�� ��ʱ
    Timer3_Run(1);
    Valve_ON;  
    status=MI_OK;                            //�򿪵�ŷ�
    tmp = 0x9000;
    for (;tmp == 0x9000;)        //��������һֱ��ʾ��
    {
        if(tCardMoney !=0 && Time_Num/100 >= Consume_Time)   // Time_Num/100-->��С100����
        {
            tCardMoney = tCardMoney - tConsumeAmount;        //�۷� 
            Cut_payment_CPU(tCardMoney);      //�۷�д��
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
















		if(!ArraySequal)   //�����0 ��2���������
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
				// ��2 ������֤ͨ��; ��ȡ��2 ���
				CardMoney = (u32)BinaryFile_data_b[4]<<16 | (u32)BinaryFile_data_b[3]<<8 | BinaryFile_data_b[2] ;
			}
			else if(Data_validation(BinaryFile_data_b) != MI_OK)  //�����0 ������֤ͨ���� ��2 ��֤��ͨ����ֱ��ȡ��0��ֵ
			{  
				CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2];    
			}
			//��0 ��2 ������֤ͨ�� �����Աȣ�ȡ�����С��ֵ��
			else{
				CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2];
				CardMoney2 = (u32)BinaryFile_data_b[4]<<16 | (u32)BinaryFile_data_b[3]<<8 | BinaryFile_data_b[2];  //��λҪ����ǿ��ת��
				if(CardMoney > CardMoney2)  CardMoney = CardMoney2;
			}
		}
		else   //��0 ��2������ȣ� ֱ��ȡ��0 ��ֵ
		{   
			if(Data_validation(BinaryFile_data) != MI_OK)
			{   
				Error_flag++;
				if(Error_flag==16)
				{	
					Error_flag=0;
					CardDat_Error(4,'c');  False_Alarm();   return	;}
			}
			CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2] ;  //��λҪ����ǿ��ת��
		}
		Disp_money_CPU(CardMoney);