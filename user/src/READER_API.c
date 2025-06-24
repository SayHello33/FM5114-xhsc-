/*********************************************************************
*                                                                    *
*   Copyright (c) 2010 Shanghai FuDan MicroElectronic Inc, Ltd.      *
*   All rights reserved. Licensed Software Material.                 *
*                                                                    *
*   Unauthorized use, duplication, or distribution is strictly       *
*   prohibited by law.                                               *
*                                                                    *
*********************************************************************/

/********************************************************************/
/* 	主要功能:						        																		*/
/* 	    TYPE A 和 TYPE B卡片Polling操作								        			*/
/*      支持身份证UID读取														      					*/
/* 	编制:宋耀海 																										*/
/* 	编制时间:2017年9月7日																						*/
/* 																																	*/
/********************************************************************/
#include "string.h"
#include "DEVICE_CFG.h"
#include "FM5114_REG.h"
#include "READER_API.h"
#include "FM5114.h"
#include "DEFINE.h"
//#include "MIFARE.h"
#include "UART.h"
#include "LPCD_API.h"
struct picc_a_struct PICC_A; 
struct picc_b_struct PICC_B;

//*************************************
//函数  名：FM5114_Initial_ReaderA
//入口参数：
//出口参数：
//*************************************
void log_printf(const char *fmt, ...);
void FM5114_Initial_ReaderA(void)
{	
	SetReg(READER_I2C_Address,JREG_MODWIDTH,MODWIDTH_106);	//MODWIDTH = 106kbps
	ModifyReg(READER_I2C_Address,JREG_TXAUTO,BIT6,SET);//Force 100ASK = 1
	SetReg(READER_I2C_Address,JREG_GSN,(GSNON_A<<4));//Config GSN; Config ModGSN 	
	SetReg(READER_I2C_Address,JREG_CWGSP,GSP_A);//Config GSP
	SetReg(READER_I2C_Address,JREG_CONTROL,BIT4);//Initiator = 1
	SetReg(READER_I2C_Address,JREG_RFCFG,RXGAIN_A<<4);//Config RxGain
	SetReg(READER_I2C_Address,JREG_RXTRESHOLD,(MINLEVEL_A<<4) | COLLLEVEL_A);//Config MinLevel; Config CollLevel	
	return;
}
//*************************************
//函数  名：FM175114_Initial_ReaderB
//入口参数：
//出口参数：
//*************************************

void FM5114_Initial_ReaderB(void)
{
	ModifyReg(READER_I2C_Address,JREG_STATUS2,BIT3,RESET);

	SetReg(READER_I2C_Address,JREG_MODWIDTH,0x30);//MODWIDTH = 106kbps	
	SetReg(READER_I2C_Address,JREG_TXAUTO,0);//Force 100ASK = 0		
	SetReg(READER_I2C_Address,JREG_GSN,(GSNON_B<<4)|MODGSNON_B);//Config GSN; Config ModGSN   
	SetReg(READER_I2C_Address,JREG_CWGSP,GSP_B);//Config GSP
	SetReg(READER_I2C_Address,JREG_MODGSP,MODGSP_B);//Config ModGSP
	SetReg(READER_I2C_Address,JREG_CONTROL,BIT4);//Initiator = 1
	SetReg(READER_I2C_Address,JREG_RFCFG,RXGAIN_B<<4);//Config RxGain
	SetReg(READER_I2C_Address,JREG_RXTRESHOLD,MINLEVEL_B<<4);//Config MinLevel;
	return;
}

//*************************************
//函数  名：ReaderA_Halt
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderA_Halt(void)
{	
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0x80);//Enable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x50);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSMIT);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x80);//Start Send
	DDL_Delay1ms(2);//Wait 2ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 0)
	{	
			return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;
	
}
//*************************************
//函数  名：ReaderA_Wakeup
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderA_Wakeup(void)
{
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_TXMODE,0);//Disable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0);//Disable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,RF_CMD_WUPA);
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x87);//Start Send
	DDL_Delay1ms(1);//Wait 1ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 2)
	{
		log_printf("-> wakeup succ \r\n");	
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.ATQA);
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.ATQA+1);
		return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;
}
//*************************************
//函数  名：ReaderA_Request
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderA_Request(void)
{
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_TXMODE,0);//Disable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0);//Disable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,RF_CMD_REQA);
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x87);//Start Send
	DDL_Delay1ms(2);//Wait 2ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 2)
	{
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.ATQA);
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.ATQA+1);
		return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;	
}

//*************************************
//函数  名：ReaderA_AntiColl
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderA_AntiColl(void)
{
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_TXMODE,0);//Disable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0);//Disable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,RF_CMD_ANTICOL);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x20);
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x80);//Start Send
	DDL_Delay1ms(2);//Wait 2ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 5)
	{
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID);
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID+1);
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID+2);
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID+3);
		GetReg(READER_I2C_Address,JREG_FIFODATA,&PICC_A.BCC);
		if( (PICC_A.UID[0] ^ PICC_A.UID[1] ^ PICC_A.UID[2] ^ PICC_A.UID[3]) == PICC_A.BCC)
			return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;	
}

//*************************************
//函数  名：ReaderA_Select
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderA_Select(void)
{
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_TXMODE,0x80);//Enable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0x80);//Enable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x93);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x70);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID[0]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID[1]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID[2]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.UID[3]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_A.BCC);		
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x80);//Start Send
	DDL_Delay1ms(2);//Wait 2ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
			GetReg(READER_I2C_Address,JREG_FIFODATA,&PICC_A.SAK);		
			return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;	
}

//*************************************
//函数  名：ReaderA_CardActivate
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderA_CardActivate(void)
{
unsigned char  result;	
		result = ReaderA_Wakeup();//	
		if (result != FM5114_SUCCESS)
		{			
			log_printf("->  1  FM5114_COMM_ERR fail \r\n");	
			return FM5114_COMM_ERR;
		}
		result = ReaderA_AntiColl();//
		if (result != FM5114_SUCCESS)
		{
			log_printf("-> 2   FM5114_COMM_ERR fail \r\n");	
					return FM5114_COMM_ERR;
		}
						
		result = ReaderA_Select();//
		if (result != FM5114_SUCCESS)
						return FM5114_COMM_ERR;
								
			
		return result;
}
static void ProcessCardUID(uint8_t uid[8])
{
    // 打印UID
    log_printf("Card UID: ");
    for (int i = 0; i < 8; i++) {
        log_printf("%X ", uid[i]);
    }
    log_printf("\n");
    
    // 这里添加您自己的业务逻辑
    // 例如：保存到数据库、验证权限等
}

//*************************************
//函数  名：ReaderB_CardActivate
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderB_CardActivate(void)
{
unsigned char  result;	
	
		result = ReaderB_Request();//	
		if (result != FM5114_SUCCESS)
		{			
			log_printf("->  B  FM5114_COMM_ERR fail \r\n");	
			return FM5114_COMM_ERR;
		}
		result = ReaderB_Attrib();//
		if (result != FM5114_SUCCESS)
		{
			log_printf("-> 2   FM5114_COMM_ERR fail \r\n");	
					return FM5114_COMM_ERR;
		}
						
		result = ReaderB_GetUID();//
		if (result != FM5114_SUCCESS)
						return FM5114_COMM_ERR;
	   ProcessCardUID(PICC_B.UID);		
 log_printf("B Card reading completed\n");		
			
		return result;
}
//*************************************
//函数  名：ReaderB_Wakeup
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderB_Wakeup(void)
{
	unsigned char reg_data,i;
	SetReg(READER_I2C_Address,JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x05);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x08);
	
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x80);//Start Send
	DDL_Delay1ms(10);//Wait 10ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 12)
	{
		for(i = 0;i < 12;i++)
		GetReg(READER_I2C_Address,JREG_FIFODATA,&PICC_B.ATQB[i]);
		memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
		memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
		memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
			return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;	
}
//*************************************
//函数  名：ReaderB_Request
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderB_Request(void)
{	
	unsigned char reg_data,i;
	SetReg(READER_I2C_Address,JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x05);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x80);//Start Send
	DDL_Delay1ms(10);//Wait 10ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 12)
	{
		for(i = 0;i < 12;i++)
		GetReg(READER_I2C_Address,JREG_FIFODATA,&PICC_B.ATQB[i]);
		memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
		memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
		memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
			return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;	
}
//*************************************
//函数  名：ReaderB_Attrib
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderB_Attrib(void)
{
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x1D);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_B.PUPI[0]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_B.PUPI[1]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_B.PUPI[2]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,PICC_B.PUPI[3]);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x08);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x01);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x01);
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x80);//Start Send
	DDL_Delay1ms(10);//Wait 10ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{		
		GetReg(READER_I2C_Address,JREG_FIFODATA,PICC_B.ATTRIB);
			return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;	
}
//*************************************
//函数  名：ReaderB_GetUID
//入口参数：
//出口参数：FM5114_SUCCESS, FM5114_COMM_ERR
//*************************************
unsigned char ReaderB_GetUID(void)
{
	unsigned char reg_data,i;
	SetReg(READER_I2C_Address,JREG_TXMODE,0x83);//Enable TxCRC
	SetReg(READER_I2C_Address,JREG_RXMODE,0x83);//Enable RxCRC
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_IDLE);//command = Idel
	SetReg(READER_I2C_Address,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x36);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x00);
	SetReg(READER_I2C_Address,JREG_FIFODATA,0x08);
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	SetReg(READER_I2C_Address,JREG_BITFRAMING,0x80);//Start Send
	DDL_Delay1ms(10);//Wait 10ms
	GetReg(READER_I2C_Address,JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 10)
	{		
		for(i=0;i<8;i++)
		GetReg(READER_I2C_Address,JREG_FIFODATA,&PICC_B.UID[i]);
			return FM5114_SUCCESS;
	}
	return FM5114_COMM_ERR;	
}


