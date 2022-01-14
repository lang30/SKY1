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

#ifndef		__CONFIG_H
#define		__CONFIG_H

//========================================================================
//                                头文件
//========================================================================
#include "type_def.h"
#include "stc8a8k64d4.h"
#include <stdlib.h>
#include <stdio.h>

//========================================================================
//                               主时钟定义
//========================================================================
//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
//#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟

// #ifndef DEBUG
// #define MAIN_Fosc		22118400L
// #else
// #define MAIN_Fosc		11000000L
// #endif

//========================================================================
//                               STC8x_SYSCLK.h配置
//========================================================================
#define    STC8Ax      (0)
#define    STC8AxD4    (1)
#define    STC8Cx      (2)
#define    STC8Fx      (3)
#define    STC8Gx      (4)
#define    STC8Hx      (5)
#define    PER_LIB_MCU_MUODEL     STC8Ax
#define   PER_LIB_SYSCLK_CTRL    (1) 		/*!< 系统时钟模块控制宏，写1开启，写0关闭。*/
////////////适配STC8x_REG.h/////////////////////
#define EAXFR_ENABLE() EAXSFR()
#define EAXFR_DISABLE() EAXRAM()
/*--------------------------------------------------------
| @Description: IO definitions                           |
--------------------------------------------------------*/
#define    __I    volatile   // 'read only' permissions 
#define    __O    volatile   // 'write only'  permissions 
#define    __IO   volatile   // 'read / write' permissions 
	

typedef  unsigned char   uint8_t; //  8 bits 
typedef  unsigned int   uint16_t; // 16 bits 
typedef  unsigned long  uint32_t; // 32 bits 
typedef  signed char  int8_t;	//  8 bits 
typedef  signed int  int16_t;	// 16 bits 
typedef  signed long int32_t;	// 32 bits 

typedef enum
{
    FSC_FAIL= 1, /* Function self checking failed */
    FSC_SUCCESS = !FSC_FAIL /* Function self checking successfed */ 
}   FSCSTATE; 


typedef struct
{
    __IO uint8_t CKSEL_REG; /*----Clock selection  */
	
    __IO uint8_t CLKDIV_REG; /*----Clock frequency division */
	
    __IO uint8_t IRC24MCR_REG; /*----High internal 24MHz oscillator control */	
	
    __IO uint8_t XOSCCR_REG; /*----External oscillator control */
	
    __IO uint8_t IRC32KCR_REG; /*----Internal 32KHz oscillator control */

}   SYSCLK_TypeDef;

#define    SYSCLK_BASE    0xFE00U
#define    SYSCLK     (* (SYSCLK_TypeDef  xdata *) SYSCLK_BASE)
#define    IRC_24M    (*(__I uint8_t idata *)0xFB)
//========================================================================
//                             外部函数和变量声明
//========================================================================

#endif
