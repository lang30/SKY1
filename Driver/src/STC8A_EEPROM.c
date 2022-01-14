/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "STC8A_EEPROM.h"

extern uint32 MAIN_Fosc;

u8 EEPROM_Read_Buff[6];
//========================================================================
// 函数: void	ISP_Disable(void)
// 描述: 禁止访问ISP/IAP.
// 参数: non.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void	DisableEEPROM(void)
{
	IAP_CONTR = 0;			//禁止IAP操作
	IAP_CMD   = 0;			//去除IAP命令
	IAP_TRIG  = 0;			//防止IAP命令误触发
	IAP_ADDRH = 0xff;		//清0地址高字节
	IAP_ADDRL = 0xff;		//清0地址低字节，指向非EEPROM区，防止误操作
}

//========================================================================
// 函数: void EEPROM_Trig(void)
// 描述: 触发EEPROM操作.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void EEPROM_Trig(void)
{
	F0 = EA;    //保存全局中断
	EA = 0;     //禁止中断, 避免触发命令无效
	IAP_TRIG = 0x5A;
	IAP_TRIG = 0xA5;                    //先送5AH，再送A5H到IAP触发寄存器，每次都需要如此
																			//送完A5H后，IAP命令立即被触发启动
																			//CPU等待IAP完成后，才会继续执行程序。
	_nop_();
	_nop_();
	EA = F0;    //恢复全局中断
}

//========================================================================
// 函数: void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u16 number)
// 描述: 从指定EEPROM首地址读出n个字节放指定的缓冲.
// 参数: EE_address:  读出EEPROM的首地址.
//       DataAddress: 读出数据放缓冲的首地址.
//       number:      读出的字节长度.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u16 number)
{
	IAP_ENABLE();                           //设置等待时间，允许IAP操作，送一次就够
	IAP_READ();                             //送字节读命令，命令不需改变时，不需重新送命令
	do
	{
		IAP_ADDRH = EE_address / 256;       //送地址高字节（地址需要改变时才需重新送地址）
		IAP_ADDRL = EE_address % 256;       //送地址低字节
		EEPROM_Trig();                      //触发EEPROM操作
		*DataAddress = IAP_DATA;            //读出的数据送往
		EE_address++;
		DataAddress++;
	}while(--number);

	DisableEEPROM();
}

//========================================================================
// 函数: void EEPROM_SectorErase(u16 EE_address)
// 描述: 把指定地址的EEPROM扇区擦除.
// 参数: EE_address:  要擦除的扇区EEPROM的地址.
// 返回: non.
// 版本: V1.0, 2013-5-10
//========================================================================
void EEPROM_SectorErase(u16 EE_address)
{
	IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
	IAP_ERASE();                        //宏调用, 送扇区擦除命令，命令不需改变时，不需重新送命令
																			//只有扇区擦除，没有字节擦除，512字节/扇区。
																			//扇区中任意一个字节地址都是扇区地址。
	IAP_ADDRH = EE_address / 256;       //送扇区地址高字节（地址需要改变时才需重新送地址）
	IAP_ADDRL = EE_address % 256;       //送扇区地址低字节
	EEPROM_Trig();                      //触发EEPROM操作
	DisableEEPROM();                    //禁止EEPROM操作
	printf("EraseAddr：%04xH ",EE_address);
}

//========================================================================
// 函数: void EEPROM_write_n(u16 EE_address,u8 *DataAddress,u16 number)
// 描述: 把缓冲的n个字节写入指定首地址的EEPROM.
// 参数: EE_address:  写入EEPROM的首地址.
//       DataAddress: 写入源数据的缓冲的首地址.
//       number:      写入的字节长度.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void EEPROM_write_n(u16 EE_address,u8 *DataAddress,u16 number)
{
	IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
	IAP_WRITE();                        //宏调用, 送字节写命令
	do
	{
		IAP_ADDRH = EE_address / 256;     //送地址高字节（地址需要改变时才需重新送地址）
		IAP_ADDRL = EE_address % 256;     //送地址低字节
		IAP_DATA  = *DataAddress;         //送数据到IAP_DATA，只有数据改变时才需重新送
		EEPROM_Trig();                    //触发EEPROM操作
		EE_address++;                     //下一个地址
		DataAddress++;                    //下一个数据
	}while(--number);                   //直到结束
	DisableEEPROM();
}
//========================================================================
// 函数: void EEPROM_write_n_ex(u16 addr, u8 *dat, u16 num)
// 描述: flash是要擦除才能写入，擦除就是把0变成1；擦除是按扇区擦除！ 此函数的功能是写入数据而不擦除其他地址的数据；
		 
// 参数: addr:  写入EEPROM的首地址.
//       dat:  写入源数据的缓冲的首地址.
//       number:      写入的字节长度.
// 返回: num.
// 版本: V1.0, 2021-8-4 11:20:40
//========================================================================
u8 xdata eeprom_buf[512];//用于整合数据的缓存
void EEPROM_write_n_ex(u16 addr, u8 *dat, u16 num)
{
    u16 num_yu;         	//剩余要写的数据的数量。
    u16 area;           	//操作扇区。
    u16 area_addr_yu;   	//扇区余下的地址数量。
    u16 area_start_addr;	//扇区开始地址。
    u16 area_stop_addr; 	//扇区结束地址。
    u16 addr_offset;    	//地址偏移。
    u16 dat_offset = 0;   	//数据偏移。
    u16 address;        	//操作地址。
    u16 i;              	//临时变量。
	u8 read_buf[6];
    address = addr;       	//将地址参数赋值给操作地址。
    num_yu = num;         	//将参数数量赋值为剩余数量。
	EEPROM_read_n(addr, read_buf, num);  //读取要写入地址的数据
	for(i=0; i< num; i++)	
	{   if(read_buf[i] != 0xff)		break;	 }     //判断地址写入前的数据是否为全为ff，如果不为FF，就擦除扇区写入！
	if(i<num )  {
		do{
			area = address >> 9;                       	//获取扇区。
			area_start_addr = (area << 9);           	//获取扇区开始的地址。
			area_stop_addr = (area << 9) + 512;       	//获取扇区末尾的地址。
			area_addr_yu = area_stop_addr - address; 	//末尾地址减操作地址，得到地址剩余数量。
			if(num_yu <= area_addr_yu)			//如果要写的数少于地址剩余数量，说明不用跨扇区。
			{             	
				EEPROM_read_n(area_start_addr, eeprom_buf, 512);  //将原来的数据读出来。
				EEPROM_SectorErase(area_start_addr);	//清空扇区。
				addr_offset = address - area_start_addr;	//获取地址偏移。
				for(i=0; i < num_yu; i++)			//修改数据。
				{   eeprom_buf[addr_offset + i] = dat[dat_offset+i];	}
				EEPROM_write_n(area_start_addr, eeprom_buf, 512);
				break;                      	//写完就跳出循环。
			}
			else 								
			{                                  	//如果要写的数多于地址剩余量。
				EEPROM_read_n(area_start_addr, eeprom_buf, 512);  	//将原来的数据读出来。
				EEPROM_SectorErase(area_start_addr);	//清空扇区。
				addr_offset=address-area_start_addr;	//获取地址偏移。
				for(i=0;i<area_addr_yu;i++)				//修改数据。
				{   eeprom_buf[addr_offset+i]=dat[dat_offset+i];	}
				EEPROM_write_n(area_start_addr, eeprom_buf, 512);		//将数据写回去。
				address += area_addr_yu;              	//写了多少，地址就加多少。
				num_yu -= area_addr_yu;               	//剩余数量就要减多少。
				dat_offset += area_addr_yu;           	//缓存的偏移就加多少。
			}
		}while(1); 
	}
	else 	EEPROM_write_n(addr, dat, num);    //如果全为FF ，直接写入;
}



