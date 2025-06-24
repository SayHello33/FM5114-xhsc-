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
/* 	FM5114 LPCD API V2函数库																			  */
/* 	主要功能:						        																		*/
/* 	编制:       																										*/
/* 	编制时间:                     																	*/
/* 																																	*/
/********************************************************************/
#include "LPCD_CFG.h"
#include "LPCD_API.h"
#include "FM5114_REG.h"
#include "FM5114.h"
#include "DEVICE_CFG.h"
#include "uart.h"
#include "MIFARE.h"
#include "READER_API.h"
#include "ddl.h"
#include "DEFINE.h"


// 全局配置参数
#define LPCD_THRESHOLD      0x13   // 幅度检测阈值 (推荐值24 0x18)
#define LPCD_RF_TIME        LPCD_RFTIME_25us  // RF检测时间
#define LPCD_REQA_TIME      LPCD_REQA_TIME_5ms  // REQA检测时间
#define LPCD_RX_GAIN        LPCD_RXGAIN_43DB  // 接收增益
#define LPCD_MIN_LEVEL      LPCD_MINLEVEL_9   // 最小检测电平
#define LPCD_MOD_WIDTH      LPCD_MODWIDTH_38  // 调制宽度
#define LPCD_TX_SCALE       LPCD_TXSCALE_4    // 发射功率比例
void log_printf(const char *fmt, ...);
unsigned char Lpcd_Card_Event(void);
//***********************************************
//函数名称：Lpcd_Init_Register()
//函数功能：LPCD寄存器初始化配置
//入口参数：
//出口参数：SUCCESS：配置完成 
//***********************************************
unsigned char Lpcd_Init_Register(void)
{
		

		FM5114_Reader_Reset();
		
		SetReg_Ext(READER_I2C_Address,0x24, 0x14);//请勿修改
		SetReg_Ext(READER_I2C_Address,0x25, 0x3A);//请勿修改
	
		uint8_t reg=0;

	while(reg==0x00)
	{
			SetReg_Ext(READER_I2C_Address,JREG_LPCDCTRLMODE, RF_DET_DISABLE|RF_DET_SEN_00|LPCD_ENABLE);//场检测使能，场检测灵敏度0，LPCD使能
			GetReg_Ext(READER_I2C_Address,JREG_LPCDCTRLMODE,&reg);
		log_printf("JREG_LPCDCTRLMODE %x \r\n",reg);
	}
				
		
		SetReg_Ext(READER_I2C_Address,JREG_LPCDRFTIMER, LPCD_IRQINV_ENABLE|LPCD_IRQ_PUSHPULL|LPCD_RFTIME_5us);//探测使用5us，LPCD探测总时间18us

			GetReg_Ext(READER_I2C_Address,JREG_LPCDRFTIMER,&reg);
		log_printf("JREG_LPCDRFTIMER %x \r\n",reg);

	SetReg_Ext(READER_I2C_Address,JREG_LPCDTHRESH_H,BIT5|BIT2|((LPCD_THRSH & 0xC0)>>6));//LPCD触发阈值高2位
		SetReg_Ext(READER_I2C_Address,JREG_LPCDTHRESH_L, LPCD_THRSH & 0x3F);//LPCD触发阈值低6位
		SetReg_Ext(READER_I2C_Address,JREG_LPCDTXCTRL2, LPCD_CWP);//设置LPCD输出P驱动
		SetReg_Ext(READER_I2C_Address,JREG_LPCDTXCTRL3, LPCD_CWN);//设置LPCD输出N驱动
		SetReg_Ext(READER_I2C_Address,JREG_LPCDREQATIMER,LPCD_REQA_TIME_5ms);//REQA检测载波时间,5ms兼容手机方案
		
		SetReg_Ext(READER_I2C_Address,JREG_LPCDREQAANA,LPCD_RXGAIN_43DB | LPCD_MINLEVEL_9 | LPCD_MODWIDTH_38);//配置REQA检测命令的接收增益，接收阈值，调制宽度
		SetReg_Ext(READER_I2C_Address,JREG_LPCDDETECTMODE,LPCD_TXSCALE_4 | LPCD_COMBINE_MODE);//配置REQA检测发射功率与探测输出场强的比例，LCPD探测模式设置
		return SUCCESS;

}
void Lpcd_Get_ADC1_Value(unsigned short* result)
{
    unsigned char reg_hi, reg_lo;
    unsigned short delta_value;
    
    // 读取高8位和低6位
    GetReg_Ext(READER_I2C_Address, JREG_LPCDDELTA_HI, &reg_hi);
    GetReg_Ext(READER_I2C_Address, JREG_LPCDDELTA_LO, &reg_lo);
    
    // 合并为10位值（高2位+低8位）
    delta_value = (reg_hi & 0x03) << 8 | reg_lo;
    
    // 输出调试信息
    log_printf("-> LPCD Delta is: %x %x\r\n",(unsigned char*)&delta_value + 1);
   
    log_printf("\r\n");
    
    // 同时输出实际数值
    log_printf(" (%d",delta_value);

    log_printf(") \r\n");
    
    // 返回结果
    if(result) *result = delta_value;
}
void Test_LPCD_Performance()
{
    // 初始化LPCD
    Lpcd_Init_Register();
    
    // 获取基准场强
    unsigned short base_delta;
    Lpcd_Get_ADC1_Value(&base_delta);
    log_printf("-> Base Delta: %d",base_delta);
   
    log_printf("\r\n");
    
}
/**
 * @brief 配置LPCD支持B卡检测
 * @retval 配置状态: SUCCESS 或 ERROR
 */
unsigned char Configure_LPCD_For_BCard(void)
{
    unsigned char result;
    
    // 1. 设置LPCD控制模式
    uint8_t ctrlMode = RF_DET_ENABLE | RF_DET_SEN_00 | LPCD_ENABLE;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDCTRLMODE, ctrlMode);
    if (result != SUCCESS) return result;
    
    // 2. 配置RF定时器
    uint8_t rfTimer = LPCD_IRQINV_ENABLE | LPCD_IRQ_PUSHPULL | LPCD_RF_TIME;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDRFTIMER, rfTimer);
    if (result != SUCCESS) return result;
    
    // 3. 设置LPCD阈值
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDTHRESH_H, 
                       BIT5 | BIT2 | ((LPCD_THRESHOLD & 0xC0) >> 6));
    if (result != SUCCESS) return result;
    
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDTHRESH_L, 
                       (LPCD_THRESHOLD)& 0x3F);
    if (result != SUCCESS) return result;
    
    // 4. 配置REQA检测参数
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDREQATIMER, LPCD_REQA_TIME);
    if (result != SUCCESS) return result;
    
    uint8_t reqaAna = LPCD_RX_GAIN | LPCD_MIN_LEVEL | LPCD_MOD_WIDTH;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDREQAANA, reqaAna);
		DDL_Delay1ms(3);
    if (result != SUCCESS) return result;
    
    // 5. 设置检测模式为幅度检测
    uint8_t detectMode = LPCD_TX_SCALE | LPCD_RX_CHANGE_MODE;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDDETECTMODE, detectMode);
    if (result != SUCCESS) return result;
    
    // 6. 配置TX控制
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDTXCTRL1, 
                        LPCD_TX2_ENABLE|LPCD_TX2_ENABLE);
    if (result != SUCCESS) return result;
    
    // 7. 清除LPCD中断标志
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDIRQ, 0xFF);
    
    return result;
}
//***********************************************
//函数名称：Lpcd_Set_Mode()
//函数功能：LCPD工作模式设置
//入口参数：mode = ENABLE:开启LPCD mode = DISABLE :关闭LPCD
//出口参数：
//***********************************************
void Lpcd_Set_Mode(unsigned char mode)
{
	if(mode == ENABLE)
	{
			DDL_Delay1ms(1);
			GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0, 进入LPCD模式
		DDL_Delay1ms(5);
			
	}
	else
	{					
			GPIO_SetBits(PORT_NRST,PIN_NRST);//NPD = 1, 退出LPCD模式
			DDL_Delay1ms(1);
	}     
  return;
}

//***********************************************
//函数名称：Lpcd_Adc_Event()
//函数功能：LPCD中断处理
//入口参数：
//出口参数：
//***********************************************
void Lpcd_Adc_Event(void)
{
	unsigned char reg;

	while(GPIO_Read(PORT_READER_IRQ,PIN_READER_IRQ) == 0)
	{
		Lpcd_Set_Mode(DISABLE); 		//NPD = 1,FM5114退出休眠模式	
		SetReg(TOUCH_I2C_Address, Slide_Channel_Config, 0x40);            //Touch_suspend=1			
		DDL_Delay1ms(5);
		
		log_printf ("-> LPCD Detceted!\r\n ");
		GetReg_Ext(READER_I2C_Address,JREG_LPCDIRQ, &reg);//读取LPCD中断标志
		SetReg_Ext(READER_I2C_Address,JREG_LPCDIRQ, reg);//CLEAR LPCD IRQ
		
		log_printf("->JREG_LPCDIRQ: %x",reg);log_printf("\r\n");
		
		if(reg & 0x04)//RXchange标志
		{ 
			log_printf("-> LPCD IRQ RXCHANGE SET!\r\n");//LPCD中断标志			
			Lpcd_Get_ADC_Value();//调试时使用，读取LPCD探测值				
		}
		if(reg & 0x02)//寻卡检测标志
		{
			log_printf("-> LPCD IRQ ATQAREC SET!\r\n");
		}
		if(reg & 0x08)//场检测标志
		{
			log_printf("-> LPCD IRQ RFDET SET!\r\n");
		}
		if((reg & 0x08) ||(reg & 0x04)||(reg & 0x02))
		{
			uint8_t result=Lpcd_Card_Event();
//			if(result!=SUCCESS)
//					Lpcd_Card_B_Event();         //读卡操作

		}
		Lpcd_Init_Register();
//		Configure_LPCD_For_BCard();
		SetReg(TOUCH_I2C_Address, Slide_Channel_Config, 0x00);            //Touch_suspend=0
		Lpcd_Set_Mode(ENABLE);
	}	
	return;	
}

//***********************************************
//Lpcd_Get_ADC_Value()
//函数功能：Lpcd_Get_ADC_Value读取LPCD的ADC数据
//入口参数：
//出口参数：
//***********************************************
void Lpcd_Get_ADC_Value(void)
{
	unsigned char reg,reg1;
	unsigned char lpcd_delta1,lpcd_delta2;
	
		GetReg_Ext(READER_I2C_Address,JREG_LPCDDELTA_HI, &reg);
		GetReg_Ext(READER_I2C_Address,JREG_LPCDDELTA_LO, &reg1);
		lpcd_delta1 = (reg<<6) + reg1;
		lpcd_delta2 = ((reg<<6)>>8);
		log_printf("-> LPCD Delta is: %x %x \r\n",lpcd_delta2,lpcd_delta1);
	
	return;	
}




unsigned char TYPE_A_EVENT(void)
{
unsigned char result;
		log_printf("-> TYPE A CARD!\r\n");	
		SetCW(0);		
		FM5114_Initial_ReaderA();
		SetCW(3);		

//		
		result = ReaderA_CardActivate();
		if(result != FM5114_SUCCESS)
				{
						log_printf("-> fail \r\n");	
						SetCW(0);		
						return result;
				}
		log_printf("-> ATQA = %x %x",PICC_A.ATQA[0],PICC_A.ATQA[1]);log_printf("\r\n");					
		log_printf("-> UID = %x %x %x %x \r\n",PICC_A.UID[0],PICC_A.UID[1],PICC_A.UID[2],PICC_A.UID[3]);					
		log_printf("-> SAK = %x" ,PICC_A.SAK);log_printf("\r\n");		
		result = Mifare_Auth(KEY_A_AUTH,0,KEY_A[0],PICC_A.UID);
		if(result != FM5114_SUCCESS)
				{
				SetCW(0);		
				return result;
				}
		log_printf("-> AUTH SUCCESS\r\n");	
		result = Mifare_Blockread(0,BLOCK_DATA);
				if(result != FM5114_SUCCESS)
				{
				SetCW(0);		
				return result;
				}
			log_printf("-> BLOCK_DATA = ");
			for(uint8_t i=0;i<16;i++)
			{
				log_printf("%x ",BLOCK_DATA[i]);
				
			}
			log_printf("\r\n");			
		SetCW(0);	
		return result;
}

//***********************************************
//函数名称：Lpcd_Card_Event()
//函数功能：LPCD触发后读卡函数
//入口参数：
//出口参数：
//***********************************************
unsigned char Lpcd_Card_Event(void)
{
	unsigned char result;
	 result = TYPE_A_EVENT();
		return result;
}
unsigned char TYPE_B_EVENT(void)
{
unsigned char result;
		log_printf("-> TYPE B CARD!\r\n");	
		SetCW(0);		
		FM5114_Initial_ReaderB();
		SetCW(3);		
		DDL_Delay1ms(5);
		
		result = ReaderB_CardActivate();
		if(result != FM5114_SUCCESS)
				{
					log_printf("-> fail 1  \r\n");	
					SetCW(0);		
					log_printf("-> fail 2 \r\n");
					return result;
				}
	log_printf("-> ***********  \r\n");	
		return result;
}

unsigned char Lpcd_Card_B_Event(void)
{
	unsigned char result;
	 result = TYPE_B_EVENT();
	return result;


}