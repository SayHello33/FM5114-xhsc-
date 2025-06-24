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
/* 	FM5114 LPCD API V2������																			  */
/* 	��Ҫ����:						        																		*/
/* 	����:       																										*/
/* 	����ʱ��:                     																	*/
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


// ȫ�����ò���
#define LPCD_THRESHOLD      0x13   // ���ȼ����ֵ (�Ƽ�ֵ24 0x18)
#define LPCD_RF_TIME        LPCD_RFTIME_25us  // RF���ʱ��
#define LPCD_REQA_TIME      LPCD_REQA_TIME_5ms  // REQA���ʱ��
#define LPCD_RX_GAIN        LPCD_RXGAIN_43DB  // ��������
#define LPCD_MIN_LEVEL      LPCD_MINLEVEL_9   // ��С����ƽ
#define LPCD_MOD_WIDTH      LPCD_MODWIDTH_38  // ���ƿ��
#define LPCD_TX_SCALE       LPCD_TXSCALE_4    // ���书�ʱ���
void log_printf(const char *fmt, ...);
unsigned char Lpcd_Card_Event(void);
//***********************************************
//�������ƣ�Lpcd_Init_Register()
//�������ܣ�LPCD�Ĵ�����ʼ������
//��ڲ�����
//���ڲ�����SUCCESS��������� 
//***********************************************
unsigned char Lpcd_Init_Register(void)
{
		

		FM5114_Reader_Reset();
		
		SetReg_Ext(READER_I2C_Address,0x24, 0x14);//�����޸�
		SetReg_Ext(READER_I2C_Address,0x25, 0x3A);//�����޸�
	
		uint8_t reg=0;

	while(reg==0x00)
	{
			SetReg_Ext(READER_I2C_Address,JREG_LPCDCTRLMODE, RF_DET_DISABLE|RF_DET_SEN_00|LPCD_ENABLE);//�����ʹ�ܣ������������0��LPCDʹ��
			GetReg_Ext(READER_I2C_Address,JREG_LPCDCTRLMODE,&reg);
		log_printf("JREG_LPCDCTRLMODE %x \r\n",reg);
	}
				
		
		SetReg_Ext(READER_I2C_Address,JREG_LPCDRFTIMER, LPCD_IRQINV_ENABLE|LPCD_IRQ_PUSHPULL|LPCD_RFTIME_5us);//̽��ʹ��5us��LPCD̽����ʱ��18us

			GetReg_Ext(READER_I2C_Address,JREG_LPCDRFTIMER,&reg);
		log_printf("JREG_LPCDRFTIMER %x \r\n",reg);

	SetReg_Ext(READER_I2C_Address,JREG_LPCDTHRESH_H,BIT5|BIT2|((LPCD_THRSH & 0xC0)>>6));//LPCD������ֵ��2λ
		SetReg_Ext(READER_I2C_Address,JREG_LPCDTHRESH_L, LPCD_THRSH & 0x3F);//LPCD������ֵ��6λ
		SetReg_Ext(READER_I2C_Address,JREG_LPCDTXCTRL2, LPCD_CWP);//����LPCD���P����
		SetReg_Ext(READER_I2C_Address,JREG_LPCDTXCTRL3, LPCD_CWN);//����LPCD���N����
		SetReg_Ext(READER_I2C_Address,JREG_LPCDREQATIMER,LPCD_REQA_TIME_5ms);//REQA����ز�ʱ��,5ms�����ֻ�����
		
		SetReg_Ext(READER_I2C_Address,JREG_LPCDREQAANA,LPCD_RXGAIN_43DB | LPCD_MINLEVEL_9 | LPCD_MODWIDTH_38);//����REQA�������Ľ������棬������ֵ�����ƿ��
		SetReg_Ext(READER_I2C_Address,JREG_LPCDDETECTMODE,LPCD_TXSCALE_4 | LPCD_COMBINE_MODE);//����REQA��ⷢ�书����̽�������ǿ�ı�����LCPD̽��ģʽ����
		return SUCCESS;

}
void Lpcd_Get_ADC1_Value(unsigned short* result)
{
    unsigned char reg_hi, reg_lo;
    unsigned short delta_value;
    
    // ��ȡ��8λ�͵�6λ
    GetReg_Ext(READER_I2C_Address, JREG_LPCDDELTA_HI, &reg_hi);
    GetReg_Ext(READER_I2C_Address, JREG_LPCDDELTA_LO, &reg_lo);
    
    // �ϲ�Ϊ10λֵ����2λ+��8λ��
    delta_value = (reg_hi & 0x03) << 8 | reg_lo;
    
    // ���������Ϣ
    log_printf("-> LPCD Delta is: %x %x\r\n",(unsigned char*)&delta_value + 1);
   
    log_printf("\r\n");
    
    // ͬʱ���ʵ����ֵ
    log_printf(" (%d",delta_value);

    log_printf(") \r\n");
    
    // ���ؽ��
    if(result) *result = delta_value;
}
void Test_LPCD_Performance()
{
    // ��ʼ��LPCD
    Lpcd_Init_Register();
    
    // ��ȡ��׼��ǿ
    unsigned short base_delta;
    Lpcd_Get_ADC1_Value(&base_delta);
    log_printf("-> Base Delta: %d",base_delta);
   
    log_printf("\r\n");
    
}
/**
 * @brief ����LPCD֧��B�����
 * @retval ����״̬: SUCCESS �� ERROR
 */
unsigned char Configure_LPCD_For_BCard(void)
{
    unsigned char result;
    
    // 1. ����LPCD����ģʽ
    uint8_t ctrlMode = RF_DET_ENABLE | RF_DET_SEN_00 | LPCD_ENABLE;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDCTRLMODE, ctrlMode);
    if (result != SUCCESS) return result;
    
    // 2. ����RF��ʱ��
    uint8_t rfTimer = LPCD_IRQINV_ENABLE | LPCD_IRQ_PUSHPULL | LPCD_RF_TIME;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDRFTIMER, rfTimer);
    if (result != SUCCESS) return result;
    
    // 3. ����LPCD��ֵ
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDTHRESH_H, 
                       BIT5 | BIT2 | ((LPCD_THRESHOLD & 0xC0) >> 6));
    if (result != SUCCESS) return result;
    
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDTHRESH_L, 
                       (LPCD_THRESHOLD)& 0x3F);
    if (result != SUCCESS) return result;
    
    // 4. ����REQA������
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDREQATIMER, LPCD_REQA_TIME);
    if (result != SUCCESS) return result;
    
    uint8_t reqaAna = LPCD_RX_GAIN | LPCD_MIN_LEVEL | LPCD_MOD_WIDTH;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDREQAANA, reqaAna);
		DDL_Delay1ms(3);
    if (result != SUCCESS) return result;
    
    // 5. ���ü��ģʽΪ���ȼ��
    uint8_t detectMode = LPCD_TX_SCALE | LPCD_RX_CHANGE_MODE;
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDDETECTMODE, detectMode);
    if (result != SUCCESS) return result;
    
    // 6. ����TX����
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDTXCTRL1, 
                        LPCD_TX2_ENABLE|LPCD_TX2_ENABLE);
    if (result != SUCCESS) return result;
    
    // 7. ���LPCD�жϱ�־
    result = SetReg_Ext(READER_I2C_Address, JREG_LPCDIRQ, 0xFF);
    
    return result;
}
//***********************************************
//�������ƣ�Lpcd_Set_Mode()
//�������ܣ�LCPD����ģʽ����
//��ڲ�����mode = ENABLE:����LPCD mode = DISABLE :�ر�LPCD
//���ڲ�����
//***********************************************
void Lpcd_Set_Mode(unsigned char mode)
{
	if(mode == ENABLE)
	{
			DDL_Delay1ms(1);
			GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0, ����LPCDģʽ
		DDL_Delay1ms(5);
			
	}
	else
	{					
			GPIO_SetBits(PORT_NRST,PIN_NRST);//NPD = 1, �˳�LPCDģʽ
			DDL_Delay1ms(1);
	}     
  return;
}

//***********************************************
//�������ƣ�Lpcd_Adc_Event()
//�������ܣ�LPCD�жϴ���
//��ڲ�����
//���ڲ�����
//***********************************************
void Lpcd_Adc_Event(void)
{
	unsigned char reg;

	while(GPIO_Read(PORT_READER_IRQ,PIN_READER_IRQ) == 0)
	{
		Lpcd_Set_Mode(DISABLE); 		//NPD = 1,FM5114�˳�����ģʽ	
		SetReg(TOUCH_I2C_Address, Slide_Channel_Config, 0x40);            //Touch_suspend=1			
		DDL_Delay1ms(5);
		
		log_printf ("-> LPCD Detceted!\r\n ");
		GetReg_Ext(READER_I2C_Address,JREG_LPCDIRQ, &reg);//��ȡLPCD�жϱ�־
		SetReg_Ext(READER_I2C_Address,JREG_LPCDIRQ, reg);//CLEAR LPCD IRQ
		
		log_printf("->JREG_LPCDIRQ: %x",reg);log_printf("\r\n");
		
		if(reg & 0x04)//RXchange��־
		{ 
			log_printf("-> LPCD IRQ RXCHANGE SET!\r\n");//LPCD�жϱ�־			
			Lpcd_Get_ADC_Value();//����ʱʹ�ã���ȡLPCD̽��ֵ				
		}
		if(reg & 0x02)//Ѱ������־
		{
			log_printf("-> LPCD IRQ ATQAREC SET!\r\n");
		}
		if(reg & 0x08)//������־
		{
			log_printf("-> LPCD IRQ RFDET SET!\r\n");
		}
		if((reg & 0x08) ||(reg & 0x04)||(reg & 0x02))
		{
			uint8_t result=Lpcd_Card_Event();
//			if(result!=SUCCESS)
//					Lpcd_Card_B_Event();         //��������

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
//�������ܣ�Lpcd_Get_ADC_Value��ȡLPCD��ADC����
//��ڲ�����
//���ڲ�����
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
//�������ƣ�Lpcd_Card_Event()
//�������ܣ�LPCD�������������
//��ڲ�����
//���ڲ�����
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