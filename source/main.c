/**
 *******************************************************************************
 * @file  main.c
 * @brief This file provides example of HSI2C
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-12-02       MADS            First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2024, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "flash.h"
#include "gpio.h"
#include "hsi2c.h"
#include "lpm.h"
#include "lpuart.h"
#include "sysctrl.h"
#include "stdio.h"
#include "uart.h"
#include "MIFARE.h"
#include "FM5114.h"
#include "FM5114_REG.h"
#include "READER_API.h"
#include "DEFINE.h"
#include "LPCD_API.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define BAUD_RATE   (1000000) /* 波特率 */
#define DEVICE_ADDR (0x06u)   /* 从机设备地址 */
#define TRANS_SIZE  (30)      /* 传输字节数 */

/* Address mode */
#define HSI2C_ADDR_MD_7BIT  (0u)
#define HSI2C_ADDR_MD_10BIT (1u)
/* Config address mode */
#define HSI2C_ADDR_MD (HSI2C_ADDR_MD_7BIT)

#define TRANS_TIMEOUT (0xFFFFFFFFu) /* 传输超时保护计数值 */
/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*******************************************************************************
 * Local function prototypes ('static')
 
 ******************************************************************************/
 #define RX_TX_FRAME_LEN 10u /* 通信帧长度 */
static void SysClockConfig(void);
static void GpioConfig(void);
static void Hsi2cMasterReadConfig(void);
static void Hsi2cMasterWriteConfig(void);
void log_printf(const char *fmt, ...);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t u8WriteData[TRANS_SIZE] = {0};
static uint8_t u8ReadData[TRANS_SIZE]  = {0};
uint8_t u8RxData[RX_TX_FRAME_LEN] = {0};
uint8_t u8TxData[2]               = {0x55, 0xAA};
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Main function
 * @retval int32_t return value, if needed
 */
// 手动设置单个按键阈值
void SetButtonThreshold(uint8_t channel, uint8_t threshold)
{
    // 计算寄存器地址: 0x20 + channel
    uint8_t reg_addr = Button0_Touch_Thrsh + channel;
    
    // 设置阈值
    SetReg(TOUCH_I2C_Address, reg_addr, threshold);
    
    log_printf("Set Ch%d Thrsh: 0x%X\n", channel, threshold);
}

int32_t main(void)
{
    uint32_t u32Index = 0;
	unsigned char reg_data,result;
	unsigned char reg_slide=1,reg1,reg2,reg3,reg4;
	unsigned char LPCD_counter = 0;
	char BUFF[10];
    /* 系统时钟配置 */
    SysClockConfig();
GpioConfig();
    /* LED端口配置 */
   // STK_LedConfig();
	uart_init();
	GPIO_ExtiInit();


		FM5114_Reader_Reset();

	
//Init_I2C_And_Scan();
			

	FM5114_Touch_Reset();
	
	result = FM5114_Touch_SoftReset();
	if(result != FM5114_SUCCESS)		 
		log_printf("FM5114 TOUCH SoftReset Error\r\n");
	
	result = FM5114_Touch_Trim_Load();
	if(result != FM5114_SUCCESS)		 
		log_printf("FM5114 TOUCH Trim Load Error\r\n");


	log_printf("FM5114 TOUCH READER DEMO V8\r\n");
	GetReg(READER_I2C_Address,JREG_VERSION,&reg_data);
	log_printf("-> Reader Version = %d",reg_data);log_printf("\r\n");
	result=Lpcd_Init_Register();                               //FM5114LPCD初始化函数
//	Configure_LPCD_For_BCard();




	result = FM5114_Touch_Init(INTT_MODE, SENSE ,BUTTON_CHANNEL, GROUP_BUTTON_EN, LPCD_TIMER_EN);//0x3FFF表示14个通道使能,bit0~bit13
	if(result != FM5114_SUCCESS)
	{
		log_printf("-> Touch Init ERROR\r\n");
		while(1);
	}
	log_printf("-> Touch Init Success\r\n");
	
	
	FM5114_Touch_Reset();
	result = FM5114_Touch_Init(FAST_MODE, SENSE ,BUTTON_CHANNEL, GROUP_BUTTON_EN, LPCD_TIMER_EN);
	if(result == FM5114_SUCCESS)
		log_printf("-> Touch Reload Success\r\n");
	Touch_IRQ = 0;
	LPCD_IRQ = 0;
	DDL_Delay1ms(500);
	Lpcd_Set_Mode(ENABLE); //LPCD使能
	DDL_Delay1ms(5); // 等待复位完成
	Scan_Start();  //启动扫描
	// 在无卡环境下读取噪声信号幅度
//	while(1)
//	{
//Lpcd_Get_ADC_Value();
//		DDL_Delay1ms(300);
//	
//	}


//		while(1)
//	{
//		Lpcd_Get_ADC_Value();
//	DDL_Delay1ms(500);
//	
//	}
uint8_t current_threshold;
GetReg(TOUCH_I2C_Address, Button9_Touch_Thrsh, &current_threshold);
log_printf("thresholod %x \r\n",current_threshold);
// 计算新阈值（降低25%）
uint8_t new_threshold = current_threshold * 0.45;

// 设置新阈值
SetReg(TOUCH_I2C_Address, Button7_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button5_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button6_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button9_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button4_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button3_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button10_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button2_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button11_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button12_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button1_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button0_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button13_Touch_Thrsh, new_threshold);
SetReg(TOUCH_I2C_Address, Button8_Touch_Thrsh, new_threshold);
log_printf("Ch9 Thrsh: 0x%X -> 0x%X\n", current_threshold, new_threshold);
	
DDL_Delay1ms(1000);


//	while(1)
//	{
//		FM5114_Sensor_Debug(9);
//	}


	while(1)
	{	

		
	if(Touch_IRQ == 1)
	{

			//	GPIO_ResetBits(PORT_LED,PIN_LED0);
				while(GPIO_Read(PORT_TOUCH_IRQ, PIN_TOUCH_IRQ) == 0)//touch IRQ

				{
					
					do
					{
						GetReg(TOUCH_I2C_Address, Special_Irq, &reg_slide);
						GetReg(TOUCH_I2C_Address, Button_Touch_Irq1, &reg1);
						GetReg(TOUCH_I2C_Address, Button_Touch_Irq2, &reg2);
						GetReg(TOUCH_I2C_Address, Button_Release_Irq1, &reg3);
						GetReg(TOUCH_I2C_Address, Button_Release_Irq2, &reg4);

						if((reg2 & 0x80) != 0)                                 //LPCD定时中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq2, 0x7F); //清中断标志位			
							LPCD_counter++;
						}
						if((reg_slide & 0x80) != 0)                            //错误中断
						{
							log_printf("-> ERR irq!\r\n");
							SetReg(TOUCH_I2C_Address, Special_Irq, 0x7F);       //清中断标志位		
							//应重新初始化触摸功能
							FM5114_Touch_Reset();
			
							result = FM5114_Touch_SoftReset();
							if(result != FM5114_SUCCESS)		 
								log_printf("FM5114 TOUCH SoftReset Error\r\n");
							
							result = FM5114_Touch_Trim_Load();
							if(result != FM5114_SUCCESS)		 
								log_printf("FM5114 TOUCH Trim Load Error\r\n");
							
							Lpcd_Init_Register();                               //FM5114LPCD初始化函数

							result = FM5114_Touch_Init(FAST_MODE, SENSE ,BUTTON_CHANNEL, GROUP_BUTTON_EN, LPCD_TIMER_EN);//0x3FFF表示14个通道使能,bit0~bit13
							if(result == FM5114_SUCCESS)
								log_printf("-> Touch Reload Success\r\n");
							
							Touch_IRQ = 0;
							LPCD_IRQ = 0;
							DDL_Delay1ms(500);
//							Lpcd_Set_Mode(ENABLE); //LPCD使能
							Lpcd_Set_Mode(DISABLE); //LPCD使能
							DDL_Delay1ms(1);
							Scan_Start();  //启动扫描
							break;
							
						}
				
						if((reg1 & 0x01) != 0)                                 //按键S0的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0xFE); //清中断标志位				
							log_printf("-> S0 touching!\r\n");
								
						}
						if((reg1 & 0x02) != 0)                                 //按键S1的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0xFD); //清中断标志位
							log_printf("-> S1 touching!\r\n");
								
						}
						if((reg1 & 0x04) != 0)                                 //按键S2的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0xFB); //清中断标志位	
							log_printf("-> S2 touching!\r\n");
			
						}
						if((reg1 & 0x08) != 0)                                 //按键S3的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0xF7); //清中断标志位	
							log_printf("-> S3 touching!\r\n");
				
						}
						if((reg1 & 0x10) != 0)                                 //按键S4的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0xEF); //清中断标志位	
							log_printf("-> S4 touching!\r\n");
			
						}
						if((reg1 & 0x20) != 0)                                 //按键S5的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0xDF); //清中断标志位					
							log_printf("-> S5 touching!\r\n");
							Group_Handler(5, 1);
						}
						if((reg1 & 0x40) != 0)                                 //按键S6的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0xBF); //清中断标志位	
							log_printf("-> S6 touching!\r\n");
							Group_Handler(6, 1);
						}
						if((reg1 & 0x80) != 0)                                 //按键S7的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq1, 0x7F); //清中断标志位
							log_printf("-> S7 touching!\r\n");
						}

						if((reg2 & 0x01) != 0)                                 //按键S8的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq2, 0xFE); //清中断标志位
							log_printf("-> S8 touching!\r\n");
						}
						if((reg2 & 0x02) != 0)                                 //按键S9的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq2, 0xFD); //清中断标志位				
							log_printf("-> S9 touching!\r\n");
						}
						if((reg2 & 0x04) != 0)                                 //按键S10的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq2, 0xFB); //清中断标志位	
							log_printf("-> S10 touching!\r\n");
						}
						if((reg2 & 0x08) != 0)                                 //按键S11的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq2, 0xF7); //清中断标志位
							log_printf("-> S11 touching!\r\n");
						}
						if((reg2 & 0x10) != 0)                                 //按键S12的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq2, 0xEF); //清中断标志位	
							log_printf("-> S12 touching!\r\n");
						}
						if((reg2 & 0x20) != 0)                                 //按键S13的Touch中断
						{
							SetReg(TOUCH_I2C_Address, Button_Touch_Irq2, 0xDF); //清中断标志位	
							log_printf("-> S13 touching!\r\n");
						}
						if((reg3 & 0x01) != 0)                                  //按键S0的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0xFE);//清中断标志位	
							log_printf("-> S0 release!\r\n");
						}	
						if((reg3 & 0x02) != 0)                                  //按键S1的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0xFD);//清中断标志位
							log_printf("-> S1 release!\r\n");
						}
						if((reg3 & 0x04) != 0)                                  //按键S2的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0xFB);//清中断标志位	
							log_printf("-> S2 release!\r\n");
						}	
						if((reg3 & 0x08) != 0)                                  //按键S3的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0xF7);//清中断标志位	
							log_printf("-> S3 release!\r\n");
						}	
						if((reg3 & 0x10) != 0)                                  //按键S4的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0xEF);//清中断标志位	
							log_printf("-> S4 release!\r\n");
						}	
						if((reg3 & 0x20) != 0)                                  //按键S5的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0xDF);//清中断标志位					
							log_printf("-> S5 release!\r\n");
							Group_Handler(5, 2);
						}
						if((reg3 & 0x40) != 0)                                  //按键S6的release中断
						{	
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0xBF);//清中断标志位	
							log_printf("-> S6 release!\r\n");
							Group_Handler(6, 2);
						}	
						if((reg3 & 0x80) != 0)                                  //按键S7的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq1, 0x7F);//清中断标志位	
							log_printf("-> S7 release!\r\n");
						}	
						if((reg4 & 0x01) != 0)                                  //按键S8的release中断
						{		
							SetReg(TOUCH_I2C_Address, Button_Release_Irq2, 0xFE);//清中断标志位	
							log_printf("-> S8 release!\r\n");
						}			
						if((reg4 & 0x02) != 0)                                  //按键S9的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq2, 0xFD);//清中断标志位	
							log_printf("-> S9 release!\r\n");
						}	
						if((reg4 & 0x04) != 0)                                  //按键S10的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq2, 0xFB);//清中断标志位	
							log_printf("-> S10 release!\r\n");
						}	
						if((reg4 & 0x08) != 0)                                  //按键S11的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq2, 0xF7);//清中断标志位	
							log_printf("-> S11 release!\r\n");
						}
						if((reg4 & 0x10) != 0)                                  //按键S12的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq2, 0xEF);//清中断标志位	
							log_printf("-> S12 release!\r\n");
						}
						if((reg4 & 0x20) != 0)                                  //按键S13的release中断
						{
							SetReg(TOUCH_I2C_Address, Button_Release_Irq2, 0xDF);//清中断标志位
							log_printf("-> S13 release!\r\n");

						}
							
					}while(( reg1 | reg2 | reg3 | reg4) != 0);

					DDL_Delay1ms(4);
					GetReg(TOUCH_I2C_Address, LPCD_Timer_Config, &reg1);
					if((reg1 & 0x07) == 0x07)
					{
						SetReg(TOUCH_I2C_Address, Gpio1_Mode_Config, 0x82);					
						SetReg(TOUCH_I2C_Address, Irq_Repeat_Rate, 0x8A);
								
					}
					else
					{
						SetReg(TOUCH_I2C_Address, Gpio1_Mode_Config, 0x62);
						SetReg(TOUCH_I2C_Address, Irq_Repeat_Rate, 0x0A);
						
					}
					if(LPCD_counter == 14)
					{				
						SetReg(TOUCH_I2C_Address, Slide_Channel_Config, 0x40);            //Touch_suspend=1			
						DDL_Delay1ms(5);//等待5ms							
						LPCD_counter = 0;
						SetReg(TOUCH_I2C_Address, Gpio1_Mode_Config, 0x92);//LPCD启动
						DDL_Delay10us(3);
						
						SetReg(TOUCH_I2C_Address, Gpio1_Mode_Config, 0x82);
						
						DDL_Delay1ms(20);//等待20ms	
						SetReg(TOUCH_I2C_Address, Slide_Channel_Config, 0x00);            //Touch_suspend=0
					}			
				}
			
				Touch_IRQ = 0;
			DDL_Delay1ms(1);
			}
			
			if(LPCD_IRQ == 1)
				{
					//GPIO_ResetBits(PORT_LED,PIN_LED0);
					//Lpcd_Card_Event();
				Lpcd_Adc_Event();
					LPCD_IRQ = 0;		
				}
			//	GPIO_SetBits(PORT_LED,PIN_LED0);
			}
}

/**
 * @brief  HSI2C主机写配置
 * @retval None
 * @note   时序：
 *         HSI2C_ADDR_MD = HSI2C_ADDR_MD_7BIT, u8SubAddrSize = 0：
 *             Start + 7-bit Slave Address(W) + u8WriteData[TRANS_SIZE] + Stop
 *         HSI2C_ADDR_MD = HSI2C_ADDR_MD_7BIT, u8SubAddrSize = 2：
 *             Start + 7-bit Slave Address(W) + Sub Address(2bytes) + u8WriteData[TRANS_SIZE] + Stop
 *         HSI2C_ADDR_MD = HSI2C_ADDR_MD_10BIT, u8SubAddrSize = 1：
 *             Start + 1st 7bits of Slave Address(W) + 2nd 8bits of Slave Address + u8WriteData[TRANS_SIZE] + Stop
 */


/**
 * @brief  系统时钟配置
 * @retval None
 */
static void SysClockConfig(void)
{
    stc_sysctrl_clock_init_t stcSysClockInit = {0};

    /* 结构体初始化 */
    SYSCTRL_ClockStcInit(&stcSysClockInit);

    stcSysClockInit.u32SysClockSrc = SYSCTRL_CLK_SRC_RC48M_48M; /* 选择系统默认RC48M 48MHz作为Hclk时钟源 */
    stcSysClockInit.u32HclkDiv     = SYSCTRL_HCLK_PRS_DIV1;     /* Hclk 1分频 */
    SYSCTRL_ClockInit(&stcSysClockInit);                        /* 系统时钟初始化 */
}

/**
 * @brief  端口配置
 * @retval None
 */
static void GpioConfig(void)
{
    stc_gpio_init_t stcGpioInit = {0};

    /* 结构体初始化 */
    GPIO_StcInit(&stcGpioInit);

    /* 开启GPIO外设时钟 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA10(SDA)、PA11(SCL)端口 */
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_10 | GPIO_PIN_11;
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_OD;
    GPIOA_Init(&stcGpioInit);
    /* 设置PA10(SDA)、PA11(SCL)功能 */
    GPIO_PA10_AF_HSI2C_SDA();
    GPIO_PA11_AF_HSI2C_SCL();
}
/**
 * @brief  PortA中断服务函数
 * @retval None
 */
void PortA_IRQHandler(void)
{
    if (TRUE == GPIO_IntFlagGet(PORT_READER_IRQ, PIN_READER_IRQ)) /* 获取中断状态 */
    {
		
				LPCD_IRQ = 1;
//			log_printf("read irq\r\n");
				GPIO_IntFlagClear(PORT_READER_IRQ, PIN_READER_IRQ); /* 清除中断标志位 */
        
    }
		if (TRUE == GPIO_IntFlagGet(PORT_TOUCH_IRQ, PIN_TOUCH_IRQ)) /* 获取中断状态 */
    {
				Touch_IRQ = 1;		
//		log_printf("touch irq\r\n");
			GPIO_IntFlagClear(PORT_TOUCH_IRQ, PIN_TOUCH_IRQ); /* 清除中断标志位 */
			
		
        
    }
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
