
#ifndef _FM5114_H
#define _FM5114_H



#define READER_I2C_Address 0x28
#define TOUCH_I2C_Address 0x32
#define I2C_TIMEOUT 0xFFFF


#define TX1_TX2_CW_DISABLE 0
#define TX1_CW_ENABLE 1
#define TX2_CW_ENABLE 2
#define TX1_TX2_CW_ENABLE 3



/*FM5114配置宏定义*/
#define SENSE                0x01// 1：高灵敏度 ~ 4：低灵敏度
#define BUTTON_CHANNEL       0x3FFF //3FFF为14通道使能
#define SENSE_MODE           1			//增强模式，默认值为1，使能增强模式
#define GROUP_BUTTON_EN      0			//S5,S6通道组合按键功能
#define SCAN_PERIOD 14 //0:20ms 1:40ms 2:60ms...14:300ms...  24:500ms  25~31:1S
#define GLOBAL_HYS	10 //设置范围0~31  按键传感器触摸阈值迟滞
#define LPCD_TIMER_EN  1
#define TOUCH_TIMEOUT 0 //0~3 0:2s,1:5s,2:10s,3:15s	
#define INTT_MODE 1
#define FAST_MODE 0
extern unsigned char Touch_IRQ;
extern unsigned char LPCD_IRQ;
extern unsigned char GetReg(unsigned char device_address,unsigned char reg_address,unsigned char *reg_data);
extern unsigned char SetReg(unsigned char device_address,unsigned char reg_address,unsigned char reg_data);

extern void FM175XX_HPD_I2C(unsigned char mode);
extern void I2C_Write_FIFO(unsigned char reglen,unsigned char* regbuf); //IIC接口连续写FIFO 
extern void I2C_Read_FIFO(unsigned char reglen,unsigned char* regbuf);  //IIC接口连续读FIFO

extern void Group_Handler(unsigned char Channel, unsigned char Condition);

extern unsigned char Scan_Start(void);
extern void SetCW(unsigned char cw_mode);
extern void FM5114_Touch_Reset(void);
extern void FM5114_Reader_Reset(void);
extern unsigned char FM5114_Reader_SoftReset(void);
extern void FM175XX_HPD(unsigned char mode);

extern void ModifyReg(unsigned char device_address,unsigned char reg_address,unsigned char mask,unsigned char set);

extern void Write_FIFO(unsigned char reglen,unsigned char * regbuf);
extern void Read_FIFO(unsigned char reglen,unsigned char * regbuf);
extern void Clear_FIFO(void);
extern unsigned char GetReg_Ext(unsigned char device_address,unsigned char ext_reg_address,unsigned char* ext_reg_data);
extern unsigned char SetReg_Ext(unsigned char device_address,unsigned char ext_reg_address,unsigned char ext_reg_data);
extern void ModifyReg(unsigned char device_address,unsigned char reg_address,unsigned char mask,unsigned char set);

extern unsigned char FM5114_Touch_SoftReset(void);
extern unsigned char FM5114_Touch_Trim_Load(void);
extern unsigned char FM5114_Touch_Init(unsigned char InitMode, unsigned char Sensi ,unsigned int ButtonChannel, unsigned char group_button_en, unsigned char Lpcd_Timer_en);
extern unsigned char FM5114_Sensor_Debug(unsigned char channel);
extern unsigned char Button_Init_FastMode(void);
extern unsigned char Button_Init_Regcopy(void);
#endif





