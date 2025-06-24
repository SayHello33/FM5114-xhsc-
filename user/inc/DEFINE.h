/*********************************************************************
*                                                                    *
*   Copyright (c) 2010 Shanghai FuDan MicroElectronic Inc, Ltd.      *
*   All rights reserved. Licensed Software Material.                 *
*                                                                    *
*   Unauthorized use, duplication, or distribution is strictly       *
*   prohibited by law.                                               *
*                                                                    *
**********************************************************************
*********************************************************************/

/*********************************************************************/
/* 	ARM所用变量宏定义													 */
/* 	主要功能:														 */
/* 		1.程序中产量定义											 */
/* 	编制:赵清泉														 */
/* 	编制时间:2012年5月16日											 */
/* 																	 */
/*********************************************************************/

#ifndef _DEFINE_H_
#define _DEFINE_H_

#include "LPCD_API.h"
#include "gpio.h"
#include "hc32l021.h"                   // Device header
#include "stdbool.h"
#define UART1_EN_SEL        0x00
#define UART3_EN_SEL  	    0x01
#define	I2C1_EN_SEL		    	0x01
#define SPI1_EN_SEL		    	0x00
#define SPI2_EN_SEL         0x00
#define USB_EN_SEL          0x00
#define UART1_BaudRate      115200
#define UART3_BaudRate      115200
#ifndef true
	#define true				TRUE	//真
#endif
#ifndef false
	#define false				FALSE	//假
#endif
#ifndef True
	#define True				TRUE
	#define False				FALSE
#endif

#ifndef uchar
	#define uchar 			u8
	#define uint			u16
#endif

#define BIT0               0x01
#define BIT1               0x02
#define BIT2               0x04
#define BIT3               0x08
#define BIT4               0x10
#define BIT5               0x20
#define BIT6               0x40
#define BIT7               0x80

#define UART_SOF 0xAA    
#define UART_EOF 0x55
#define Status_0 0
#define Status_1 1
#define Status_2 2
#define Status_3 3
#define Status_4 4
#define Status_5 5
#define Status_6 6

#define Status_Acheivement  254//完成状态
#define Status_Idle 255//空闲状态

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef struct
{
	u16 Send_Len;
	u16 Send_Index;
	bool SendStatus;
	u16 Recv_Len;
	u16 Recv_Index; 
	bool RecvStatus;			
	char *buff;
	u8 Recv_data;
} UART_Com_Para_Def;

typedef struct
{
		u8 Cmd[2];	
		u8 Result;
		u8 Buff[255];
    u8 Length;
    u8 Index;
    u8 Status;
    u8 Bcc;
   
}BUFF_Def;

typedef struct
{
    u8 Mode;
    BUFF_Def Send;
    BUFF_Def Receive;
} UART_Def;

//******************* ARM 管脚定义区 **************************
//FM5114 Pin_IRQ1
#define PORT_READER_IRQ					GPIOA
#define PIN_READER_IRQ			GPIO_PIN_05				//PC12 IRQ引脚（Pin19）
//FM5114 Pin_IRQ2
#define PORT_TOUCH_IRQ         GPIOA
#define PIN_TOUCH_IRQ        GPIO_PIN_02//PB14
//FM5114 Touch Reset
#define PORT_XRST					GPIOA
#define PIN_XRST					GPIO_PIN_06		//PB12
//FM5114 Reader Reset
#define PORT_NRST					GPIOA
#define PIN_NRST					GPIO_PIN_12		//PC15 NPD引脚（Pin6）

//FM5114 Pin_I2C1	 
#define PORT_I2C1					GPIOA
#define PIN_SDA1					GPIO_PIN_10			//PB7 SDA引脚（Pin24）
#define PIN_SCL1					GPIO_PIN_11		//PB6 SCL引脚（Pin31）

//LED控制：PD2
//#define PORT_LED              GPIOA		   
//#define PIN_LED0              GPIO_PIN_12
//#define LED_ARM_WORKING				PIN_LED0
//#define LED_ARM_WORKING_OFF		(PORT_LED->BSRR = LED_ARM_WORKING)		//LED0
//#define LED_ARM_WORKING_ON		(PORT_LED->BRR = LED_ARM_WORKING)			//LED0

//-----------------------------------------------------------------------------
void GPIO_ExtiInit();
#endif
