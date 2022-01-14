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

#ifndef		__TYPE_DEF_H
#define		__TYPE_DEF_H

// bool ���ͣ��޷��� struct ���ã�
#define bool bit
#define true 1
#define false 0
#define on 1
#define off 0
#define nil 0
#define SUCCESS 0

#define byte unsigned char // 0~255
#define word unsigned int  // 0~65535
#define error unsigned int

#define pbyte unsigned char *
#define pByte unsigned char *

#define pUint32 unsigned long *
//========================================================================
//                               ���Ͷ���
//========================================================================

typedef unsigned char   u8;     //  8 bits 
typedef unsigned int    u16;    // 16 bits 
typedef unsigned long   u32;    // 32 bits 

typedef signed char     int8;   //  8 bits 
typedef signed int      int16;  // 16 bits 
typedef signed long     int32;  // 32 bits 

typedef unsigned char   uint8;  //  8 bits 
typedef unsigned int    uint16; // 16 bits 
typedef unsigned long   uint32; // 32 bits 

typedef void (*Procs)() small;
typedef void (*ProcUint16)(uint16 AValue);
typedef void (*ProcsUint16)(uint16 AValue) small;
typedef void (*ProcUint32)(uint32 AValue);
typedef void (*ProcPUint32)(pUint32 AValue);
typedef void (*ProcByte)(byte AValue);

typedef void (*FuncUint16_2)(uint16 AValue,uint16 BValue);
typedef void (*FuncUint8)(uint8 Value);
typedef void (*FuncVoid)(void);     //typedef void (*Proc)();
#define Proc FuncVoid

#define	TRUE	1
#define	FALSE	0

//=============================================================

#define	NULL	0

//========================================

#define	Priority_0			0	//�ж����ȼ�Ϊ 0 ������ͼ���
#define	Priority_1			1	//�ж����ȼ�Ϊ 1 �����ϵͼ���
#define	Priority_2			2	//�ж����ȼ�Ϊ 2 �����ϸ߼���
#define	Priority_3			3	//�ж����ȼ�Ϊ 3 ������߼���

#define ENABLE		1
#define DISABLE		0

#define SUCCESS		0
#define FAIL			-1


#endif
