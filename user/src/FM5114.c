#include "DEFINE.h"
#include "FM5114_REG.h"
#include "gpio.h"
#include "hsi2c.h"
#include "lpm.h"
#include "lpuart.h"
#include "sysctrl.h"
#include "stdio.h"
#include "uart.h"
#include "uart.h"
#include "READER_API.h"
#include "FM5114.h"
//***************************************************************************************//
//����˵��
//V7�汾��1������Csֵ�Ĳ�ͬ�����컯���ô�����ֵ
//				2������INIT_MODE��FAST_MODE, ���Ӵ���У׼����������ָ�����
//				3�����Ӵ���ͨ���޸Ĵ������ڹ��ܣ��û����Բο��ù���������������ɨ������
//***************************************************************************************//
unsigned char Touch_IRQ;
unsigned char LPCD_IRQ;

unsigned char FM5114_TOUCH_CS_LUT[50]={1,3,4,5,7,8,10,11,12,14,15,16,18,19,20,22,23,24,26,27,29,30,31,33,34,35,37,38,39,41,42,44,45,46,48,49,50,52,53,54,56,57,58,60};
unsigned char FM5114_TOUCH_SEN_LUT1[60]={10,11,11,12,12,12,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
unsigned char FM5114_TOUCH_SEN_LUT2[60]={9,10,10,11,11,11,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15};
unsigned char FM5114_TOUCH_SEN_LUT3[60]={9,9,10,10,10,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14};
unsigned char FM5114_TOUCH_SEN_LUT4[60]={9,9,9,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14};
void log_printf(const char *fmt, ...);
	
#define BAUD_RATE   (1000000) /* ������ */
#define DEVICE_ADDR (0x28u)   /* �ӻ��豸��ַ */
#define TRANS_SIZE  (30)      /* �����ֽ��� */
									 
unsigned char FM5114_TOUCH_REGVAL[] = {0x01, 0x00, 0x00, 0xF0, 0x03, 0xF0, 0x04, 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF0, 0x00,//BUTTON0
										0x02, 0x00, 0x01, 0x0F, 0x30, 0x0F, 0x40, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x0F, 0x04,//BUTTON1
										0x04, 0x00, 0x02, 0xF0, 0x03, 0xF0, 0x04, 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF0, 0x00,//BUTTON2
										0x08, 0x00, 0x03, 0x0F, 0x30, 0x0F, 0x40, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x0F, 0x04,//BUTTON3
										0x10, 0x00, 0x04, 0xF0, 0x03, 0xF0, 0x04, 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF0, 0x00,//BUTTON4
										0x20, 0x00, 0x05, 0x0F, 0x30, 0x0F, 0x40, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x0F, 0x04,//BUTTON5
										0x40, 0x00, 0x06, 0xF0, 0x03, 0xF0, 0x04, 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF0, 0x00,//BUTTON6
										0x80, 0x00, 0x07, 0x0F, 0x30, 0x0F, 0x40, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x0F, 0x04,//BUTTON7
										0x00, 0x01, 0x08, 0xF0, 0x03, 0xF0, 0x04, 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF0, 0x00,//BUTTON8
										0x00, 0x02, 0x09, 0x0F, 0x30, 0x0F, 0x40, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x0F, 0x04,//BUTTON9
										0x00, 0x04, 0x0A, 0xF0, 0x03, 0xF0, 0x04, 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF0, 0x00,//BUTTON10
										0x00, 0x08, 0x0B, 0x0F, 0x30, 0x0F, 0x40, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x0F, 0x04,//BUTTON11
										0x00, 0x10, 0x0C, 0xF0, 0x03, 0xF0, 0x04, 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF0, 0x00,//BUTTON12
										0x00, 0x20, 0x0D, 0x0F, 0x30, 0x0F, 0x40, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x0F, 0x04 //BUTTON13
									 };

unsigned char FM5114_TOUCH_SENSEMODE_REGVAL[] = {0x0F, 0x01, 0x08,//BUTTON0
												 0xF0, 0x10, 0x80,//BUTTON1
												 0x0F, 0x01, 0x08,//BUTTON2
												 0xF0, 0x10, 0x80,//BUTTON3
	                       0x0F, 0x01, 0x08,//BUTTON4
												 0xF0, 0x10, 0x80,//BUTTON5
	                       0x0F, 0x01, 0x08,//BUTTON6
												 0xF0, 0x10, 0x80,//BUTTON7
	                       0x0F, 0x01, 0x08,//BUTTON8
												 0xF0, 0x10, 0x80,//BUTTON9
	                       0x0F, 0x01, 0x08,//BUTTON10
												 0xF0, 0x10, 0x80,//BUTTON11
												 0x0F, 0x01, 0x08,//BUTTON12
												 0xF0, 0x10, 0x80 //BUTTON13
												};

												
unsigned char Target_Signal_High[] = {0x0A, 0x09, 0x1B, 0x18, 0x16, 0x14, 0x14, 0x13, 0x13, 0x36,
													 0x31, 0x31, 0x2D, 0x2D, 0x2B, 0x29, 0x28, 0x28, 0x26, 0x26,
													 0x25, 0x25, 0x25, 0x6C, 0x6C, 0x67, 0x62, 0x62, 0x62, 0x5A,
													 0x5E, 0x5A, 0x5A, 0x57, 0x53, 0x53, 0x53, 0x50, 0x53, 0x50,
													 0x50, 0x4D, 0x4B, 0x4D, 0x4B, 0x4B, 0x4B, 0x4D, 0x4D, 0x4B,
	                         0x4B, 0x48, 0x48, 0x48, 0x48, 0x48
													};

unsigned char Target_Signal_Low[] =  {0x76, 0xB7, 0x33, 0xBA, 0xAB, 0xEC, 0xEC, 0x6E, 0x6E, 0x66,
													 0x74, 0x74, 0x55, 0x55, 0x85, 0xD9, 0x4C, 0x4C, 0xDB, 0xDB,
													 0x84, 0x84, 0x84, 0xCD, 0xCD, 0x9E, 0xE9, 0xE9, 0xE9, 0xAB,
													 0x9C, 0xAB, 0xAB, 0x0A, 0xB1, 0xB1, 0xB1, 0x98, 0xB1, 0x98,
													 0x98, 0xB7, 0x09, 0xB7, 0x09, 0x09, 0x09, 0xB7, 0xB7, 0x09,
	                         0x09, 0x89, 0x89, 0x89, 0x89, 0x89
													};	
unsigned char CsButton[14] = {0};
unsigned char Num;
unsigned char Button_InitReg[14][4] = {0};
// ȫ��I2C����
static stc_hsi2c_master_init_t stcHsi2cInit;
static bool i2cInitialized = false;
#define TIMEOUT 1000
////===========2022 1108 V8.0���£�������ֵ����Ϊ�ĵ���ֵ=====================
////����Ŀ�ģ�ϸ����ֵ���ã�
////������ֵ����ʹ�õ��Թ��߲�����������ֵ�����ó�
//unsigned char Button_Thrsh_H[14]  = { 92, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128};//������ֵ��ߵ�
//unsigned char Button_Thrsh_M1[14] = { 76, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};//������ֵ�θߵ�
//unsigned char Button_Thrsh_M2[14] = { 64,  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  90}; //������ֵ�е� 
//unsigned char Button_Thrsh_L[14]  = { 57,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80}; //������ֵ�͵�
////=============================================================================


unsigned char SenseModeInit(unsigned int channel);

#define SENSE_MODE         1 /* ������������ǿģʽ���� */
#define GROUP_BUTTON_EN      0

void Group_Handler(unsigned char Channel, unsigned char Condition)
{
	if(GROUP_BUTTON_EN == 1)
	{
		switch(Condition)
		{
			case 1:				
				if(Channel == 5)
				{
					log_printf("-> Group button S5 touching!\r\n");
				}
				if(Channel == 6)
				{
					log_printf("-> Group button S6 touching!\r\n");
				}
			break;
				
			case 2:
				if(Channel == 5)
				{
				
					log_printf("-> Group button S5 release!\r\n");
				}
				if(Channel == 6)
				{
		
					log_printf("-> Group button S6 release!\r\n");
				}
			break;
		}
	}
}

void i2c_init()
{
		if(i2cInitialized) return;
    
    /* �ṹ���ʼ�� */
    HSI2C_MasterStcInit(&stcHsi2cMasterInit);

    /* ����HSI2C����ʱ�� */
    SYSCTRL_PeriphClockEnable(PeriphClockHsi2c);

    stcHsi2cMasterInit.bResetBeforeInit = TRUE;   /* ��ʼ��ǰ��λ */
    stcHsi2cMasterInit.u32EnableDebug   = HSI2C_MASTER_DEBUG_OFF;
    stcHsi2cMasterInit.u32BaudRateHz    = BAUD_RATE; /* ������ */
    
    // 7λ��ַģʽ����
    stcHsi2cMasterInit.u8SubAddrSize = 0;          // ��ʹ���ӵ�ַ
    
    // ͨ������
    stcHsi2cMasterInit.stcMasterConfig2.u32SdaFilterEnable = HSI2C_MASTER_FILTBPSDA_ENABLE;
    stcHsi2cMasterInit.stcMasterConfig2.u32SclFilterEnable = HSI2C_MASTER_FILTBPSCL_ENABLE;

    if (Ok != HSI2C_MasterInit(HSI2C, &stcHsi2cMasterInit, SystemCoreClock))
    {
        // ��ʼ��ʧ�ܴ���
        i2cInitialized = false;
				log_printf("i2c init fail\r\n");
        return;
    }
    
    i2cInitialized = true;


}

// ����I2C�ӻ���ַ�ʹ��䷽��
static void I2C_Configure(uint8_t devAddr, bool isReadOperation)
{
    if(!i2cInitialized) i2c_init();
    
    // ����7λ�ӻ���ַ
    stcHsi2cMasterInit.u8SlaveAddr = devAddr;
    
    // ���ô��䷽��
    stcHsi2cMasterInit.enDir = isReadOperation ? 
                               Hsi2cMasterReadSlaveWrite : 
                               Hsi2cMasterWriteSlaveRead;

    // Ӧ������
    if (Ok != HSI2C_MasterInit(HSI2C, &stcHsi2cMasterInit, SystemCoreClock))
    {
        // ������ - ����������Ի���־��¼
    }
}

void FM5114_Reader_Reset(void)
{	
	GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0	
	DDL_Delay1ms(1);		
	GPIO_SetBits(PORT_NRST,PIN_NRST);//NPD = 1		
	DDL_Delay1ms(1);
	return;
}


unsigned char FM5114_Reader_SoftReset(void)
{	
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_SOFT_RESET);
	DDL_Delay1ms(1);//readerоƬ��λ��Ҫ1ms
	GetReg(READER_I2C_Address,JREG_COMMAND,&reg_data);
	if(reg_data == 0x20)
		return FM5114_SUCCESS;
	else
		return FM5114_COMM_ERR;
}

//*************************************
//����  ����SetCW �ز�����
//��ڲ�����
//���ڲ�����
//*************************************

void SetCW(unsigned char cw_mode)
{
	if(cw_mode == TX1_TX2_CW_DISABLE)	
		{
		ModifyReg(READER_I2C_Address,JREG_TXCONTROL,JBIT_TX1RFEN|JBIT_TX2RFEN,RESET);
		}
	if(cw_mode == TX1_CW_ENABLE)	
		{
		ModifyReg(READER_I2C_Address,JREG_TXCONTROL,JBIT_TX1RFEN,SET);
		ModifyReg(READER_I2C_Address,JREG_TXCONTROL,JBIT_TX2RFEN,RESET);	
		}
	if(cw_mode == TX2_CW_ENABLE)	
		{
		ModifyReg(READER_I2C_Address,JREG_TXCONTROL,JBIT_TX1RFEN,RESET);
		ModifyReg(READER_I2C_Address,JREG_TXCONTROL,JBIT_TX2RFEN,SET);	
		}
	if(cw_mode == TX1_TX2_CW_ENABLE)	
		{
		ModifyReg(READER_I2C_Address,JREG_TXCONTROL,JBIT_TX1RFEN|JBIT_TX2RFEN,SET);
		}
		DDL_Delay1ms(10);
}

void FM5114_Touch_Reset(void)             //FM5114 TOUCH��λ����
{
	GPIO_ResetBits(PORT_XRST,PIN_XRST);
	DDL_Delay100us(2);
	GPIO_SetBits(PORT_XRST,PIN_XRST);
	DDL_Delay1ms(5);
}	




//***********************************************
//�������ƣ�GetReg(unsigned char device_address,unsigned char reg_address,unsigned char *reg_data)
//�������ܣ���ȡ�Ĵ���ֵ
//��ڲ�����device_address��������ַ��reg_address���Ĵ�����ַ��reg_data����ȡ��ֵ
//���ڲ�����FM5114_SUCCESS���ɹ�   FM5114_COMM_ERR:ʧ��
//***********************************************
unsigned char GetReg(unsigned char device_address,unsigned char reg_address,unsigned char *reg_data)
{	
		en_result_t result;
    
    // �׶�1: д��Ĵ�����ַ
    I2C_Configure(device_address, false);
    result = HSI2C_MasterTransferPoll(HSI2C, &reg_address, 1, TIMEOUT);
    if (Ok != result) {
        return FM5114_COMM_ERR;
    }
    
    // �׶�2: ��ȡ�Ĵ���ֵ
    I2C_Configure(device_address, true);
    result = HSI2C_MasterTransferPoll(HSI2C, reg_data, 1, TIMEOUT);
    if (Ok != result) {
        return FM5114_COMM_ERR;
    }
    
    return FM5114_SUCCESS;
}




//***********************************************
//�������ƣ�SetReg(unsigned char device_address,unsigned char reg_address,unsigned char reg_data)
//�������ܣ�д�Ĵ���
//��ڲ�����device_address��������ַ��reg_address���Ĵ�����ַ��reg_data����ȡ��ֵ
//���ڲ�����FM5114_SUCCESS���ɹ�   FM5114_COMM_ERR:ʧ��
//***********************************************
unsigned char SetReg(unsigned char device_address,unsigned char reg_address,unsigned char reg_data)
{
	 // ׼�����͵�����: [�Ĵ�����ַ, ����ֵ]
    uint8_t txData[2] = {reg_address, reg_data};
    
    // ����I2CΪдģʽ
    I2C_Configure(device_address, false);
    
    // ִ�д���
    en_result_t result = HSI2C_MasterTransferPoll(HSI2C, txData, sizeof(txData), TIMEOUT);
    
    return (Ok == result) ? FM5114_SUCCESS : FM5114_COMM_ERR;
}



//*******************************************************
//�������ƣ�ModifyReg(unsigned char addr,unsigned char* mask,unsigned char set)
//�������ܣ�д�Ĵ���
//��ڲ�����addr:Ŀ��Ĵ�����ַ   mask:Ҫ�ı��λ  
//         set:  0:��־��λ����   ����:��־��λ����
//���ڲ�����
//********************************************************
void ModifyReg(unsigned char device_address,unsigned char reg_address,unsigned char mask,unsigned char set)
{
	unsigned char reg_data;
	
	GetReg(device_address,reg_address,&reg_data);
	
		if(set)
		{
			reg_data |= mask;
		}
		else
		{
			reg_data &= ~mask;
		}

	SetReg(device_address,reg_address,reg_data);
	return ;
}



 unsigned char Reader_SoftReset(void)
{	
	unsigned char reg_data;
	SetReg(READER_I2C_Address,JREG_COMMAND,CMD_SOFT_RESET);
	DDL_Delay1ms(1);//FM175XXоƬ��λ��Ҫ1ms
	GetReg(READER_I2C_Address,JREG_COMMAND,&reg_data);
	if(reg_data == 0x20)
		return FM5114_SUCCESS;
	else
		return FM5114_COMM_ERR;
}  
//***********************************************
//�������ƣ�GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//�������ܣ���ȡ��չ�Ĵ���ֵ
//��ڲ�����ExtRegAddr:��չ�Ĵ�����ַ   ExtRegData:��ȡ��ֵ
//���ڲ�����unsigned char  TRUE����ȡ�ɹ�   FALSE:ʧ��
//***********************************************
unsigned char GetReg_Ext(unsigned char device_address,unsigned char ext_reg_address,unsigned char* ext_reg_data)
{
	SetReg(device_address,JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_RD_ADDR + ext_reg_address);
	GetReg(device_address,JREG_EXT_REG_ENTRANCE,&(*ext_reg_data));
	return FM5114_SUCCESS;	
}
//***********************************************
//�������ƣ�SetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//�������ܣ�д��չ�Ĵ���
//��ڲ�����ExtRegAddr:��չ�Ĵ�����ַ   ExtRegData:Ҫд���ֵ
//���ڲ�����unsigned char  TRUE��д�ɹ�   FALSE:дʧ��
//***********************************************
unsigned char SetReg_Ext(unsigned char device_address,unsigned char ext_reg_address,unsigned char ext_reg_data)
{
	SetReg(device_address,JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_ADDR + ext_reg_address);

	SetReg(device_address,JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_DATA + ext_reg_data);
	return FM5114_SUCCESS; 	
}
 
//*******************************************************
//�������ƣ�ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char* mask,unsigned char set)
//�������ܣ��Ĵ���λ����
//��ڲ�����ExtRegAddr:Ŀ��Ĵ�����ַ   mask:Ҫ�ı��λ  
//         set:  0:��־��λ����   ����:��־��λ����
//���ڲ�����unsigned char  TRUE��д�ɹ�   FALSE:дʧ��
//********************************************************
void ModifyReg_Ext(unsigned char device_address,unsigned char ExtRegAddr,unsigned char mask,unsigned char set)
{
  unsigned char regdata;
	
	GetReg_Ext(device_address,ExtRegAddr,&regdata);
	
		if(set)
		{
			regdata |= mask;
		}
		else
		{
			regdata &= ~(mask);
		}
	
	SetReg_Ext(device_address,ExtRegAddr,regdata);
	return;
}

void I2C_Write_FIFO(unsigned char reglen,unsigned char* regbuf)  //IIC�ӿ�����дFIFO�Ĵ��� 
{
	 // 1. �����������ݻ��������Ĵ�����ַ + FIFO���ݣ�
    uint8_t writeBuf[reglen + 1];
    writeBuf[0] = 0x3F & JREG_FIFODATA; // �Ĵ�����ַ
    
    // 2. ����FIFO���ݵ�������
    memcpy(&writeBuf[1], regbuf, reglen);
	 // 4. ִ������д��
    en_result_t result = HSI2C_MasterTransferPoll(HSI2C, 
                                                 writeBuf, 
                                                 sizeof(writeBuf),
                                                 1000);
    
    // 5. ��������ѡ��
    if (Ok != result)
    {
        // �������߼�
        log_printf("FIFO write error!");
        return;
    }
	
	
	return;
}
#define DEVICE_FOUND     1
#define DEVICE_NOT_FOUND 0
// ���ָ�� I2C ��ַ�Ƿ�����豸
uint8_t I2C_DeviceDetect(uint8_t device_address)
{
    en_result_t result;
    uint8_t dummy;
    
    // ���� I2C Ϊдģʽ����ַ���ֻ��Ҫ��ַ�׶Σ�
    I2C_Configure(device_address, false);
    
    // ���Է��͵�ַ��0 �ֽ����ݴ��䣩�ȴ�ACK
    result = HSI2C_MasterTransferPoll(HSI2C, &dummy, 0, TIMEOUT);
    
    // �豸���ڵ��ҽ�������ֵΪ Ok
    return (result == Ok) ? 1 : 0;
}

// ɨ�����п��ܵ� I2C ��ַ (0x08 - 0x77)
void I2C_ScanAllDevices(void)
{
    log_printf("Starting I2C device scan...\r\n");
    log_printf("Address: 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07\r\n");
    
    uint8_t foundCount = 0;
    uint8_t addresses[128] = {0}; // ��¼�ҵ����豸��ַ
    
    // ɨ�� 0x08 - 0x77 (��Ч�� 7 λ I2C ��ַ��Χ)
    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        // ÿ����ʾ 8 ����ַ
        if ((addr & 0x07) == 0) {
            log_printf("0x%X: ", addr & 0xF8);
        }
        
        if (I2C_DeviceDetect(addr)) {
            log_printf("[%X] ", addr);
            addresses[foundCount++] = addr;
        } else {
            log_printf(" --  ");
        }
        
        // ÿ�н�������
        if ((addr & 0x07) == 0x07) {
            log_printf("\r\n");
        }
    }
    
    log_printf("\r\nScan complete. Found %d devices:\r\n", foundCount);
    for (uint8_t i = 0; i < foundCount; i++) {
        log_printf("Device %d at 0x%X\r\n", i+1, addresses[i]);
    }
}


// ��ʼ�� I2C ��ɨ���豸��ʹ��ʾ��
void Init_I2C_And_Scan(void)
{
    // 1. ��ʼ�� I2C
    i2c_init();
    log_printf("I2C initialized. Starting device scan...\r\n");
    
    // 2. ɨ�������豸
    I2C_ScanAllDevices();
    
    // 3. ���Ԥ�ڵ� FM5114 �Ƿ����
    if (!I2C_DeviceDetect(TOUCH_I2C_Address)) {
        log_printf("ERROR: FM5114 not found at 0x%X!\r\n", TOUCH_I2C_Address);
        // ���������Ӵ�������������ָʾ�ƾ���
    }
		if (!I2C_DeviceDetect(READER_I2C_Address)) {
        log_printf("ERROR: FM5114 not found at 0x%X!\r\n", READER_I2C_Address);
        // ���������Ӵ�������������ָʾ�ƾ���
    }
}
// ���ʾ��
void CheckFM5114Presence(void)
{
    if (I2C_DeviceDetect(READER_I2C_Address) == 1) {
        log_printf("FM5114 detected on address 0x%X\n", READER_I2C_Address);
    } else {
        log_printf("Error: FM5114 not found at 0x%X\n", READER_I2C_Address);
        // ���������Ӵ������߼�
        while(1); // ʾ����ִֹͣ��
    }
}
void I2C_Read_FIFO(unsigned char reglen,unsigned char* regbuf)  //I2C�ӿ�������FIFO
{
	 // 4. ִ��������ȡ
    en_result_t result = HSI2C_MasterTransferPoll(HSI2C, 
                                                 regbuf, 
                                                 reglen,
                                                 1000);
    
    // 5. �ָ�ԭʼ���ã���Ҫ����
    //stcHsi2cMasterInit = local_config_backup;
    
    // 6. ������
    if (Ok != result)
    {
     //   I2C_ErrorHandler("FIFO read error");
    }
}

unsigned char Scan_Start(void)
{
	unsigned char reg;
	GetReg(TOUCH_I2C_Address, Scan_Ctrl,&reg);
  SetReg(TOUCH_I2C_Address, Scan_Ctrl, reg|0x80);

		return FM5114_SUCCESS;
}

unsigned char TrimLoad_PowerOn(void)
{
	unsigned char reg;
	
	//DDL_Delay1ms(10);//10msɾ��2022 0817
							//Trim����ʹ���ڲ�LDO���磬��ȷ��VCC��ѹ=1.35V~1.65V
	
	if(GetReg(TOUCH_I2C_Address, 0x5F, &reg) != FM5114_SUCCESS)  
		return FM5114_COMM_ERR;	
	if(SetReg(TOUCH_I2C_Address, 0x5F, reg | 0x01) != FM5114_SUCCESS)           //Trim�ϵ�
		return FM5114_COMM_ERR;
	
	return FM5114_SUCCESS;

}

unsigned char TrimLoad_PowerOff(void)
{
	unsigned char reg;
	
	DDL_Delay1ms(1);//1ms 20220817
	
	if(GetReg(TOUCH_I2C_Address, 0x5F, &reg) != FM5114_SUCCESS)  
		return FM5114_COMM_ERR;	
	if(SetReg(TOUCH_I2C_Address, 0x5F, reg & 0xFE) != FM5114_SUCCESS)           //TrimLoad�µ�
		return FM5114_COMM_ERR;
	
	return FM5114_SUCCESS;

}


unsigned char FM5114_Touch_Trim_Load(void)
{
	unsigned char reg=0;

	TrimLoad_PowerOn();
	
	DDL_Delay1ms(10);
	if(GetReg(TOUCH_I2C_Address, 0x5E, &reg) != FM5114_SUCCESS)  
		return FM5114_COMM_ERR;	
	if(SetReg(TOUCH_I2C_Address, 0x5E, reg | 0x80) != FM5114_SUCCESS)           //��ʼ����Trim
		return FM5114_COMM_ERR;	
	DDL_Delay1ms(1);		
	if(GetReg(TOUCH_I2C_Address, 0x5E, &reg) != FM5114_SUCCESS)  
		return FM5114_COMM_ERR;

	
	if((reg & 0x01) != 0x00)
	{
		TrimLoad_PowerOff();	
		return FM5114_SUCCESS;
	}
	else
	{
		TrimLoad_PowerOff();
		return FM5114_COMM_ERR;
	}

}


unsigned char FM5114_Touch_SoftReset(void)
{
	unsigned char reg_data,result;
	result = GetReg(TOUCH_I2C_Address,Scan_Ctrl,&reg_data);
	
	result = SetReg(TOUCH_I2C_Address,Scan_Ctrl, reg_data|0x20);

	DDL_Delay1ms(1);

	return result;
}


char FM5114_Touch_Calibrate(void)
{
	unsigned char reg_data;
	if(GetReg(TOUCH_I2C_Address,0x7E,&reg_data) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	if(SetReg(TOUCH_I2C_Address,0x7E,reg_data|0x80) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	return FM5114_SUCCESS;
}
unsigned char FM5114_Touch_Button_Auto_Tuning(unsigned char Sensitivity, unsigned int channel)                           //�������ݼ��,����������
{
	unsigned char Cs,CsTemp;  
	unsigned char N_Sens;                                                                 
	unsigned char reg,reg1;
	unsigned char i;
	unsigned char Num;
	unsigned char reg_addr1, reg_addr2, reg_addr3, reg_addr4;
	unsigned int Target_temp;


	reg_addr1 = 0x9D +(channel/2);
	reg_addr2 = 0x95 +(channel/2);
	reg_addr3 = 0xA6 + channel;
	reg_addr4 = 0xB7 + channel;	
	
	if(SetReg(TOUCH_I2C_Address, 0x08, FM5114_TOUCH_REGVAL[(channel*14)+0]) != FM5114_SUCCESS) //s0_en--s7_en ����������0ʹ��
		return FM5114_COMM_ERR;
	if(SetReg(TOUCH_I2C_Address, 0x09, FM5114_TOUCH_REGVAL[(channel*14)+1]) != FM5114_SUCCESS) //s8_en--s15_en
		return FM5114_COMM_ERR;

	if(SetReg(TOUCH_I2C_Address, 0x7E, FM5114_TOUCH_REGVAL[(channel*14)+2]) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	
	if(SetReg(TOUCH_I2C_Address, 0x0B, 0x80) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	
	if(GetReg(TOUCH_I2C_Address, reg_addr1, &reg) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	if(SetReg(TOUCH_I2C_Address, reg_addr1, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+3]) | FM5114_TOUCH_REGVAL[(channel*14)+4])) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
			
	if(GetReg(TOUCH_I2C_Address, reg_addr2, &reg) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	if(SetReg(TOUCH_I2C_Address, reg_addr2, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+5]) | FM5114_TOUCH_REGVAL[(channel*14)+6])) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;

	if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0D) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	if(SetReg(TOUCH_I2C_Address, 0x8B, 0x9A) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	
	if(FM5114_Touch_Calibrate() != FM5114_SUCCESS)
		return FM5114_COMM_ERR;

	for(i = 0; i < 25; i++)
	{
		DDL_Delay1ms(2);
		if(GetReg(TOUCH_I2C_Address, 0x87,&reg) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
		
		if((reg&0x80)==0x80)
		{
				break;
		}	
	}

	if((reg&0x80)!=0x80)   	
	{
		return FM5114_CALI_ERR;
	}
	
	if(GetReg(TOUCH_I2C_Address, 0x88, &Num) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	if(FM5114_Touch_SoftReset() != FM5114_SUCCESS)					                                 //soft_reset=1
		return FM5114_COMM_ERR;
	
	if(Num >= 0x2D)                                                              //�����������
	{
		Num = 0x2C;
	}
	
	Cs = FM5114_TOUCH_CS_LUT[Num-1];
	CsTemp = Cs;
	
	if(Cs<=10)
	{
		if(GetReg(TOUCH_I2C_Address, reg_addr1,&reg) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, reg_addr1, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+7]) | FM5114_TOUCH_REGVAL[(channel*14)+8])) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}
	else if(Cs<=27)
	{
		if(GetReg(TOUCH_I2C_Address, reg_addr1,&reg) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, reg_addr1, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+7]) | FM5114_TOUCH_REGVAL[(channel*14)+9])) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}
	else if(Cs>27 && Cs<=50)
	{
		if(GetReg(TOUCH_I2C_Address, reg_addr1,&reg) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, reg_addr1, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+7]) | FM5114_TOUCH_REGVAL[(channel*14)+10])) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}
	else if(Cs>50 && Cs<=60)
	{
		if(GetReg(TOUCH_I2C_Address, reg_addr1,&reg) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, reg_addr1, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+7]) | FM5114_TOUCH_REGVAL[(channel*14)+11])) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}
	
	
		if(Cs == 5 || Cs == 20)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0B) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0x3D) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}
	else if(Cs == 7 || Cs == 12)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0B) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0x0F) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 8 || Cs == 31 || Cs == 37 || Cs == 46 || Cs == 48 || Cs == 56)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0C) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0x5D) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}	
	else if(Cs == 10 || Cs == 35)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0C) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0x09) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 14 || Cs == 52)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0B) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0xEE) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 16 || Cs == 29)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0B) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0x26) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 18 || Cs == 33)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0B) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0xD3) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 22 || Cs == 26 || Cs == 27 || Cs == 58)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0D) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0x9A) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 24)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0C) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0xB6) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 39 || Cs == 50 || Cs == 54 || Cs == 60)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0C) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0x25) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 41)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0B) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0xB9) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	else if(Cs == 42 || Cs == 44)
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0B) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0xA0) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;		
	}
	

		if(FM5114_Touch_Calibrate() != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	
	
	
	for(i = 0; i < 25; i++)
	{
		DDL_Delay1ms(2);
		if(GetReg(TOUCH_I2C_Address, 0x87,&reg) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
		
		if((reg&0x80)==0x80)
		{
				break;
		}	
	}

	if((reg&0x80)!=0x80)   	
	{
		return FM5114_CALI_ERR;
	}
		
	if(GetReg(TOUCH_I2C_Address, 0x88, &reg) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	
	switch(Cs)
	{
		case 5:
			if(reg >= 20)
				CsTemp = 6;
			else
				CsTemp = 5;
		break;
			
		case 7:
			if(reg <= 23)
				CsTemp = 6;
			else
				CsTemp = 7;
		break;
			
		case 8:
			if(reg >= 28)
				CsTemp = 9;
			else
				CsTemp = 8;
		break;
			
		case 10:
			if(reg <= 31)
				CsTemp = 9;
			else
				CsTemp = 10;
		break;

		case 12:
			if(reg >= 23)
				CsTemp = 13;
			else
				CsTemp = 12;
		break;
			
		case 14:
			if(reg <= 22)
				CsTemp = 13;
			else
				CsTemp = 14;
		break;
			
		case 16:
			if(reg >= 30)
				CsTemp = 17;
			else
				CsTemp = 16;
		break;
			
		case 18:
			if(reg <= 29)
				CsTemp = 17;
			else
				CsTemp = 18;
		break;
			
		case 20:
			if(reg >= 37)
				CsTemp = 21;
			else
				CsTemp = 20;
		break;
			
		case 22:
			if(reg <= 31)
				CsTemp = 21;
			else
				CsTemp = 22;
		break;
			
		case 24:
			if(reg >= 39)
				CsTemp = 25;
			else
				CsTemp = 24;
		break;
			
		case 26:
			if(reg <= 37)
				CsTemp = 25;
			else
				CsTemp = 26;
		break;
			
		case 27:
			if(reg >= 41)
			{
				CsTemp = 28;
				if(GetReg(TOUCH_I2C_Address, reg_addr1,&reg) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;
				if(SetReg(TOUCH_I2C_Address, reg_addr1, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+7]) | FM5114_TOUCH_REGVAL[(channel*14)+10])) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;				
			}
			else
				CsTemp = 27;
		break;
			
		case 29:
			if(reg <= 25)
				CsTemp = 28;
			else
				CsTemp = 29;
		break;
			
		case 31:
			if(reg >= 26)
				CsTemp = 32;
			else
				CsTemp = 31;
		break;
			
		case 33:
			if(reg <= 27)
				CsTemp = 32;
			else
				CsTemp = 33;
		break;
			
		case 35:
			if(reg >= 30)
				CsTemp = 36;
			else
				CsTemp = 35;
		break;
			
		case 37:
			if(reg <= 29)
				CsTemp = 36;
			else
				CsTemp = 37;
		break;
			
		case 39:
			if(reg >= 33)
				CsTemp = 40;
			else
				CsTemp = 39;
		break;
			
		case 41:
			if(reg <= 34)
				CsTemp = 40;
			else
				CsTemp = 41;
		break;
			
		case 42:
			if(reg >= 37)
				CsTemp = 43;
			else
				CsTemp = 42;
		break;
			
		case 44:
			if(reg <= 37)
				CsTemp = 43;
			else
				CsTemp = 44;
		break;
			
		case 46:
			if(reg >= 38)
				CsTemp = 47;
			else
				CsTemp = 46;
		break;
			
		case 48:
			if(reg <= 38)
				CsTemp = 47;
			else
				CsTemp = 48;
		break;
			
		case 50:
			if(reg >= 42)
			{
				CsTemp = 51;
				if(GetReg(TOUCH_I2C_Address, reg_addr1,&reg) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;
				if(SetReg(TOUCH_I2C_Address, reg_addr1, ((reg & FM5114_TOUCH_REGVAL[(channel*14)+7]) | FM5114_TOUCH_REGVAL[(channel*14)+11])) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;				
			}
			else
				CsTemp = 50;
		break;
			
		case 52:
			if(reg <= 21)
				CsTemp = 51;
			else
				CsTemp = 52;
		break;
			
		case 54:
			if(reg >= 23)
				CsTemp = 55;
			else
				CsTemp = 54;
		break;
			
		case 56:
			if(reg <= 22)
				CsTemp = 55;
			else
				CsTemp = 56;
		break;
			
		case 58:
			if(reg >= 22)
				CsTemp = 59;
			else
				CsTemp = 58;
		break;
			
		case 60:
			if(reg <= 24)
				CsTemp = 59;
			else
				CsTemp = 60;
		break;
	}
	
	Cs = CsTemp;
	CsButton[channel] = Cs; 
	
	
	if(Sensitivity==0x01)                    
	{
		N_Sens=FM5114_TOUCH_SEN_LUT1[Cs-1];
	}
	else if(Sensitivity==0x02)
	{
		N_Sens=FM5114_TOUCH_SEN_LUT2[Cs-1];
	}
	else
	{
		N_Sens=FM5114_TOUCH_CS_LUT[Cs-1];
	}
	
	if(GetReg(TOUCH_I2C_Address, reg_addr2, &reg) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	if(SetReg(TOUCH_I2C_Address, reg_addr2, (reg & FM5114_TOUCH_REGVAL[(channel*14)+12]) + ((N_Sens - 8) << FM5114_TOUCH_REGVAL[(channel*14)+13])) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	
	if(N_Sens == 8)      
	{
		if(SetReg(TOUCH_I2C_Address, 0x8A, 0x00) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, 0x8B, 0xDA) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}
	else if(N_Sens == 9)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0x01) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0xB3) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}
	else if(N_Sens == 10)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0x03) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0x66) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}	
	else if(N_Sens == 11)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0x06) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0xCD) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}	
	else if(N_Sens == 12)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0x0D) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0x9A) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}
	else if(N_Sens == 13)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0x1B) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0x33) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}
	else if(N_Sens == 14)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0x36) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0x66) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}
	else if(N_Sens == 15)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0x6C) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0xCD) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}
	else if(N_Sens == 16)
	{
		 if(SetReg(TOUCH_I2C_Address, 0x8A, 0xD9) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
		 if(SetReg(TOUCH_I2C_Address, 0x8B, 0x9A) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
	}
	if(FM5114_Touch_Calibrate() != FM5114_SUCCESS)
		return FM5114_COMM_ERR;

	for(i = 0; i < 25; i++)
	{
		DDL_Delay1ms(2);
		if(GetReg(TOUCH_I2C_Address, 0x87, &reg) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
		
		if((reg&0x80)==0x80)
		{
				break;
		}	
	}

	if((reg & 0x80) == 0x80)
	{
		if(GetReg(TOUCH_I2C_Address, 0x88, &reg) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(GetReg(TOUCH_I2C_Address, reg_addr3, &reg1) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, reg_addr3,((reg1&0x80)+(reg&0x7F))) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}	
	else
	{
		return FM5114_CALI_ERR;
	}
//	if(FM5114_Touch_SoftReset() != FM5114_SUCCESS)			     //soft_reset=1
//		return FM5114_COMM_ERR;
	
		if(SENSE_MODE == 1)
	{
		if(CsButton[channel] == 7 || CsButton[channel] == 14 || CsButton[channel] == 28 || CsButton[channel] == 29 || CsButton[channel] < 5)
		{
			if(GetReg(TOUCH_I2C_Address, reg_addr3, &reg) != FM5114_SUCCESS)  
				return FM5114_COMM_ERR;
			if(SetReg(TOUCH_I2C_Address, reg_addr3,((reg & 0x80) + ((reg << 1) >> 2))) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
			if(GetReg(TOUCH_I2C_Address, reg_addr4, &reg1) != FM5114_SUCCESS)  
				return FM5114_COMM_ERR;
			if(SetReg(TOUCH_I2C_Address, reg_addr4,((reg1 & 0x80)+ (reg & 0x7F) - ((reg << 1) >> 2))) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
		}
		else if(Sensitivity == 0x02 && CsButton[channel] >= 56)
		{
			if(GetReg(TOUCH_I2C_Address, reg_addr3, &reg) != FM5114_SUCCESS)  
				return FM5114_COMM_ERR;
			if(SetReg(TOUCH_I2C_Address, reg_addr3,((reg & 0x80) + ((reg << 1) >> 2))) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
			if(GetReg(TOUCH_I2C_Address, reg_addr4, &reg1) != FM5114_SUCCESS)  
				return FM5114_COMM_ERR;
			if(SetReg(TOUCH_I2C_Address, reg_addr4,((reg1 & 0x80)+ (reg & 0x7F) - ((reg << 1) >> 2))) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;		
		}
		else
		{
		Target_temp = (Target_Signal_High[CsButton[channel] - 5] << 8) + (Target_Signal_Low[CsButton[channel] - 5]);
			Target_temp = Target_temp/Sensitivity;
			if(SetReg(TOUCH_I2C_Address, 0x8A, (Target_temp & 0xFF00) >> 8) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;
			if(SetReg(TOUCH_I2C_Address, 0x8B, Target_temp & 0x00FF) != FM5114_SUCCESS)
			 return FM5114_COMM_ERR;	
			
		if(FM5114_Touch_Calibrate() != FM5114_SUCCESS)
			return FM5114_COMM_ERR;


			for(i = 0; i < 25; i++)
			{
				DDL_Delay1ms(2);
				if(GetReg(TOUCH_I2C_Address, 0x87, &reg) != FM5114_SUCCESS)
						return FM5114_COMM_ERR;
				
				if((reg&0x80)==0x80)
				{
					break;
				}	
			}

			if(GetReg(TOUCH_I2C_Address, 0x88, &reg) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
			if(reg%2 == 0)
			{
				reg = reg >> 1;
			}
			else
			{
				reg = (reg + 1) >> 1;
			}
			if(GetReg(TOUCH_I2C_Address, reg_addr4, &reg1) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
			if(SetReg(TOUCH_I2C_Address, reg_addr4,((reg1&0x80)+(reg&0x7F))) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
			if(GetReg(TOUCH_I2C_Address, reg_addr3, &reg1) != FM5114_SUCCESS)  
				return FM5114_COMM_ERR;
			if(SetReg(TOUCH_I2C_Address, reg_addr3, (reg1 - (reg&0x7F))) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;			
		}

		if(GetReg(TOUCH_I2C_Address, reg_addr2, &reg) != FM5114_SUCCESS)  
			return FM5114_COMM_ERR;
		if((reg & FM5114_TOUCH_SENSEMODE_REGVAL[(channel * 3) + 0]) != 0)
		{
			reg = reg - FM5114_TOUCH_SENSEMODE_REGVAL[(channel * 3) + 1];
			if(SetReg(TOUCH_I2C_Address, reg_addr2, reg) != FM5114_SUCCESS)
				return FM5114_COMM_ERR;
		}
		if(GetReg(TOUCH_I2C_Address, reg_addr1 , &reg) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(SetReg(TOUCH_I2C_Address, reg_addr1, reg | FM5114_TOUCH_SENSEMODE_REGVAL[(channel * 3) + 2]) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
	}
	

	return FM5114_SUCCESS;
}



unsigned char FM5114_Touch_Scan_Start(void)
{
	unsigned char reg_data;
	if(GetReg(TOUCH_I2C_Address,Scan_Ctrl,&reg_data) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	if(SetReg(TOUCH_I2C_Address,Scan_Ctrl, reg_data | 0x80) != FM5114_SUCCESS)
		return FM5114_COMM_ERR;
	else
		return FM5114_SUCCESS;
}


/****************************************************************
 * �������ƣ�Button_Init_Regcopy								
 * ����˵���������Զ���У�������ƣ����ڿ��ٳ�ʼ��ģʽ
 * ��ڲ�����			
 * ���ڲ�����						
 * ʱ��: 													
 * ����: ��Ԫ													
 ****************************************************************/
unsigned char Button_Init_Regcopy(void)
{
	unsigned char reg,i;
	
	for(i = 0;i < 14;i++)
	{
		log_printf ("i =%x \r\n",i);
		GetReg(TOUCH_I2C_Address, 0x9D +(i/2), &reg);			
		Button_InitReg[i][0] = reg;
		log_printf ("S1=%x \r\n",reg);
		
		GetReg(TOUCH_I2C_Address, 0x95 +(i/2), &reg);			
		Button_InitReg[i][1] = reg;
		log_printf ("S2=%x \r\n",reg);
		
		GetReg(TOUCH_I2C_Address, 0xA6 + i, &reg);		
		Button_InitReg[i][2] = reg;	
		log_printf ("S3=%x \r\n",reg);
		
		GetReg(TOUCH_I2C_Address, 0xB7 + i, &reg);		
		Button_InitReg[i][3] = reg;	
		log_printf ("S4=%x \r\n",reg);
	}
	
	
	//Button_InitReg������Ա��浽MCU��Flash
	return FM5114_SUCCESS;
}

/****************************************************************
 * �������ƣ�Button_Init_FastMode								
 * ����˵�����������ٳ�ʼ��ģʽ
 * ��ڲ�����				
 * ���ڲ�����						
 * ʱ��: 													
 * ����: ��Ԫ													
 ****************************************************************/
unsigned char Button_Init_FastMode(void)
{
	unsigned char i,result;	
		
	result = FM5114_Touch_SoftReset();
	if(result != FM5114_SUCCESS)		 
		log_printf("FM5114 TOUCH SoftReset Error\r\n");
	
	result = FM5114_Touch_Trim_Load();
	if(result != FM5114_SUCCESS)		 
	log_printf("FM5114 TOUCH Trim Load Error\r\n");

	for(i = 0;i < 14;i++)
	{
		SetReg(TOUCH_I2C_Address, 0x9D +(i/2), Button_InitReg[i][0]);			
		SetReg(TOUCH_I2C_Address, 0x95 +(i/2), Button_InitReg[i][1]);			
		SetReg(TOUCH_I2C_Address, 0xA6 + i, Button_InitReg[i][2]);	
		SetReg(TOUCH_I2C_Address, 0xB7 + i, Button_InitReg[i][3]);		
	}
	
	return FM5114_SUCCESS;
}



unsigned char  FM5114_Touch_Init(unsigned char InitMode, unsigned char Sensi ,unsigned int ButtonChannel, unsigned char group_button_en, unsigned char Lpcd_Timer_en)
{	
		unsigned char reg = 0;
	unsigned int channel = ButtonChannel;
	unsigned char i;
	
//=============2020 1108  V8.0���� =================================
//=============
//��ͨ����ʼ����Ĭ���ڲ��ӵ�
//ɨ��ĳͨ��ʱ��ͨ���ڲ���ضϿ�������ͨ������������ӣ�
//ɨ��ĳͨ�������󣬸�ͨ�����Զ�������ӡ�	
//�޸�Ŀ�ģ���������ͨ��������ɨ���ͨ��Ӱ�졣
		SetReg(TOUCH_I2C_Address, 0xB4, 0x55);   
		SetReg(TOUCH_I2C_Address, 0xB5, 0x55);   
		SetReg(TOUCH_I2C_Address, 0xC5, 0x55);   
		SetReg(TOUCH_I2C_Address, 0xC6, 0x05);   
//===================================================================
	
	
/*�ڵ�һ���ϵ�ʱ����������Զ���У��ʼ����ʽ*/
	if(FM5114_Touch_SoftReset() != FM5114_SUCCESS)			                                 //soft_reset=1
	return FM5114_COMM_ERR;
	
	if(InitMode == INTT_MODE)
	{	
			for(i=0;i<14;i++)
			{
				if(((channel >> i) & 0x01) != 0)
				{
					if(FM5114_Touch_Button_Auto_Tuning(Sensi, i) != FM5114_SUCCESS)                       //������У����
						return ERROR;			
				}	
				log_printf ("Cs %x=%x\r\n",i,CsButton[i]);
			}			
			
	if(FM5114_Touch_SoftReset() != FM5114_SUCCESS)			                                 //soft_reset=1
		return FM5114_COMM_ERR;
	
				//�Զ���У��ʼ���ļĴ����������ƣ���Щ������Ҫ����Flash����Ϊ���ٳ�ʼ���ı��ݡ������洢�������û����б�д��
		Button_Init_Regcopy(); 
	}
	else
	{
		Button_Init_FastMode();//Reload
	}


	/*  �ж�����ܽ����� */
	SetReg(TOUCH_I2C_Address, 0x43, 0x02);                //0x02�������������0x03����OC���
	
	SetReg(TOUCH_I2C_Address, Irq_Ctrl, 0x81);            //��ƽ�жϣ�������release�ж�ʹ��
	
	SetReg(TOUCH_I2C_Address, Button_Channel_Config1, (channel&0x00FF));      //s0-s7_en		
	SetReg(TOUCH_I2C_Address, Button_Channel_Config2, ((channel&0xFF00)>>8)); //s8-s11_en
		
	/* ����ֵ �������źű仯��������*/
	SetReg(TOUCH_I2C_Address, Button0_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button1_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button2_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button3_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button4_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button5_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button6_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button7_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button8_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button9_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button10_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button11_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button12_Touch_Thrsh, 0x80);
	SetReg(TOUCH_I2C_Address, Button13_Touch_Thrsh, 0x80);

	SetReg(TOUCH_I2C_Address, Global_Hys_Thrsh, 0x40|GLOBAL_HYS);     //����������������ֵ����
		
	SetReg(TOUCH_I2C_Address, Global_Noise_Thrsh, 0x1E);   //����������������ֵΪ30
		
	SetReg(TOUCH_I2C_Address, Global_NegNoise_Thrsh, 0x28);//������������������ֵΪ40
		
	SetReg(TOUCH_I2C_Address, Button_Debounce, 0x11);      //����������������Ϊ1
		
	SetReg(TOUCH_I2C_Address, Button_NegNoise_Num_Thrsh, 0x0A);//������������ʱ��λʱ�䣬10��ɨ������
		
	SetReg(TOUCH_I2C_Address, 0x0B, 0x50);                 //����������������������
		
	SetReg(TOUCH_I2C_Address, 0x0C, 0x20);
		
	GetReg(TOUCH_I2C_Address, Scan_Period_Config , &reg);
		
	SetReg(TOUCH_I2C_Address, Scan_Period_Config, (reg&0xE0)|SCAN_PERIOD);//���ٰ���ɨ��ģʽ����ɨ������
		
	
//	GetReg(TOUCH_I2C_Address, Scan_Ctrl , &reg);
//	SetReg(TOUCH_I2C_Address, Scan_Ctrl, reg&0xEF);       //��������ر�
	
	
	GetReg(TOUCH_I2C_Address, Scan_Ctrl , &reg);		
	SetReg(TOUCH_I2C_Address, Scan_Ctrl, reg | 0x10);        //�������⿪��
		
	
	if(group_button_en == 1)
	{	
		GetReg(TOUCH_I2C_Address, Button_Channel_Config2, &reg);
			 	
		SetReg(TOUCH_I2C_Address, Button_Channel_Config2, reg|0xC0);//ʹ��S5��S6Ϊgroup���ܰ���
			
	}	
	if(Lpcd_Timer_en == 1)
	{	
		GetReg(TOUCH_I2C_Address, LPCD_Timer_Config, &reg);
			 	
		SetReg(TOUCH_I2C_Address, LPCD_Timer_Config, reg|0x38);//ʹ��LPCD��ʱ����
			
		
		GetReg(TOUCH_I2C_Address, Gpio1_Mode_Config, &reg);
			 	
		SetReg(TOUCH_I2C_Address, Gpio1_Mode_Config, (reg&0x1C)|0x62);//ʹ��LPCD��ʱ����
					
	}
			
	SetReg(TOUCH_I2C_Address, Timeout_Config, 0x19|(TOUCH_TIMEOUT<<6));//2D�Ĵ���	bit6~bit7��������ʱʱ��00:2s,01:5s,10:10s,11:15s
	
	log_printf("-> Tuning OK!\r\n");
	return FM5114_SUCCESS;
	
}

unsigned char SenseModeInit(unsigned int channel)
{
	unsigned char reg,reg1;
	unsigned char reg_addr1, reg_addr2, reg_addr3, reg_addr4;
	
	reg_addr1 = 0x9D+(channel/2);
	reg_addr2 = 0x95+(channel/2);	
	reg_addr3 = 0xA6+channel;
	reg_addr4 = 0xB7+channel;
	
	
	GetReg(TOUCH_I2C_Address,reg_addr3, &reg) ;
	
	SetReg(TOUCH_I2C_Address, reg_addr3,((reg & 0x80) + ((reg << 1) >> 2))) ;
		
	GetReg(TOUCH_I2C_Address, reg_addr4, &reg1);
	
	SetReg(TOUCH_I2C_Address, reg_addr4,((reg1 & 0x80)+ (reg & 0x7F) - ((reg << 1) >> 2))) ;	
	
	GetReg(TOUCH_I2C_Address, reg_addr2, &reg) ;
	if((reg & FM5114_TOUCH_SENSEMODE_REGVAL[(channel * 3) + 0]) != 0)
	{
		reg = reg - FM5114_TOUCH_SENSEMODE_REGVAL[(channel * 3) + 1];
		SetReg(TOUCH_I2C_Address, reg_addr2, reg) ;
	}
	GetReg(TOUCH_I2C_Address, reg_addr1 , &reg);
	SetReg(TOUCH_I2C_Address,reg_addr1, reg | FM5114_TOUCH_SENSEMODE_REGVAL[(channel * 3) + 2]) ;
	
	return FM5114_SUCCESS;		
				
}


unsigned char FM5114_Sensor_Debug(unsigned char channel)
{
	unsigned char reg1,reg2,reg3,reg4;
	unsigned char signal,base;	
	unsigned long reg5,reg6,touchrise;


		switch(channel)
		{
			case 0://'LOCK'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x00) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;			
			break;
			
			case 1://'1'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x01) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 2://'4'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x02) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 3://'7'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x03) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 4://'*'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x04) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 5://'0'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x05) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 6://'8'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x06) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 7://'5'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x07) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 8://'2'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x08) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 9://'BELL'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x09) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 10://'3'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x0A) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 11://'6'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x0B) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 12://'9'
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x0C) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
			
			case 13://#
				if(SetReg(TOUCH_I2C_Address, 0x7E, 0x0D) != FM5114_SUCCESS)
					return FM5114_COMM_ERR;					
			break;
		}
			DDL_Delay1ms(500);
		if(GetReg(TOUCH_I2C_Address, 0x81, &reg1) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(GetReg(TOUCH_I2C_Address, 0x82, &reg2) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(GetReg(TOUCH_I2C_Address, 0x7F, &reg3) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;
		if(GetReg(TOUCH_I2C_Address, 0x80, &reg4) != FM5114_SUCCESS)
			return FM5114_COMM_ERR;

		
		
		log_printf ("-> Channel = %x\r\n",channel);
		
		log_printf ("-> Signal = 0x");
		signal = reg1;log_printf ("%x ",signal);
		signal = reg2;log_printf ("%x ",signal);
	
		
		log_printf(", Base = 0x");
		base = reg3;log_printf ("%x ",base);
		base = reg4;log_printf ("%x ",base);


		reg5 = (reg1 << 8) + reg2;
		reg6 = (reg3 << 8) + reg4;
		log_printf(", Rise = 0x");
		if(reg5 > reg6)
		{
			touchrise = reg5 - reg6;
			signal = (touchrise & 0xFF00) >> 8;
			log_printf ("%x ",signal);
			signal = touchrise & 0x00FF;
			log_printf ("%x ",signal);
		}
		else
		{
			touchrise = reg6 - reg5;
			signal = ((touchrise & 0xFF00) >> 8);
			log_printf ("%x ",signal);
			signal = touchrise & 0x00FF;
			log_printf ("%x ",signal);
		
		}
		log_printf("\r\n");

		DDL_Delay1ms(300);
	
	return FM5114_SUCCESS;
}



