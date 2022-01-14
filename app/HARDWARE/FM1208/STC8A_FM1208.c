/*---------------------------------------------------------------------*/
/* ---       ���տƼ� FM1208 CPU�� ��������                       -------*/
/* ��ע�⣡     ���ļ�����Ϊ�߶ȱ��ܣ�                                    */
/*---------------------------------------------------------------------*/

#include "STC8A_FM1208.h"
#include "STC8A_RC522.h"
#include "STC8A_CardProc.h"
#include "STC8A_EEPROM.h"
#include "des.h"
#include "string.h"
#include "STC8A_HT1621.h"
#include "STC8A_Delay.h"
#include "STC8A_Timer.h"

u8 xdata g_ucTempbuf[MAXRLEN];  //��������
extern u8 Card_ID[4];
extern u8 Error_flag;
extern u32 Time_Num;
extern u32 T4_cnt;               //��ʱ��ֵ
u8 Write_Count;

void CPUCardProsing()
{
	u8 tt[2],i;
	u8 Random_array[4];
	u8 CardKey_Buff[6];        //����Կ���棻
	u8 BinaryFile_data[16];
	u8 BinaryFile_data_b[16];
	u32 CardMoney;
	u32 CardMoney2;
	u16 uConsumeAmount;
	u8 EEPROM_Array_buff[2];      //���ѷ��ʻ���
	u8 DeductionMode;          //����ģʽ

	u8 License_NUM[2];
	u8 ArraysEqual;
	if(CardReset(g_ucTempbuf,tt) != MI_OK)	//ATS  
    {	Error_flag++;
		if(Error_flag==16)
		{	CardDat_Error(5,'a'); 	False_Alarm();   Error_flag=0;}
		printf("CardReset error! \r\n");    return;	}	
		
	if(Choose_MF_File() != MI_OK)
	{	Error_flag++;
		if(Error_flag==16)
		{	CardDat_Error(5,'b'); 	False_Alarm();   Error_flag=0;}
		printf("MF_File error! \r\n");		return;	}

	EEPROM_read_n(CardKEY_Addr,CardKey_Buff,6);
	if(UserKey_Verify(CardKey_Buff) != MI_OK)
	{	Error_flag++;
		if(Error_flag==16)
		{	CardDat_Error(5,'c'); 	False_Alarm();   Error_flag=0;}
		printf("UserKey error! \r\n");	return; }	
	
	memset(Random_array, 0x00, sizeof(Random_array));
	if(Get_Random(Random_array)!= MI_OK)
	{	printf("Get_Random error! \r\n");	return; }	
	if(Verify_DES(Random_array,CardKey_Buff) != MI_OK)
	{	
		Error_flag++;
		if(Error_flag==16)
		{	CardDat_Error(5,'d'); 	False_Alarm();   Error_flag=0;}
		printf("Verify_DES error! \r\n");
		return	;
	}
	EEPROM_read_n(LicenseNUM_Addr,License_NUM,2);
	read_BinaryFile(0x86,BinaryFile_data,0x10);
	if(Data_validation(BinaryFile_data) != MI_OK)
	{	
		Error_flag++;
		if(Error_flag==16)
		{	CardDat_Error(5,'e'); 	False_Alarm();   Error_flag=0;}
		printf("Data_validation error! \r\n");  return	;}
	if(License_NUM[0] == BinaryFile_data[2] && License_NUM[1] == BinaryFile_data[3])
	{
		read_BinaryFile(0x87,BinaryFile_data,0x10);
		read_BinaryFile(0x88,BinaryFile_data_b,0x10);
		ArraysEqual = 1;
		for(i=0; i<16 ;i++)
		{
			if(BinaryFile_data_b[i] != BinaryFile_data[i])
			{
				ArraysEqual = 0;
				break;		 
			}
		}
		if(!ArraysEqual)   //�����0 ��2���������
		{
			printf("Arrays no Equal !"); 
			if(Data_validation(BinaryFile_data) != MI_OK)
			{	
				if(Data_validation(BinaryFile_data_b) != MI_OK)
				{	CardDat_Error(4,'e');  False_Alarm();   return	;}
				// ��2 ������֤ͨ��; ��ȡ��2 ���
				CardMoney = (u32)BinaryFile_data_b[4]<<16 | (u32)BinaryFile_data_b[3]<<8 | BinaryFile_data_b[2] ;
				Write_Count = BinaryFile_data_b[13];
			}
			else if(Data_validation(BinaryFile_data_b) != MI_OK)  //�����0 ������֤ͨ���� ��2 ��֤��ͨ����ֱ��ȡ��0��ֵ
			{  
				CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2];  
				Write_Count = BinaryFile_data[13];  
			}
			//��0 ��2 ������֤ͨ�� �����Աȣ�ȡ�����С��ֵ��
			else{
				CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2];
				CardMoney2 = (u32)BinaryFile_data_b[4]<<16 | (u32)BinaryFile_data_b[3]<<8 | BinaryFile_data_b[2];  //��λҪ����ǿ��ת��
				if(CardMoney > CardMoney2)  
				{	
					CardMoney = CardMoney2;
					Write_Count = BinaryFile_data_b[13];
				}
				else Write_Count = BinaryFile_data[13];
			}
		}
		else   //��0 ��2������ȣ� ֱ��ȡ��0 ��ֵ
		{   
			if(Data_validation(BinaryFile_data) != MI_OK)
			{   
				CardDat_Error(4,'c'); False_Alarm();
				return;  
			}
			CardMoney = (u32)BinaryFile_data[4]<<16 | (u32)BinaryFile_data[3]<<8 | BinaryFile_data[2] ;  //��λҪ����ǿ��ת��
			Write_Count = BinaryFile_data[13];
		}
		Disp_money_CPU(CardMoney);
		delay_ms(500);   
		if(CardMoney>=99999)              //���ѽ���Ƿ�������ֹ��
        {
			Disp_money(CardMoney,0);
			Ht1621WrOneDat(11,0x0A);    
			Ht1621WrOneDat(12,0x00);   
			Ht1621WrOneDat(17,0x0E);    
			Ht1621WrOneDat(13,0x00);Ht1621WrOneDat(14,0x00);          
        	Ht1621WrOneDat(15,0x00);Ht1621WrOneDat(16,0x00);          
			False_Alarm();
			delay_ms(300);
			memset(g_ucTempbuf, 0x00, 8);

			for (;Get_Random(Random_array) == MI_OK;)		//��������һֱ��ʾ
			{	} 
            return;
        }
		EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);     
		uConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
		if(CardMoney == 0 || (CardMoney < uConsumeAmount))  
        {
			Disp_money(CardMoney,0);
			Ht1621WrOneDat(11,0x08);    // ������ʾ��Ԫ  ��
			Ht1621WrOneDat(12,0x00);    // ������ʾ������
			Ht1621WrOneDat(17,0x0E);    // ������ʾ������ -- ͣ
			Ht1621WrOneDat(13,0x00);Ht1621WrOneDat(14,0x00);          
        	Ht1621WrOneDat(15,0x00);Ht1621WrOneDat(16,0x00);          
			False_Alarm();
			delay_ms(300);

            for (;Get_Random(Random_array) == MI_OK;)		//��������һֱ��ʾ
			{	} 
			return;
		}
		EEPROM_read_n(DeductionMode_Addr,&DeductionMode,1);
        if(DeductionMode == 0x60)        //�����������ģʽ
        {
            Hall_Deduction_Mode_CPU(&CardMoney);
        }
        else if(DeductionMode == 0x30)
        {
            Timer_Deduction_Mode_CPU(&CardMoney);
        }
	}
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
void Hall_Deduction_Mode_CPU(u32 *hCardMoney)
{
	u16 hConsumeAmount;         //���ѷ���
	u16 Consume_Hall;
	u8 EEPROM_Array_buff[2];
	u16 last_Update_value = 0;      //��һ�θ��µ�ֵ
	u16 last_TIM_GetCounter = 0;    //��һ�μ�����ֵ
	u16 now_TIM_GetCounter = 0; 
	u8 Random_array[4];

    EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);      //�������ѷ��ʣ�
    hConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    EEPROM_read_n(Consume_Hall_Addr,EEPROM_Array_buff,2);      //����flash�洢�����ѻ�������
    Consume_Hall = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];

	*hCardMoney = *hCardMoney - hConsumeAmount;        //�ȿ۷� 	
	Cut_payment_CPU(hCardMoney);      //�۷�д��

	Ht1621WrOneDat(12,0x01);      //��ʾ����
	Timer4_Run(1);     
	Valve_ON;                     //�򿪻���������
	for (;Get_Random(Random_array) == MI_OK;)		//��������һֱ��ʾ
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
			*hCardMoney = *hCardMoney - hConsumeAmount; 
            Cut_payment_CPU(hCardMoney);      //�۷�д��
			if(*hCardMoney == 0 || *hCardMoney < hConsumeAmount)
            {   Valve_OFF;    
                break;    }           
            last_Update_value = 0;       //���¼������µ�ֵ
		}
		Disp_money(*hCardMoney,0);
		Ht1621_DisplayNum(6,0,T4_cnt/10%10);
        Ht1621_DisplayNum(7,0,T4_cnt%10); 
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
void Timer_Deduction_Mode_CPU(u32 *tCardMoney)
{
	u8 EEPROM_Array_buff[2];
	u16 tConsumeAmount;         //���ѷ���
	u16 Consume_Time;
	u8 Card_Data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	u8 Random_array[4];

	EEPROM_read_n(ConsumeAmount_Addr,EEPROM_Array_buff,2);      //�������ѷ��ʣ�
    tConsumeAmount = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];
    EEPROM_read_n(Consume_Time_Addr,EEPROM_Array_buff,2);      //����flash�洢�����ѻ�������
    Consume_Time = (u16)EEPROM_Array_buff[0]<<8 | EEPROM_Array_buff[1];

	*tCardMoney = *tCardMoney - tConsumeAmount;   
	Cut_payment_CPU(tCardMoney);
	
	Ht1621WrOneDat(12,0x00);      //����ʾ����
	Ht1621WrOneDat(11,0x01);   // ������ʾ�� ��ʱ
	Timer3_Run(1);
    Valve_ON;  
	for (;Get_Random(Random_array) == MI_OK;)		//��������һֱ��ʾ
	{	
		if(*tCardMoney !=0 && Time_Num/100 >= Consume_Time)   // Time_Num/100-->��С100����
        {
            *tCardMoney = *tCardMoney - tConsumeAmount;        //�۷� 
            Cut_payment_CPU(tCardMoney);      //�۷�д��
            if(*tCardMoney == 0 || *tCardMoney < tConsumeAmount)
            {   Valve_OFF;    
                break;    } 
            Time_Num = 0;	  
        }
		Disp_money(*tCardMoney,1);
		Ht1621_DisplayNum(6,0,Time_Num/1000%10);
        Ht1621_DisplayNum(7,0,Time_Num/100%10);
	} 
	Timer3_Run(0);
    Time_Num = 0;
    Valve_OFF; 
}
/*********************************************************
 * @brief      �۷�д��  
 * @param[in]  money��  
***********************************************************/ 
void Cut_payment_CPU(u32 *money)
{
	u8 i,tt[2];
	u8 sum=0,sum1=0;
    u8 Card_Data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	u8 Card_Data_write[21];
	char status;
	u8 command[5] = {0x00,0xD6,0x87,0x00,0x10};

    Card_Data[4] = (u8)(((u8)(*money/65535)) & 0xFF);
    Card_Data[3] = (u8)(((u8)(*money/256)) & 0xFF);
    Card_Data[2] = ((u8)(*money) & 0xff);

    Card_Data[1] = (Card_Data[2]+Card_Data[3]+Card_Data[4]) & 0xff;//X01=X02+X03+X04���ۼӺ͵�ֵ����λ���ԣ�
    Card_Data[5] = (~(Card_Data[2]+Card_Data[3]+Card_Data[4])) & 0xff; // ��X05=X02+X03+X04���ۼӺ͵�ֵ��ȡ������λ���ԣ� 
    Card_Data[13] = Write_Count+1;	Card_Data[14] = Write_Count+1;
    for(i=1;i<15;i++)
    {
        sum += Card_Data[i];
        sum1 ^= Card_Data[i]; 
    }
    Card_Data[15] = ~(sum+Card_ID[0]+Card_ID[1]+Card_ID[2]+Card_ID[3]+0x5A) & 0xFF; 
    Card_Data[0] = sum1^Card_ID[0]^Card_ID[1]^Card_ID[2]^Card_ID[3]^0x5A;    // X1F=X11����X1E�ۼӺ͵�ֵ��ȡ������λ���ԣ�
 
	command[2] = 0x88;
	for(i=0;i<5;i++)
    { Card_Data_write[i] = command[i]; }
	for(i=0;i<16;i++)
    { Card_Data_write[i+5] = Card_Data[i]; }

    status = MI_OK;
	status = Pcd_Cmd(Card_Data_write, sizeof(Card_Data_write), g_ucTempbuf, tt);
	if (status != MI_OK) 
	{	printf("Data_write %b02X error! \r\n",command[2]);  return ;}

	read_BinaryFile(0x88,Card_Data,0x10);
	if(Data_validation(Card_Data) != MI_OK)
	{ return ;}

	command[2] = 0x87;
	for(i=0;i<5;i++)
    { Card_Data_write[i] = command[i]; }
	for(i=0;i<16;i++)
    { Card_Data_write[i+5] = Card_Data[i]; }
    status = MI_OK;
	status = Pcd_Cmd(Card_Data_write, sizeof(Card_Data_write), g_ucTempbuf, tt);
	if (status != MI_OK) 
	{	printf("Data_write %b02X error! \r\n",command[2]);  return ;}

	read_BinaryFile(0x87,Card_Data,0x10);
	if(Data_validation(Card_Data) != MI_OK)
	{ return ;}
	
	*money = (u32)Card_Data[4]<<16 | (u32)Card_Data[3]<<8 | Card_Data[2] ;    //*money =
}
/*********************************************************
 * @brief      ��ʾ�����  
 * @param[in]  money��  
***********************************************************/ 
void Disp_money_CPU(u32 money)
{
    Ht1621WrOneDat(18,0x08);    // ������ʾ����� 
    Ht1621_DisplayNum(3,1,money/100%10);
    Ht1621_DisplayNum(4,0,money/10%10);
    Ht1621_DisplayNum(5,0,money%10);
    if(money <= 99999 && money >= 10000)
    {
        Ht1621_DisplayNum(1,0,money/10000%10);
        Ht1621_DisplayNum(2,0,money/1000%10);
    }
    if(money <= 9999 && money >= 1000)
    {
        Ht1621WrOneDat(9,0x00); Ht1621WrOneDat(10,0x00); //1λ����ʾ  
        Ht1621_DisplayNum(2,0,money/1000%10);
    }
    if(money <= 999 )
    {
        Ht1621WrOneDat(9,0x00);Ht1621WrOneDat(10,0x00);    //1λ����ʾ  
        Ht1621WrOneDat(7,0x00);Ht1621WrOneDat(8,0x00);     //2λ����ʾ  
    }
}
/*********************************************************
 * @brief      ����У��
 * @details    
 * @param[in]  
 * @date       2021-9-14 22:15:20
***********************************************************/
char Data_validation(u8 *vData)
{
	u8 i;
	u8 Sum_NOT, Sum_XOR;        //��ȡ���������

	Sum_NOT = 0; Sum_XOR = 0;
	for(i=1; i<15; i++)      
	{   Sum_NOT += vData[i];    Sum_XOR ^= vData[i]; }  
	Sum_NOT = ~(Sum_NOT +Card_ID[0] +Card_ID[1] +Card_ID[2] +Card_ID[3] +0x5A);
	Sum_XOR = Sum_XOR ^Card_ID[0] ^Card_ID[1] ^Card_ID[2] ^Card_ID[3]^0x5A;
	if(((Sum_NOT & 0xFF) != vData[15]) && (Sum_XOR != vData[0]))
	{	return MI_ERR; }
	if(vData[5] == 0xBB)
	{
		if(vData[6] == (~(vData[1] +vData[2] +vData[3] +vData[4] +vData[5]) & 0xFF))
		{
			if(vData[10] == (~(vData[7] +vData[8] +vData[9]) & 0xFF))
			{	return MI_OK;}
		}
		if(vData[1] != ((vData[2] +vData[3] +vData[4] ) & 0xFF))
		{    return MI_ERR;}
		if(vData[5] != (~(vData[2] +vData[3] +vData[4]) & 0xFF))  // X1F=X11����X1E�ۼӺ͵�ֵ��ȡ������λ���ԣ�
		{    return MI_ERR;}

		return MI_ERR;
	}	
	if(vData[1] != ((vData[2] +vData[3] +vData[4] ) & 0xFF))
		{    return MI_ERR;}
	if(vData[5] != ((~(vData[2] +vData[3] +vData[4])) & 0xFF))  // X1F=X11����X1E�ۼӺ͵�ֵ��ȡ������λ���ԣ�
		{    return MI_ERR;}

	return MI_OK;
}
/*********************************************************
 * @brief      userkey ��֤
 * @details    
 * @param[in]  
 * @date       2021-9-14 22:15:20
***********************************************************/
char UserKey_Verify(u8 *UserKey)
{
	u8 i;
	u8 BinaryFile_data[32];
	u8 UserKey_10byte[10];
	u8 UserKey_6byte_Conver[6];

	read_BinaryFile(0x85,BinaryFile_data,0x20);	   //��֤         
	for(i=0; i<10; i++)
	{
		UserKey_10byte[i] = BinaryFile_data[i+16];
	}
	UserKey_6byte_Conver[0] = UserKey_10byte[3] ^ 0x01;
	UserKey_6byte_Conver[1] = UserKey_10byte[4] ^ 0x23;
	UserKey_6byte_Conver[2] = UserKey_10byte[5] ^ 0x45;
	UserKey_6byte_Conver[3] = UserKey_10byte[6] ^ 0x67;
	UserKey_6byte_Conver[4] = UserKey_10byte[7] ^ 0x89;
	UserKey_6byte_Conver[5] = UserKey_10byte[8] ^ 0x5A;
	for(i=0; i<6; i++)
	{
		if(UserKey_6byte_Conver[i]!= UserKey[i])
		{
			return MI_ERR;
		}	
	}
    if(UserKey_10byte[0] != (UserKey_10byte[1]^UserKey_10byte[2]^UserKey_10byte[3]^UserKey_10byte[4]^
							 UserKey_10byte[5]^UserKey_10byte[6]^UserKey_10byte[7]^UserKey_10byte[8]^
							 Card_ID[0]^Card_ID[1]^Card_ID[2]^Card_ID[3]))
	{	
		return MI_ERR;
	}
	if(UserKey_10byte[9] != ((UserKey_10byte[1]+UserKey_10byte[2]+UserKey_10byte[3]+UserKey_10byte[4]+
							 UserKey_10byte[5]+UserKey_10byte[6]+UserKey_10byte[7]+UserKey_10byte[8]+
							 Card_ID[0]+Card_ID[1]+Card_ID[2]+Card_ID[3]) &0xff) )
	{	
		return MI_ERR;
	}
	return MI_OK;
}
/*********************************************************
 * @brief      ��ȡ�������ļ�
 * @details    
 * @param[in]  
 * @date       2021-9-14 22:15:20
***********************************************************/
void read_BinaryFile(u8 DFile,u8* pDataOut,u8 Out_Len)
{
	u8 tt[2],i;
	u8 command[5] = {0x00,0xB0,0x85,0x00,0x10};
	char status;

	memset(g_ucTempbuf, 0x00, MAXRLEN);
	command[2] = DFile ;
	command[4] = Out_Len ;
	status = Pcd_Cmd(command, 5, g_ucTempbuf, tt);
	if (status != MI_OK) 
	{	return ;}
	//printf("DFile %b02X: ",DFile);             //��ʾ����ֵ
	for(i=0; i<tt[0]-2; i++)
    {  
		//printf("%b02X",g_ucTempbuf[i+2]);
		pDataOut[i] = g_ucTempbuf[i+2];
	}
	//printf("\r\n");
}
/*********************************************************
 * @brief      DES ����
 * @details    
 * @param[in]  
 * @date       2021-9-14 22:15:20
***********************************************************/
char Verify_DES(u8 *Random,u8 *key)
{
	u8 tt[2],i;
	u8 Cleartext_tab[8];
	u8 key_8byte[8];
	u8 cipher1[8];
	u8 command[13] = {0x00, 0x82, 0x00, 0x14, 0x08, 0,0,0,0, 0,0,0,0};
	char status;

    for(i=0; i<4; i++)       //�����+0000 0000
	{	
		Cleartext_tab[i] = Random[i];
		Cleartext_tab[i+4] = 0x00;
	}
	for(i=0; i<6; i++){
		key_8byte[i] = key[i];
	}
	key_8byte[6] = 0X5A;	key_8byte[7] = 0X5A;

	cipher1[0]=Card_ID[2]; cipher1[1]=Card_ID[3]; cipher1[2]=0X5A;       
	cipher1[3]=0X5A;   	   cipher1[4]=0X5A;       cipher1[5]=0X5A; 
	cipher1[6]=Card_ID[0]; cipher1[7]=Card_ID[1];

	for(i=0; i<8; i++)
	{	key_8byte[i] = key_8byte[i] ^ cipher1[i]; }

	des(Cleartext_tab, key_8byte, 0, cipher1);   //����
	//printf("DES: ");             //��ʾ����ֵ
    for(i=0; i<8; i++)
    {  
		//printf("%b02X",cipher1[i]);
	    command[i+5] = cipher1[i];
	}
    //printf("\r\n");

	status = Pcd_Cmd(command, 13, g_ucTempbuf, tt);
	if (status != MI_OK) 
	{	return MI_ERR;}
    if((g_ucTempbuf[2]<<8 | g_ucTempbuf[3]) != 0x9000)
    { 	return MI_ERR;	}
	//printf("%b02X%b02X",g_ucTempbuf[2],g_ucTempbuf[3]);
	return MI_OK;
}

/*********************************************************
 * @brief      ��ȡ�����
 * @details    
 * @param[in]  
 * @date       2021-9-14 22:15:20
***********************************************************/ 
char Get_Random(u8 *pRandom_array)
{
	u8 tt[2],i;
	u8 command[5]={0x00, 0x84, 0x00, 0x00, 0x04};   //��ȡ�����
    char status;

	status = Pcd_Cmd(command, 5, g_ucTempbuf, tt);
	if (status != MI_OK) 
	{	return MI_ERR; }
	if((g_ucTempbuf[6]<<8 | g_ucTempbuf[7]) != 0x9000)
	{	return MI_ERR;	}

	for(i=0; i<4; i++)
    {  
		pRandom_array[i] = g_ucTempbuf[i+2];
	}
	memset(g_ucTempbuf, 0x00, sizeof(tt[0]+2));
	return MI_OK;
    // printf("Get_Random: ");             //��ʾ����ֵ
    // for(i=0; i<tt[0]; i++)
    // {  
	// 	printf("%b02X",g_ucTempbuf[i+2]);
	// }
}
/*********************************************************
 * @brief      ѡ��CPU��MF�ļ�Ŀ¼
 * @details    
 * @param[in]  
 * @date       2021-9-14 22:15:20
***********************************************************/ 
char Choose_MF_File()
{
	u8 status = MI_OK;
	u8 tt[2];
	u8 command[7] = {0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00};

	status = Pcd_Cmd(command, 7, g_ucTempbuf, tt);
	if(status != MI_OK)	
        return MI_ERR;						//����	
	if((g_ucTempbuf[tt[0]]<<8 | g_ucTempbuf[tt[0]+1]) != 0x9000)
		return MI_ERR;

	return MI_OK;	
	// printf("MF_File: ");
    // for(i=0; i<tt[0]; i++)
    // {  printf("%b02X",g_ucTempbuf[i]);  }
    // printf("\r\n");
}


