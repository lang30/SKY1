/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include "STC8A_EEPROM.h"

extern uint32 MAIN_Fosc;

u8 EEPROM_Read_Buff[6];
//========================================================================
// ����: void	ISP_Disable(void)
// ����: ��ֹ����ISP/IAP.
// ����: non.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void	DisableEEPROM(void)
{
	IAP_CONTR = 0;			//��ֹIAP����
	IAP_CMD   = 0;			//ȥ��IAP����
	IAP_TRIG  = 0;			//��ֹIAP�����󴥷�
	IAP_ADDRH = 0xff;		//��0��ַ���ֽ�
	IAP_ADDRL = 0xff;		//��0��ַ���ֽڣ�ָ���EEPROM������ֹ�����
}

//========================================================================
// ����: void EEPROM_Trig(void)
// ����: ����EEPROM����.
// ����: none.
// ����: none.
// �汾: V1.0, 2014-6-30
//========================================================================
void EEPROM_Trig(void)
{
	F0 = EA;    //����ȫ���ж�
	EA = 0;     //��ֹ�ж�, ���ⴥ��������Ч
	IAP_TRIG = 0x5A;
	IAP_TRIG = 0xA5;                    //����5AH������A5H��IAP�����Ĵ�����ÿ�ζ���Ҫ���
																			//����A5H��IAP������������������
																			//CPU�ȴ�IAP��ɺ󣬲Ż����ִ�г���
	_nop_();
	_nop_();
	EA = F0;    //�ָ�ȫ���ж�
}

//========================================================================
// ����: void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u16 number)
// ����: ��ָ��EEPROM�׵�ַ����n���ֽڷ�ָ���Ļ���.
// ����: EE_address:  ����EEPROM���׵�ַ.
//       DataAddress: �������ݷŻ�����׵�ַ.
//       number:      �������ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u16 number)
{
	IAP_ENABLE();                           //���õȴ�ʱ�䣬����IAP��������һ�ξ͹�
	IAP_READ();                             //���ֽڶ���������ı�ʱ����������������
	do
	{
		IAP_ADDRH = EE_address / 256;       //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
		IAP_ADDRL = EE_address % 256;       //�͵�ַ���ֽ�
		EEPROM_Trig();                      //����EEPROM����
		*DataAddress = IAP_DATA;            //��������������
		EE_address++;
		DataAddress++;
	}while(--number);

	DisableEEPROM();
}

//========================================================================
// ����: void EEPROM_SectorErase(u16 EE_address)
// ����: ��ָ����ַ��EEPROM��������.
// ����: EE_address:  Ҫ����������EEPROM�ĵ�ַ.
// ����: non.
// �汾: V1.0, 2013-5-10
//========================================================================
void EEPROM_SectorErase(u16 EE_address)
{
	IAP_ENABLE();                       //���õȴ�ʱ�䣬����IAP��������һ�ξ͹�
	IAP_ERASE();                        //�����, ������������������ı�ʱ����������������
																			//ֻ������������û���ֽڲ�����512�ֽ�/������
																			//����������һ���ֽڵ�ַ����������ַ��
	IAP_ADDRH = EE_address / 256;       //��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
	IAP_ADDRL = EE_address % 256;       //��������ַ���ֽ�
	EEPROM_Trig();                      //����EEPROM����
	DisableEEPROM();                    //��ֹEEPROM����
	printf("EraseAddr��%04xH ",EE_address);
}

//========================================================================
// ����: void EEPROM_write_n(u16 EE_address,u8 *DataAddress,u16 number)
// ����: �ѻ����n���ֽ�д��ָ���׵�ַ��EEPROM.
// ����: EE_address:  д��EEPROM���׵�ַ.
//       DataAddress: д��Դ���ݵĻ�����׵�ַ.
//       number:      д����ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void EEPROM_write_n(u16 EE_address,u8 *DataAddress,u16 number)
{
	IAP_ENABLE();                       //���õȴ�ʱ�䣬����IAP��������һ�ξ͹�
	IAP_WRITE();                        //�����, ���ֽ�д����
	do
	{
		IAP_ADDRH = EE_address / 256;     //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
		IAP_ADDRL = EE_address % 256;     //�͵�ַ���ֽ�
		IAP_DATA  = *DataAddress;         //�����ݵ�IAP_DATA��ֻ�����ݸı�ʱ����������
		EEPROM_Trig();                    //����EEPROM����
		EE_address++;                     //��һ����ַ
		DataAddress++;                    //��һ������
	}while(--number);                   //ֱ������
	DisableEEPROM();
}
//========================================================================
// ����: void EEPROM_write_n_ex(u16 addr, u8 *dat, u16 num)
// ����: flash��Ҫ��������д�룬�������ǰ�0���1�������ǰ����������� �˺����Ĺ�����д�����ݶ�������������ַ�����ݣ�
		 
// ����: addr:  д��EEPROM���׵�ַ.
//       dat:  д��Դ���ݵĻ�����׵�ַ.
//       number:      д����ֽڳ���.
// ����: num.
// �汾: V1.0, 2021-8-4 11:20:40
//========================================================================
u8 xdata eeprom_buf[512];//�����������ݵĻ���
void EEPROM_write_n_ex(u16 addr, u8 *dat, u16 num)
{
    u16 num_yu;         	//ʣ��Ҫд�����ݵ�������
    u16 area;           	//����������
    u16 area_addr_yu;   	//�������µĵ�ַ������
    u16 area_start_addr;	//������ʼ��ַ��
    u16 area_stop_addr; 	//����������ַ��
    u16 addr_offset;    	//��ַƫ�ơ�
    u16 dat_offset = 0;   	//����ƫ�ơ�
    u16 address;        	//������ַ��
    u16 i;              	//��ʱ������
	u8 read_buf[6];
    address = addr;       	//����ַ������ֵ��������ַ��
    num_yu = num;         	//������������ֵΪʣ��������
	EEPROM_read_n(addr, read_buf, num);  //��ȡҪд���ַ������
	for(i=0; i< num; i++)	
	{   if(read_buf[i] != 0xff)		break;	 }     //�жϵ�ַд��ǰ�������Ƿ�ΪȫΪff�������ΪFF���Ͳ�������д�룡
	if(i<num )  {
		do{
			area = address >> 9;                       	//��ȡ������
			area_start_addr = (area << 9);           	//��ȡ������ʼ�ĵ�ַ��
			area_stop_addr = (area << 9) + 512;       	//��ȡ����ĩβ�ĵ�ַ��
			area_addr_yu = area_stop_addr - address; 	//ĩβ��ַ��������ַ���õ���ַʣ��������
			if(num_yu <= area_addr_yu)			//���Ҫд�������ڵ�ַʣ��������˵�����ÿ�������
			{             	
				EEPROM_read_n(area_start_addr, eeprom_buf, 512);  //��ԭ�������ݶ�������
				EEPROM_SectorErase(area_start_addr);	//���������
				addr_offset = address - area_start_addr;	//��ȡ��ַƫ�ơ�
				for(i=0; i < num_yu; i++)			//�޸����ݡ�
				{   eeprom_buf[addr_offset + i] = dat[dat_offset+i];	}
				EEPROM_write_n(area_start_addr, eeprom_buf, 512);
				break;                      	//д�������ѭ����
			}
			else 								
			{                                  	//���Ҫд�������ڵ�ַʣ������
				EEPROM_read_n(area_start_addr, eeprom_buf, 512);  	//��ԭ�������ݶ�������
				EEPROM_SectorErase(area_start_addr);	//���������
				addr_offset=address-area_start_addr;	//��ȡ��ַƫ�ơ�
				for(i=0;i<area_addr_yu;i++)				//�޸����ݡ�
				{   eeprom_buf[addr_offset+i]=dat[dat_offset+i];	}
				EEPROM_write_n(area_start_addr, eeprom_buf, 512);		//������д��ȥ��
				address += area_addr_yu;              	//д�˶��٣���ַ�ͼӶ��١�
				num_yu -= area_addr_yu;               	//ʣ��������Ҫ�����١�
				dat_offset += area_addr_yu;           	//�����ƫ�ƾͼӶ��١�
			}
		}while(1); 
	}
	else 	EEPROM_write_n(addr, dat, num);    //���ȫΪFF ��ֱ��д��;
}



