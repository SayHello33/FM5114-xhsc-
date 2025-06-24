#ifndef _LPCD_API_H
#define _LPCD_API_H




typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
unsigned char TYPE_A_EVENT(void);
extern void Lpcd_Adc_Event(void);
unsigned char ReaderB_CardActivate(void);
extern unsigned char Lpcd_Init_Register(void);
unsigned char Configure_LPCD_For_BCard(void);
extern void Lpcd_Set_Mode(unsigned char mode);
unsigned char Lpcd_Card_Event(void);
void Lpcd_Get_ADC_Value(void);
void CheckFM5114Presence(void);void I2C_ScanAllDevices(void);
void Test_LPCD_Performance();
#endif


